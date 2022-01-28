#include <random>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/FFTConvolution.h>
#include <pcl/IntegerResample.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleLinearTransform.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/StandardStatus.h>
#include <pcl/VariableShapeFilter.h>
#include <pcl/View.h>

#include "DustFreeInstance.h"
#include "DustFreeParameters.h"

namespace pcl
{

template <class P>
class DustFreeThread : public Thread
{
public:
    typedef void(*LineProcessFunc)(DustFreeInstance* superFlat, ReferenceArray<GenericImage<P>>&, GenericImage<P>&, int, int);

    DustFreeThread(int id, LineProcessFunc lineProcessFunc, const AbstractImage::ThreadData& data, DustFreeInstance* superFlat,
        ReferenceArray<GenericImage<P>>& srcImages, GenericImage<P>& dstImage,
        int channel, int firstRow, int endRow)
        : m_id(id)
        , m_lineProcessFunc(lineProcessFunc)
        , m_data(data)
        , m_superFlat(superFlat)
        , m_srcImages(srcImages)
        , m_dstImage(dstImage)
        , m_channel(channel)
        , m_firstRow(firstRow)
        , m_endRow(endRow)
        , m_threadErrorMsg("")
    {
    }

    void Run() override
    {
        INIT_THREAD_MONITOR();
        try {
            for (int y = m_firstRow; y < m_endRow; y++) {
                m_lineProcessFunc(m_superFlat, m_srcImages, m_dstImage, y, m_channel);
                UPDATE_THREAD_MONITOR(65536);
            }
        }
        catch (...) {
            volatile AutoLock lock(m_data.mutex);
            try {
                throw;
            }
            catch (Exception & x) {
                m_threadErrorMsg = x.Message();
            }
            catch (std::bad_alloc&) {
                m_threadErrorMsg = "Out of memory";
            }
            catch (...) {
                m_threadErrorMsg = "Unknown error";
            }
        }
    }

    static void dispatch(LineProcessFunc lineProcessFunc, DustFreeInstance* superFlat,
        ReferenceArray<GenericImage<P>>& srcImages, GenericImage<P>& dstImage, int channel)
    {
        Array<size_type> L = Thread::OptimalThreadLoads(dstImage.Height(), 1, PCL_MAX_PROCESSORS);
        ReferenceArray<DustFreeThread> threads;
        AbstractImage::ThreadData data(dstImage, dstImage.NumberOfPixels());
        for (int i = 0, n = 0; i < int(L.Length()); n += int(L[i++]))
            threads << new DustFreeThread(i, lineProcessFunc, data, superFlat, srcImages, dstImage, channel, n, n + int(L[i]));
        AbstractImage::RunThreads(threads, data);
        for (DustFreeThread& t : threads)
            if (t.m_threadErrorMsg != "")
                throw Error(t.m_threadErrorMsg);
        threads.Destroy();
        dstImage.Status() = data.status;
    }

private:
    int m_id;
    LineProcessFunc m_lineProcessFunc;
    const AbstractImage::ThreadData& m_data;
    DustFreeInstance* m_superFlat;
    ReferenceArray<GenericImage<P>>& m_srcImages;
    GenericImage<P>& m_dstImage;
    int m_channel;
    int m_firstRow;
    int m_endRow;
    String m_threadErrorMsg;
};

DustFreeInstance::DustFreeInstance(const MetaProcess* m)
    : ProcessImplementation(m)
    , starDetectionSensitivity(TheDFStarDetectionSensitivityParameter->DefaultValue())
    , starDiffusionDistance(TheDFStarDiffusionDistanceParameter->DefaultValue())
    , dustMaskViewId()
    , smoothness(TheDFSmoothnessParameter->DefaultValue())
    , downsample(TheDFDownsampleParameter->DefaultValue())
    , testSkyDetection(TheDFTestSkyDetectionParameter->DefaultValue())
{
}

DustFreeInstance::DustFreeInstance(const DustFreeInstance& x)
    : ProcessImplementation(x)
{
    Assign(x);
}

void DustFreeInstance::Assign(const ProcessImplementation& p)
{
    const DustFreeInstance* x = dynamic_cast<const DustFreeInstance*>(&p);
    if (x != nullptr) {
        starDetectionSensitivity = x->starDetectionSensitivity;
        starDiffusionDistance = x->starDiffusionDistance;
        smoothness = x->smoothness;
    }
}

bool DustFreeInstance::IsHistoryUpdater(const View& view) const
{
    return true;
}

UndoFlags DustFreeInstance::UndoMode(const View&) const
{
    return UndoFlag::PixelData;
}

bool DustFreeInstance::CanExecuteOn(const View& view, pcl::String& whyNot) const
{
    if (view.Image().IsComplexSample())
    {
        whyNot = "DustFree cannot be executed on complex images.";
        return false;
    } else if (!view.Image().IsFloatSample()) {
        whyNot = "DustFree can only be executed on float images.";
        return false;
    }

    return true;
}

bool DustFreeInstance::ExecuteOn(View& view)
{
    AutoViewLock lock(view);

    StandardStatus status;
    Console console;

    console.EnableAbort();

    ImageVariant image = view.Image();

    if (image.IsComplexSample() || !view.Image().IsFloatSample())
        return false;

    View dustMaskView;
    if (dustMaskViewId.IsEmpty()) {
        throw Error("No dust mask selected");
    } else {
        dustMaskView = View::ViewById(dustMaskViewId);
        if (dustMaskView.IsNull())
            throw Error("No such view (dust mask): " + dustMaskViewId);
    }

    image.SetStatusCallback(&status);

    // Downsample
    ImageVariant downImage;
    downImage.CopyImage(image);
    downImage.EnsureUniqueImage();
    downImage.SetStatusCallback(nullptr);
    if (downsample > 1) {
        IntegerResample ir(-downsample);
        ir >> downImage;
    }

    // Star detection
    ImageVariant starMask;
    starMask.CopyImage(downImage);
    starMask.EnsureUniqueImage();
    starMask.SetStatusCallback(nullptr);
    image.Status().Initialize("Performing star detection", 3);
    MultiscaleLinearTransform mlt(4);
    mlt << starMask;
    mlt.DisableLayer(0);
    mlt.DisableLayer(4);
    mlt >> starMask;
    starMask.Truncate(0.0f, 1.0f);
    starMask.Normalize();
    image.Status() += 1;

    MorphologicalTransformation mf;
    mf.SetStructure(BoxStructure(3));
    mf.SetOperator(MedianFilter());
    mf >> starMask;
    starMask.Binarize(pcl::Pow10(-starDetectionSensitivity));
    image.Status() += 1;

    MorphologicalTransformation df;
    df.SetStructure(CircularStructure(2 * starDiffusionDistance + 3));
    df.SetOperator(DilationFilter());
    df >> starMask;
    image.Status() += 1;
    starMask.Invert();

    // Extract background
    image.Status().Initialize("Extracting background", image.NumberOfChannels() + 5);
    ImageVariant mask;
    mask.CopyImage(starMask);
    mask.EnsureUniqueImage();
    mask.SetStatusCallback(nullptr);
    ImageVariant bg;
    bg.CopyImage(downImage);
    bg.EnsureUniqueImage();
    bg.SetStatusCallback(nullptr);
    bg.Multiply(mask);
    image.Status() += 1;

    if (testSkyDetection) {
        IsoString id = view.FullId() + "_bg";
        ImageWindow OutputWindow = ImageWindow(bg.Width(), bg.Height(), bg.NumberOfChannels(), bg.BitsPerSample(), true, bg.IsColor(), true, id);
        if (OutputWindow.IsNull())
            throw Error("Unable to create image window: " + id);
        OutputWindow.MainView().Lock();
        OutputWindow.MainView().Image().CopyImage(bg);
        OutputWindow.MainView().Unlock();
        OutputWindow.Show();

        return true;
    }

    // Inpaint
    ImageVariant bg0;
    bg0.CopyImage(bg);
    bg0.EnsureUniqueImage();
    bg0.SetStatusCallback(nullptr);
    image.Status() += 1;
    if (image.BitsPerSample() == 32) {
        ReferenceArray<GenericImage<FloatPixelTraits>> input;
        input << &static_cast<Image&>(*bg);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            DustFreeThread<FloatPixelTraits>::dispatch(inpaint<FloatPixelTraits>, this, input, static_cast<Image&>(*bg0), c);
            image.Status() += 1;
        }
    } else if (image.BitsPerSample() == 64) {
        ReferenceArray<GenericImage<DoublePixelTraits>> input;
        input << &static_cast<DImage&>(*bg);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            DustFreeThread<DoublePixelTraits>::dispatch(inpaint<DoublePixelTraits>, this, input, static_cast<DImage&>(*bg0), c);
            image.Status() += 1;
        }
    }

    // Apply dust mask and inpaint
    ImageVariant dustMask;
    {
        AutoViewLock viewLock(dustMaskView);
        dustMask.CopyImage(dustMaskView.Image());
        dustMask.EnsureUniqueImage();
        dustMask.SetStatusCallback(nullptr);
    }
    if ((dustMask.Width() != mask.Width()) || (dustMask.Height() != mask.Height())) {
        BicubicFilterPixelInterpolation bs(2, 2, CubicBSplineFilter());
        Resample rs(bs, double(mask.Width()) / dustMask.Width(), double(mask.Height()) / dustMask.Height());
        rs >> dustMask;
    }
    if ((dustMask.NumberOfChannels() != mask.NumberOfChannels()) && (dustMask.ColorSpace() == ColorSpace::Gray))
        dustMask.SetColorSpace(mask.ColorSpace());

    if (dustMask.NumberOfChannels() != mask.NumberOfChannels())
        throw Error("Number of channels of non-sky mask mismatch with the image being processed.");

    dustMask.Binarize(0.5);
    bg.Multiply(dustMask.Invert());

    ImageVariant bg1;
    bg1.CopyImage(bg);
    bg1.EnsureUniqueImage();
    bg1.SetStatusCallback(nullptr);
    image.Status() += 1;
    if (image.BitsPerSample() == 32) {
        ReferenceArray<GenericImage<FloatPixelTraits>> input;
        input << &static_cast<Image&>(*bg);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            DustFreeThread<FloatPixelTraits>::dispatch(inpaint<FloatPixelTraits>, this, input, static_cast<Image&>(*bg1), c);
            image.Status() += 1;
        }
    } else if (image.BitsPerSample() == 64) {
        ReferenceArray<GenericImage<DoublePixelTraits>> input;
        input << &static_cast<DImage&>(*bg);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            DustFreeThread<DoublePixelTraits>::dispatch(inpaint<DoublePixelTraits>, this, input, static_cast<DImage&>(*bg1), c);
            image.Status() += 1;
        }
    }

    // Blur
    VariableShapeFilter H2(pcl::Pow(1.7f, smoothness), 5.0f, 0.01f, 1.0f, 0.0f);
    FFTConvolution(H2) >> bg0;
    FFTConvolution(H2) >> bg1;
    if (downsample > 1) {
        BicubicFilterPixelInterpolation bs(2, 2, CubicBSplineFilter());
        Resample rs(bs, double(image.Width()) / bg.Width(), double(image.Height()) / bg.Height());
        rs >> bg0;
        rs >> bg1;
    }
    image.Status() += 1;
    image.Status().Complete();

    image.Subtract(bg0);
    image.Add(bg1);

    return true;
}

void* DustFreeInstance::LockParameter(const MetaParameter* p, size_type /*tableRow*/)
{
    return 0;
}

template <class P>
void DustFreeInstance::inpaint(DustFreeInstance* superFlat, ReferenceArray<GenericImage<P>>& inputs, GenericImage<P>& output, int y, int channel)
{
    typename P::sample* pOut = output.ScanLine(y, channel);
    GenericImage<P>& input = inputs[0];
    const int n = 32;
    const int distance = pcl::Max(output.Width(), output.Height());
    std::minstd_rand rg(output.Height() * channel + y);
    std::uniform_real_distribution<float> ud(-0.5f, 0.5f);

    for (int x = 0; x < output.Width(); x++) {
        typename P::sample in = input(x, y, channel);
        if (in > 0.0) {
            pOut[x] = in;
            continue;
        }
        typename P::sample p = 0.0;
        float w0 = 0.0f;
        for (int i = 0; i < n; i++) {
            float rad = pcl::Pi() * 2.0f * i / n;
            float step_x = pcl::Cos(rad);
            float step_y = pcl::Sin(rad);
            for (int j = 1; j < distance; j = (j < 16) ? j + 1 : j * 1.1f) {
                if ((j < 64) && (i % 2 != 0))
                    continue;
                float w = 1.0f / float(j);
                if (w < w0 * 0.01f)
                    continue;
                float rx = ud(rg) * j * 6.0f / n;
                float ry = ud(rg) * j * 6.0f / n;
                rx = 0.0;
                ry = 0.0;
                int ix = int(x + step_x * j + rx + 0.5f);
                int iy = int(y + step_y * j + ry + 0.5f);
                if (ix < 0)
                    ix = 0;
                else if (ix >= input.Width())
                    ix = input.Width() - 1;
                if (iy < 0)
                    iy = 0;
                else if (iy >= input.Height())
                    iy = input.Height() - 1;
                typename P::sample in = input(ix, iy, channel);
                if (in == 0.0)
                    continue;
                p += in * w;
                w0 += w;
                break;
            }
        }
        if (w0 > 0.0f)
            pOut[x] = p / w0;
        else
            pOut[x] = 0.0;
    }
}

}	// namespace pcl