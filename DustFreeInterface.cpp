#include "DustFreeInterface.h"
#include "DustFreeParameters.h"
#include "DustFreeProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

DustFreeInterface* TheDustFreeInterface = nullptr;

DustFreeInterface::DustFreeInterface()
	: instance(TheDustFreeProcess)
{
	TheDustFreeInterface = this;
}

DustFreeInterface::~DustFreeInterface()
{
	if (GUI != nullptr)
		delete GUI, GUI = nullptr;
}

IsoString DustFreeInterface::Id() const
{
	return "DustFree";
}

MetaProcess* DustFreeInterface::Process() const
{
	return TheDustFreeProcess;
}

IsoString DustFreeInterface::IconImageSVG() const
{
	return TheDustFreeProcess->IconImageSVG();
}

InterfaceFeatures DustFreeInterface::Features() const
{
	return InterfaceFeature::Default;
}

void DustFreeInterface::ApplyInstance() const
{
	instance.LaunchOnCurrentView();
}

void DustFreeInterface::ResetInstance()
{
	DustFreeInstance defaultInstance(TheDustFreeProcess);
	ImportProcess(defaultInstance);
}

bool DustFreeInterface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)
{
	if (GUI == nullptr)
	{
		GUI = new GUIData(*this);
		SetWindowTitle("DustFree");
		UpdateControls();
	}

	dynamic = false;
	return &P == TheDustFreeProcess;
}

ProcessImplementation* DustFreeInterface::NewProcess() const
{
	return new DustFreeInstance(instance);
}

bool DustFreeInterface::ValidateProcess(const ProcessImplementation& p, String& whyNot) const
{
	if (dynamic_cast<const DustFreeInstance*>(&p) != nullptr)
		return true;
	whyNot = "Not a DustFree instance.";
	return false;
}

bool DustFreeInterface::RequiresInstanceValidation() const
{
	return true;
}

bool DustFreeInterface::ImportProcess(const ProcessImplementation& p)
{
	instance.Assign(p);
	UpdateControls();
	return true;
}

#define NO_MASK			String( "<No mask>" )
#define MASK_ID(x)		(x.IsEmpty() ? NO_MASK : x)
#define DUST_MASK_ID	MASK_ID(instance.dustMaskViewId)

void DustFreeInterface::UpdateControls()
{
	GUI->StarDetectionSensitivity_NumericControl.SetValue(instance.starDetectionSensitivity);
	GUI->StarDiffusionDistance_NumericControl.SetValue(instance.starDiffusionDistance);
	GUI->DustMaskView_Edit.SetText(DUST_MASK_ID);
	GUI->Smoothness_NumericControl.SetValue(instance.smoothness);
	GUI->Downsample_SpinBox.SetValue(instance.downsample);
	GUI->TestSkyDetection_CheckBox.SetChecked(instance.testSkyDetection);
}

void DustFreeInterface::__GetFocus(Control& sender)
{
	Edit* e = dynamic_cast<Edit*>(&sender);
	if (e != nullptr)
		if (e->Text() == NO_MASK)
			e->Clear();
}

void DustFreeInterface::__EditCompleted(Edit& sender)
{
	if (sender == GUI->DustMaskView_Edit)
	{
		try
		{
			String id = sender.Text().Trimmed();
			if (id == NO_MASK)
				id.Clear();
			if (!id.IsEmpty())
				if (!View::IsValidViewId(id))
					throw Error("Invalid view identifier: " + id);
			instance.dustMaskViewId = id;
			sender.SetText(DUST_MASK_ID);
		}
		catch (...)
		{
			sender.SetText(DUST_MASK_ID);
			try
			{
				throw;
			}
			ERROR_HANDLER
				sender.SelectAll();
			sender.Focus();
		}
	}
}

void DustFreeInterface::__EditValueUpdated(NumericEdit& sender, double value)
{
	if (sender == GUI->StarDetectionSensitivity_NumericControl)
		instance.starDetectionSensitivity = value;
	else if (sender == GUI->StarDiffusionDistance_NumericControl)
		instance.starDiffusionDistance = value;
	else if (sender == GUI->Smoothness_NumericControl)
		instance.smoothness = value;
}

void DustFreeInterface::__SpinBoxValueUpdated(SpinBox & sender, int value)
{
	if (sender == GUI->Downsample_SpinBox)
		instance.downsample = value;
}

void DustFreeInterface::__Click(Button& sender, bool checked)
{
	if (sender == GUI->DustMaskView_ToolButton) {
		ViewSelectionDialog d(instance.dustMaskViewId);
		if (d.Execute() == StdDialogCode::Ok)
		{
			instance.dustMaskViewId = d.Id();
			GUI->DustMaskView_Edit.SetText(DUST_MASK_ID);
		}
	} else if (sender == GUI->TestSkyDetection_CheckBox) {
		instance.testSkyDetection = checked;
	}
}

void DustFreeInterface::__ViewDrag(Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView)
{
	if (sender == GUI->DustMaskView_Edit)
		wantsView = true;
}

// ----------------------------------------------------------------------------

void DustFreeInterface::__ViewDrop(Control& sender, const Point& pos, const View& view, unsigned modifiers)
{
	if (sender == GUI->DustMaskView_Edit) {
		instance.dustMaskViewId = view.FullId();
		GUI->DustMaskView_Edit.SetText(DUST_MASK_ID);
	}
}

DustFreeInterface::GUIData::GUIData(DustFreeInterface& w)
{
	pcl::Font fnt = w.Font();
	int labelWidth1 = fnt.Width(String("Star detection sensitivity:") + 'T');
	int editWidth1 = fnt.Width(String('0', 12));
	int ui4 = w.LogicalPixelsToPhysical(4);

	StarDetectionSensitivity_NumericControl.label.SetText("Star detection sensitivity:");
	StarDetectionSensitivity_NumericControl.label.SetFixedWidth(labelWidth1);
	StarDetectionSensitivity_NumericControl.slider.SetRange(0, 600);
	StarDetectionSensitivity_NumericControl.slider.SetScaledMinWidth(300);
	StarDetectionSensitivity_NumericControl.SetReal();
	StarDetectionSensitivity_NumericControl.SetRange(TheDFStarDetectionSensitivityParameter->MinimumValue(), TheDFStarDetectionSensitivityParameter->MaximumValue());
	StarDetectionSensitivity_NumericControl.SetPrecision(TheDFStarDetectionSensitivityParameter->Precision());
	StarDetectionSensitivity_NumericControl.edit.SetFixedWidth(editWidth1);
	StarDetectionSensitivity_NumericControl.SetToolTip("<p>This value describes the star detection sensitivity. "
		"Increasing sensitivity allows identifying fainter stars, at the risk of identifying noise as stars.</p>");
	StarDetectionSensitivity_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & DustFreeInterface::__EditValueUpdated, w);
	StarDetectionSensitivity_Sizer.SetSpacing(4);
	StarDetectionSensitivity_Sizer.Add(StarDetectionSensitivity_NumericControl);
	StarDetectionSensitivity_Sizer.AddStretch();

	StarDiffusionDistance_NumericControl.label.SetText("Star diffusion distance:");
	StarDiffusionDistance_NumericControl.label.SetFixedWidth(labelWidth1);
	StarDiffusionDistance_NumericControl.slider.SetRange(0, 1000);
	StarDiffusionDistance_NumericControl.slider.SetScaledMinWidth(300);
	StarDiffusionDistance_NumericControl.SetInteger();
	StarDiffusionDistance_NumericControl.SetRange(TheDFStarDiffusionDistanceParameter->MinimumValue(), TheDFStarDiffusionDistanceParameter->MaximumValue());
	StarDiffusionDistance_NumericControl.edit.SetFixedWidth(editWidth1);
	StarDiffusionDistance_NumericControl.SetToolTip("<p>After star detection is done, the star mask will be diffused in order to protect the edges "
		                                              "and flares of stars. This value describes the distance of the diffusion.</p>");
	StarDiffusionDistance_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & DustFreeInterface::__EditValueUpdated, w);
	StarDiffusionDistance_Sizer.SetSpacing(4);
	StarDiffusionDistance_Sizer.Add(StarDiffusionDistance_NumericControl);
	StarDiffusionDistance_Sizer.AddStretch();

	DustMaskView_Label.SetText("Dust mask image:");
	DustMaskView_Label.SetFixedWidth(labelWidth1);
	DustMaskView_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	DustMaskView_Edit.SetToolTip("<p>123</p>");
	DustMaskView_Edit.OnGetFocus((Control::event_handler) & DustFreeInterface::__GetFocus, w);
	DustMaskView_Edit.OnEditCompleted((Edit::edit_event_handler) & DustFreeInterface::__EditCompleted, w);
	DustMaskView_Edit.OnViewDrag((Control::view_drag_event_handler) & DustFreeInterface::__ViewDrag, w);
	DustMaskView_Edit.OnViewDrop((Control::view_drop_event_handler) & DustFreeInterface::__ViewDrop, w);
	DustMaskView_ToolButton.SetIcon(Bitmap(w.ScaledResource(":/icons/select-view.png")));
	DustMaskView_ToolButton.SetScaledFixedSize(20, 20);
	DustMaskView_ToolButton.SetToolTip("<p>Binary mask of the dusts that need to be removed.</p>");
	DustMaskView_ToolButton.OnClick((Button::click_event_handler) & DustFreeInterface::__Click, w);
	DustMaskView_Sizer.SetSpacing(4);
	DustMaskView_Sizer.Add(DustMaskView_Label);
	DustMaskView_Sizer.Add(DustMaskView_Edit);
	DustMaskView_Sizer.Add(DustMaskView_ToolButton);

	Smoothness_NumericControl.label.SetText("Smoothness:");
	Smoothness_NumericControl.label.SetFixedWidth(labelWidth1);
	Smoothness_NumericControl.slider.SetRange(0, 1000);
	Smoothness_NumericControl.slider.SetScaledMinWidth(300);
	Smoothness_NumericControl.SetReal();
	Smoothness_NumericControl.SetRange(TheDFSmoothnessParameter->MinimumValue(), TheDFSmoothnessParameter->MaximumValue());
	Smoothness_NumericControl.SetPrecision(TheDFSmoothnessParameter->Precision());
	Smoothness_NumericControl.edit.SetFixedWidth(editWidth1);
	Smoothness_NumericControl.SetToolTip("<p>Smoothness of inpainting after dust removal.</p>");
	Smoothness_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & DustFreeInterface::__EditValueUpdated, w);
	Smoothness_Sizer.SetSpacing(4);
	Smoothness_Sizer.Add(Smoothness_NumericControl);
	Smoothness_Sizer.AddStretch();

	Downsample_Label.SetText("Downsample");
	Downsample_Label.SetFixedWidth(labelWidth1);
	Downsample_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	Downsample_SpinBox.SetRange(TheDFDownsampleParameter->MinimumValue(), int(TheDFDownsampleParameter->MaximumValue()));
	Downsample_SpinBox.OnValueUpdated((SpinBox::value_event_handler) & DustFreeInterface::__SpinBoxValueUpdated, w);
	Downsample_Sizer.SetSpacing(4);
	Downsample_Sizer.Add(Downsample_Label);
	Downsample_Sizer.Add(Downsample_SpinBox);
	Downsample_Sizer.AddStretch();

	TestSkyDetection_CheckBox.SetText("Test sky detection");
	TestSkyDetection_CheckBox.SetToolTip("<p>If selected, only sky detection will be shown as the result. Inpainting will be skipped.</p>");
	TestSkyDetection_CheckBox.OnClick((Button::click_event_handler) & DustFreeInterface::__Click, w);
	TestSkyDetection_Sizer.AddUnscaledSpacing(labelWidth1 + ui4);
	TestSkyDetection_Sizer.Add(TestSkyDetection_CheckBox);
	TestSkyDetection_Sizer.AddStretch();

	Global_Sizer.SetMargin(8);
	Global_Sizer.SetSpacing(4);
	Global_Sizer.Add(StarDetectionSensitivity_Sizer);
	Global_Sizer.Add(StarDiffusionDistance_Sizer);
	Global_Sizer.Add(DustMaskView_Sizer);
	Global_Sizer.Add(Smoothness_Sizer);
	Global_Sizer.Add(Downsample_Sizer);
	Global_Sizer.Add(TestSkyDetection_Sizer);

	w.SetSizer(Global_Sizer);

	w.EnsureLayoutUpdated();
	w.AdjustToContents();
	w.SetFixedSize();
}

}	// namespace pcl
