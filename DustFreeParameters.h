#ifndef __DustFreeParameters_h
#define __DustFreeParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

class DFStarDetectionSensitivity : public MetaFloat
{
public:
    DFStarDetectionSensitivity(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern DFStarDetectionSensitivity* TheDFStarDetectionSensitivityParameter;

class DFStarDiffusionDistance : public MetaInt8
{
public:
    DFStarDiffusionDistance(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern DFStarDiffusionDistance* TheDFStarDiffusionDistanceParameter;

class DFSmoothness : public MetaFloat
{
public:
    DFSmoothness(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern DFSmoothness* TheDFSmoothnessParameter;

class DFTestSkyDetection : public MetaBoolean
{
public:
    DFTestSkyDetection(MetaProcess*);

    IsoString Id() const override;
    bool DefaultValue() const override;
};

extern DFTestSkyDetection * TheDFTestSkyDetectionParameter;

class DFDownsample : public MetaUInt32
{
public:
    DFDownsample(MetaProcess*);

    IsoString Id() const override;
    double DefaultValue() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
};

extern DFDownsample * TheDFDownsampleParameter;

PCL_END_LOCAL

}	// namespace pcl

#endif	// __DustFreeParameters_h
