#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

#include "DustFreeProcess.h"
#include "DustFreeParameters.h"
#include "DustFreeInstance.h"
#include "DustFreeInterface.h"

namespace pcl
{

DustFreeProcess* TheDustFreeProcess = nullptr;

DustFreeProcess::DustFreeProcess()
{
    TheDustFreeProcess = this;

    // Instantiate process parameters
    new DFStarDetectionSensitivity(this);
    new DFStarDiffusionDistance(this);
    new DFSmoothness(this);
    new DFDownsample(this);
    new DFTestSkyDetection(this);
}

IsoString DustFreeProcess::Id() const
{
    return "DustFree";
}

IsoString DustFreeProcess::Category() const
{
    return "BackgroundModelization";
}

// ----------------------------------------------------------------------------

uint32 DustFreeProcess::Version() const
{
    return 0x100;
}

// ----------------------------------------------------------------------------

String DustFreeProcess::Description() const
{
    return "";
}

// ----------------------------------------------------------------------------

IsoString DustFreeProcess::IconImageSVG() const
{
    return "<svg width=\"512\" height=\"512\" xmlns=\"http://www.w3.org/2000/svg\">"
           "<g id=\"Layer_1\">"
           "<title>Layer 1</title>"
           "<ellipse filter=\"url(#svg_4_blur)\" stroke-width=\"20\" ry=\"39.5\" rx=\"39.5\" id=\"svg_4\" cy=\"100.5\" cx=\"99.5\" stroke=\"#000\" fill=\"#fff\"/>"
           "<ellipse filter=\"url(#svg_5_blur)\" stroke-width=\"15\" ry=\"30\" rx=\"30\" id=\"svg_5\" cy=\"383\" cx=\"127\" stroke=\"#000\" fill=\"#fff\"/>"
           "<ellipse filter=\"url(#svg_6_blur)\" stroke-width=\"22\" ry=\"54.5\" rx=\"54.5\" id=\"svg_6\" cy=\"280.5\" cx=\"282.5\" stroke=\"#000\" fill=\"#fff\"/>"
           "<ellipse filter=\"url(#svg_7_blur)\" stroke-width=\"14\" ry=\"26\" rx=\"26\" id=\"svg_7\" cy=\"192\" cx=\"445\" stroke=\"#000\" fill=\"#fff\"/>"
           "<ellipse filter=\"url(#svg_9_blur)\" stroke-width=\"16\" ry=\"37.5\" rx=\"37.5\" id=\"svg_9\" cy=\"106.5\" cx=\"290.5\" stroke=\"#000\" fill=\"#fff\"/>"
           "<ellipse filter=\"url(#svg_10_blur)\" stroke-width=\"15\" ry=\"36\" rx=\"36\" id=\"svg_10\" cy=\"450\" cx=\"325\" stroke=\"#000\" fill=\"#fff\"/>"
           "</g>"
           "<defs>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_4_blur\">"
           "<feGaussianBlur stdDeviation=\"10\" in=\"SourceGraphic\"/>"
           "</filter>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_5_blur\">"
           "<feGaussianBlur stdDeviation=\"8\" in=\"SourceGraphic\"/>"
           "</filter>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_6_blur\">"
           "<feGaussianBlur stdDeviation=\"10\" in=\"SourceGraphic\"/>"
           "</filter>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_7_blur\">"
           "<feGaussianBlur stdDeviation=\"7\" in=\"SourceGraphic\"/>"
           "</filter>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_9_blur\">"
           "<feGaussianBlur stdDeviation=\"8\" in=\"SourceGraphic\"/>"
           "</filter>"
           "<filter height=\"200%\" width=\"200%\" y=\"-50%\" x=\"-50%\" id=\"svg_10_blur\">"
           "<feGaussianBlur stdDeviation=\"8\" in=\"SourceGraphic\"/>"
           "</filter>"
           "</defs>"
           "</svg>";
}
// ----------------------------------------------------------------------------

ProcessInterface* DustFreeProcess::DefaultInterface() const
{
    return TheDustFreeInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* DustFreeProcess::Create() const
{
    return new DustFreeInstance(this);
}

// ----------------------------------------------------------------------------

ProcessImplementation* DustFreeProcess::Clone(const ProcessImplementation& p) const
{
    const DustFreeInstance* instPtr = dynamic_cast<const DustFreeInstance*>(&p);
    return (instPtr != 0) ? new DustFreeInstance(*instPtr) : 0;
}

// ----------------------------------------------------------------------------

bool DustFreeProcess::NeedsValidation() const
{
    return false;
}

// ----------------------------------------------------------------------------

bool DustFreeProcess::CanProcessCommandLines() const
{
    return false;
}

}	// namespace pcl
