#ifndef __DustFreeInstance_h
#define __DustFreeInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_enum

namespace pcl
{

class DustFreeInstance : public ProcessImplementation
{
public:
    DustFreeInstance(const MetaProcess*);
    DustFreeInstance(const DustFreeInstance&);

    void Assign(const ProcessImplementation&) override;
    bool IsHistoryUpdater(const View& v) const override;
    UndoFlags UndoMode(const View&) const override;
    bool CanExecuteOn(const View&, pcl::String& whyNot) const override;
    bool ExecuteOn(View&) override;
    void* LockParameter(const MetaParameter*, size_type tableRow) override;

private:
    float starDetectionSensitivity;
    int starDiffusionDistance;
    String dustMaskViewId;
    float smoothness;
    int downsample;
    bool testSkyDetection;

    template <class P>
    static void inpaint(DustFreeInstance* dustFree, ReferenceArray<GenericImage<P>>& inputs, GenericImage<P>& output, int y, int channel);

    friend class DustFreeProcess;
    friend class DustFreeInterface;
};

}	// namespace pcl

#endif	// __DustFreeInstance_h
