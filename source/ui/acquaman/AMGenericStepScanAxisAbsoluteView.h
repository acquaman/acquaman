#ifndef AMGENERICSTEPSCANAXISABSOLUTEVIEW_H
#define AMGENERICSTEPSCANAXISABSOLUTEVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/AMControlSet.h"
#include "ui/dataman/AMStepScanAxisView.h"

#define AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN -10000
#define AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX 10000

class AMGenericStepScanAxisAbsoluteView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMGenericStepScanAxisAbsoluteView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMGenericStepScanAxisAbsoluteView();

	/// Returns the configuration being viewed.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }
	/// Returns the list of displayed controls.
	AMControlSet* controls() const { return controls_; }

signals:
	/// Notifier that the configuration being viewed has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the list of displayed controls has changed.
	void controlsChanged(AMControlSet *newControls);

public slots:
	/// Sets the scan configuration being viewed.
	void setConfiguration(AMGenericStepScanConfiguration *configuration);
	/// Sets the list of displayed controls.
	void setControls(AMControlSet *newControls);

protected slots:
	/// Handles setting the configurations axis 1 control info.
	void onAxisControlChoice1Changed();

	/// Updates the axis 1 control combo box.
	void updateControl1Box();

	/// Helper slot that manages setting the axis 1 start position.
	void onStart1Changed();
	/// Helper slot that manages setting the axis 1 step size.
	void onStep1Changed();
	/// Helper slot that manages setting the axis 1 end position.
	void onEnd1Changed();

	/// Updates the axis 1 start position box.
	void updateStart1Box();
	/// Updates the axis 1 step size box.
	void updateStep1Box();
	/// Updates the axis 1 end position box.
	void updateEnd1Box();

	/// Handles setting the configurations axis 2 control info.
	void onAxisControlChoice2Changed();

	/// Updates the axis 2 control combo box.
	void updateControl2Box();

	/// Helper slot that manages setting the axis 2 start position.
	void onStart2Changed();
	/// Helper slot that manages setting the axis 2 step size.
	void onStep2Changed();
	/// Helper slot that manages setting the axis 2 end position.
	void onEnd2Changed();

	/// Updates the axis 2 start position box.
	void updateStart2Box();
	/// Updates the axis 2 step size box.
	void updateStep2Box();
	/// Updates the axis 2 end position box.
	void updateEnd2Box();

protected:
	/// The configuration being viewed.
	AMGenericStepScanConfiguration *configuration_;
	/// The list of displayed controls.
	AMControlSet* controls_;

	/// The combo box that holds the controls for the first axis.
	QComboBox *axisControlChoice1_;
	/// The start position for axis 1.
	QDoubleSpinBox *axisStart1_;
	/// The step position for axis 1.
	QDoubleSpinBox *axisStep1_;
	/// The end position for axis 1.
	QDoubleSpinBox *axisEnd1_;

	/// The combo box that holds the controls for the second axis.
	QComboBox *axisControlChoice2_;
	/// The start position for axis 2.
	QDoubleSpinBox *axisStart2_;
	/// The step position for axis 2.
	QDoubleSpinBox *axisStep2_;
	/// The end position for axis 2.
	QDoubleSpinBox *axisEnd2_;
};

#endif // AMGENERICSTEPSCANAXISABSOLUTEVIEW_H
