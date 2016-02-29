#ifndef SGMLINESCANCONFIGURATIONVIEW_H
#define SGMLINESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMLineScanConfiguration.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMDetectorSet.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QComboBox>

/// Holds the view for customizing continuous spatial line scans with the hexpod on SGM.
class SGMLineScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	SGMLineScanConfigurationView(SGMLineScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMLineScanConfigurationView();

	/// Getter for the configuration.
	virtual const AMScanConfiguration *configuration() const { return configuration_; }

	/// Returns the list of displayed controls.
	AMControlSet* controls() const { return controls_; }
	/// Returns the list of displayed detectors.
	AMDetectorSet* detectors() const { return detectors_; }

signals:

public slots:
	/// Sets the list of displayed controls.
	void setControls(AMControlSet *newControls);
	/// Sets the list of displayed detectors.
	void setDetectors(AMDetectorSet *newDetectors);

protected slots:
	/// Sets the axis start position.
	void setStart(const AMNumber &value);
	/// Sets the axis end position.
	void setEnd(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Helper slot that manages setting the axis start position.
	void onStartChanged();
	/// Helper slot that manages setting the axis end position.
	void onEndChanged();
	/// Helper slot that manages setting the time.
	void onDwellTimeChanged();

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged(double newTime);
	/// Handles setting the configurations axis 1 control info.
	void onAxisControlChoiceChanged();
	/// Handles updating the configurations detector infos.
	void onDetectorSelectionChanged(QAbstractButton *button);

protected:
	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateScanInformation();
	/// Creates a double spin box for the position spin boxes.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);

	/// Holds the pointer to the configuration we are visualizing.
	SGMLineScanConfiguration *configuration_;

	/// The combo box that holds the controls for the first axis.
	QComboBox *axisControlChoice_;
	/// The start position.
	QDoubleSpinBox *axisStart_;
	/// The end position for axis.
	QDoubleSpinBox *axisEnd_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Pointer to the label that holds the current map settings.
	QLabel *scanInformation_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// The controls being displayed.
	AMControlSet *controls_;
	/// The mapping between control and control name.
	QMap<QString, AMControl*> controlNameMap_;

	/// The detectors being displayed.
	AMDetectorSet *detectors_;
	/// The detectors button group.
	QButtonGroup *detectorGroup_;
	/// The detectors widget layout.
	QVBoxLayout *detectorLayout_;
	/// The mapping between detector and detector button.
	QMap<AMDetector*, QAbstractButton*> detectorButtonMap_;
};

#endif // SGMLINESCANCONFIGURATIONVIEW_H
