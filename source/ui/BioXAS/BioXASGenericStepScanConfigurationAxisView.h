#ifndef BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_H
#define BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_H

#include <QWidget>
#include <QComboBox>

#include "beamline/AMControlSet.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "ui/BioXAS/BioXASScanAxisRegionView.h"

#define BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MIN -10000
#define BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MAX 10000

class BioXASGenericStepScanConfigurationAxisView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASGenericStepScanConfigurationAxisView(AMGenericStepScanConfiguration *configuration, int axisNumber, BioXASScanAxisRegionView::ViewMode viewMode = BioXASScanAxisRegionView::Absolute, AMControlSet *controls = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASGenericStepScanConfigurationAxisView();

	/// Returns the view mode.
	BioXASScanAxisRegionView::ViewMode viewMode() const { return viewMode_; }
	/// Returns the configuration being viewed.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }
	/// Returns the axis number for the axis being viewed.
	int axisNumber() const { return axisNumber_; }
	/// Returns the set of controls to use as options.
	AMControlSet* controls() const { return controls_; }
	/// Returns the region relative value, used in relative view mode.
	AMNumber regionValue() const { return AMNumber(AMNumber::InvalidError); }

signals:
	/// Notifier that the view mode has changed.
	void viewModeChanged(BioXASScanAxisRegionView::ViewMode newMode);
	/// Notifier that the configuration being viewed has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the axis number for the axis being viewed has changed.
	void axisNumberChanged(int newNumber);
	/// Notifier that the set of controls to use as options has changed.
	void controlsChanged(AMControlSet *newControls);
	/// Notifier that the region relative value used in relative view mode has changed.
	void regionValueChanged(const AMNumber &newValue);

public slots:
	/// Sets the view mode.
	void setViewMode(BioXASScanAxisRegionView::ViewMode newMode);
	/// Sets the configuration being viewed.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the axis number for the axis being viewed.
	void setAxisNumber(int newNumber);
	/// Sets the set of controls to use as options.
	void setControls(AMControlSet *newControls);

protected slots:
	/// Updates the whole axis view.
	void updateAxisView();
	/// Updates the controls box.
	void updateControlsBox();
	/// Updates the region relative value box.
	void updateRegionValueBox();
	/// Updates the region view.
	void updateRegionView();

	/// Handles updating the configuration when the controls box selection changes.
	void onControlsBoxCurrentIndexChanged();

protected:
	/// Returns true if the axis in the given configuration at the given axis number is valid, false otherwise.
	virtual bool validAxis(AMGenericStepScanConfiguration *configuration, int number) const;
	/// Returns true if the axis region in the given configuration at the given axis number and region number is valid, false otherwise.
	virtual bool validAxisRegion(AMGenericStepScanConfiguration *configuration, int axisNumber, int regionNumber) const;
	/// Returns true if the axis control in the given configuration at the given axis number is valid, false otherwise.
	virtual bool validAxisControl(AMGenericStepScanConfiguration *configuration, int axisNumber) const;

protected:
	/// The view mode.
	BioXASScanAxisRegionView::ViewMode viewMode_;
	/// The configuration being viewed.
	AMGenericStepScanConfiguration *configuration_;
	/// The axis number for the axis being viewed.
	int axisNumber_;
	/// The set of controls to use as options.
	AMControlSet *controls_;

	/// The controls box.
	QComboBox *controlsBox_;
	/// The region relative value box.
	QDoubleSpinBox *regionValueBox_;
	/// The first region view.
	BioXASScanAxisRegionView *regionView_;
};

#endif // BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_H
