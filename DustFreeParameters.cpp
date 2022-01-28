#include "DustFreeParameters.h"

namespace pcl
{

DFStarDetectionSensitivity* TheDFStarDetectionSensitivityParameter = nullptr;
DFStarDiffusionDistance* TheDFStarDiffusionDistanceParameter = nullptr;
DFSmoothness* TheDFSmoothnessParameter = nullptr;
DFTestSkyDetection * TheDFTestSkyDetectionParameter = nullptr;
DFDownsample * TheDFDownsampleParameter = nullptr;

DFStarDetectionSensitivity::DFStarDetectionSensitivity(MetaProcess* P) : MetaFloat(P)
{
    TheDFStarDetectionSensitivityParameter = this;
}

IsoString DFStarDetectionSensitivity::Id() const
{
    return "starDetectionSensitivity";
}

int DFStarDetectionSensitivity::Precision() const
{
    return 2;
}

double DFStarDetectionSensitivity::MinimumValue() const
{
    return 0.0;
}

double DFStarDetectionSensitivity::MaximumValue() const
{
    return 6.0;
}

double DFStarDetectionSensitivity::DefaultValue() const
{
    return 4.0;
}

DFStarDiffusionDistance::DFStarDiffusionDistance(MetaProcess* P) : MetaInt8(P)
{
    TheDFStarDiffusionDistanceParameter = this;
}

IsoString DFStarDiffusionDistance::Id() const
{
    return "starDiffusionDistance";
}

double DFStarDiffusionDistance::MinimumValue() const
{
    return 0.0;
}

double DFStarDiffusionDistance::MaximumValue() const
{
    return 10.0;
}

double DFStarDiffusionDistance::DefaultValue() const
{
    return 5.0;
}

DFSmoothness::DFSmoothness(MetaProcess* P) : MetaFloat(P)
{
    TheDFSmoothnessParameter = this;
}

IsoString DFSmoothness::Id() const
{
    return "smoothness";
}

int DFSmoothness::Precision() const
{
    return 2;
}

double DFSmoothness::MinimumValue() const
{
    return 0.0;
}

double DFSmoothness::MaximumValue() const
{
    return 10.0;
}

double DFSmoothness::DefaultValue() const
{
    return 2.0;
}

DFTestSkyDetection::DFTestSkyDetection(MetaProcess * P) : MetaBoolean(P) 
{
    TheDFTestSkyDetectionParameter = this; 
}

IsoString DFTestSkyDetection::Id() const
{
    return "testSkyDetection";
}

bool DFTestSkyDetection::DefaultValue() const
{
    return false; 
}

DFDownsample::DFDownsample(MetaProcess * P) : MetaUInt32(P) 
{
    TheDFDownsampleParameter = this; 
}

IsoString DFDownsample::Id() const
{
    return "downsample"; 
}

double DFDownsample::DefaultValue() const
{
    return 2; 
}

double DFDownsample::MinimumValue() const
{
    return 1; 
}

double DFDownsample::MaximumValue() const
{
    return 16; 
}

}	// namespace pcl
