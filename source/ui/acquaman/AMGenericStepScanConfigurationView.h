#ifndef AMGENERICSTEPSCANCONFIGURATIONVIEW_H
#define AMGENERICSTEPSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

class QAbstractButton;

class AMGenericStepScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	AMGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	~AMGenericStepScanConfigurationView(){}

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

protected slots:
	/// Sets the axis 1 start position.
	void setStart1(const AMNumber &value);
	/// Sets the axis 1 step size.
	void setStep1(const AMNumber &value);
	/// Sets the axis 1 end position.
	void setEnd1(const AMNumber &value);
	/// Sets the axis 2 start position.
	void setStart2(const AMNumber &value);
	/// Sets the axis 2 step size.
	void setStep2(const AMNumber &value);
	/// Sets the axis 2 end position.
	void setEnd2(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Helper slot that manages setting the axis 1 start position.
	void onStart1Changed();
	/// Helper slot that manages setting the axis 1 step size.
	void onStep1Changed();
	/// Helper slot that manages setting the axis 1 end position.
	void onEnd1Changed();
	/// Helper slot that manages setting the axis 2 start position.
	void onStart2Changed();
	/// Helper slot that manages setting the axis 2 step size.
	void onStep2Changed();
	/// Helper slot that manages setting the axis 2 end position.
	void onEnd2Changed();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Handles setting the configurations axis 1 control info.
	void onAxisControlChoice1Changed();
	/// Handles setting the configurations axis 2 control info.
	void onAxisControlChoice2Changed();
	/// Handles updating the configurations detector infos.
	void onDetectorSelectionChanged(QAbstractButton *button);

	/// Handles doing some connections when an scan axis has been added or removed.
	void onScanAxisAdded(AMScanAxis *axis);

protected:
	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);
	/// Creates a double spin box for the position spin boxes.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);

	/// Pointer to the generic scan configuration.
	AMGenericStepScanConfiguration *configuration_;

	/// The combo box that holds the controls for the first axis.
	QComboBox *axisControlChoice1_;
	/// The combo box that holds the controls for the second axis.
	QComboBox *axisControlChoice2_;
	/// The start position for axis 1.
	QDoubleSpinBox *axisStart1_;
	/// The step position for axis 1.
	QDoubleSpinBox *axisStep1_;
	/// The end position for axis 1.
	QDoubleSpinBox *axisEnd1_;
	/// The start position for axis 2.
	QDoubleSpinBox *axisStart2_;
	/// The step position for axis 2.
	QDoubleSpinBox *axisStep2_;
	/// The end position for axis 2.
	QDoubleSpinBox *axisEnd2_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Pointer to the label that holds the current map settings.
	QLabel *scanInformation_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // AMGENERICSTEPSCANCONFIGURATIONVIEW_H
