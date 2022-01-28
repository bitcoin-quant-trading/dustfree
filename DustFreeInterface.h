#ifndef __DustFreeInterface_h
#define __DustFreeInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "DustFreeInstance.h"

namespace pcl {

class DustFreeInterface : public ProcessInterface
{
public:
    DustFreeInterface();
    virtual ~DustFreeInterface();

    IsoString Id() const override;
    MetaProcess* Process() const override;
    IsoString IconImageSVG() const override;
    InterfaceFeatures Features() const override;
    void ApplyInstance() const override;
    void ResetInstance() override;
    bool Launch(const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/) override;
    ProcessImplementation* NewProcess() const override;
    bool ValidateProcess(const ProcessImplementation&, pcl::String& whyNot) const override;
    bool RequiresInstanceValidation() const override;
    bool ImportProcess(const ProcessImplementation&) override;

private:
    DustFreeInstance instance;

    struct GUIData
    {
        GUIData(DustFreeInterface&);

        VerticalSizer   Global_Sizer;
            HorizontalSizer StarDetectionSensitivity_Sizer;
                NumericControl  StarDetectionSensitivity_NumericControl;
            HorizontalSizer StarDiffusionDistance_Sizer;
                NumericControl  StarDiffusionDistance_NumericControl;
            HorizontalSizer DustMaskView_Sizer;
                Label           DustMaskView_Label;
                Edit            DustMaskView_Edit;
                ToolButton      DustMaskView_ToolButton;
            HorizontalSizer Smoothness_Sizer;
                NumericControl  Smoothness_NumericControl;
            HorizontalSizer   Downsample_Sizer;
                Label           Downsample_Label;
                SpinBox         Downsample_SpinBox;
            HorizontalSizer TestSkyDetection_Sizer;
                CheckBox        TestSkyDetection_CheckBox;
    };

    GUIData* GUI = nullptr;

    void UpdateControls();
    void __GetFocus(Control& sender);
    void __EditCompleted(Edit& sender);
    void __EditValueUpdated(NumericEdit& sender, double value);
    void __SpinBoxValueUpdated(SpinBox& sender, int value);
    void __Click(Button& sender, bool checked);
    void __ViewDrag(Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView);
    void __ViewDrop(Control& sender, const Point& pos, const View& view, unsigned modifiers);

    friend struct GUIData;
};

PCL_BEGIN_LOCAL
extern DustFreeInterface* TheDustFreeInterface;
PCL_END_LOCAL

}	// namespace pcl

#endif  // __DustFreeInterface_h
