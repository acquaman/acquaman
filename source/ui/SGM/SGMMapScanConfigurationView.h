#ifndef SGMMAPSCANCONFIGURATIONVIEW_H
#define SGMMAPSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMMapScanConfiguration.h"
#include "beamline/AMDetectorSet.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QComboBox>

/// Holds the view for customizing continuous spatial map scans with the hexpod on SGM.
class SGMMapScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	SGMMapScanConfigurationView(SGMMapScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMMapScanConfigurationView();

	/// Getter for the configuration.
	virtual const AMScanConfiguration *configuration() const { return configuration_; }

	/// Returns the list of displayed detectors.
	AMDetectorSet* detectors() const { return detectors_; }

signals:

public slots:
	/// Sets the list of displayed detectors.
	void setDetectors(AMDetectorSet *newDetectors);

protected slots:
	/// Sets the axis 1 start position.
	void setStart1(const AMNumber &value);
	/// Sets the axis 1 end position.
	void setEnd1(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);
	/// Sets the axis 2 start position.
	void setStart2(const AMNumber &value);
	/// Sets the axis 2 step position.
	void setStep2(const AMNumber &value);
	/// Sets the axis 2 end position.
	void setEnd2(const AMNumber &value);

	/// Helper slot that manages setting the axis start position.
	void onStart1Changed();
	/// Helper slot that manages setting the axis end position.
	void onEnd1Changed();
	/// Helper slot that manages setting the time.
	void onDwellTimeChanged();
	/// Helper slot that manages setting the axis 2 start position.
	void onStart2Changed();
	/// Helper slot that manages setting the axis 2 step position.
	void onStep2Changed();
	/// Helper slot that manages setting the axis 2 end position.
	void onEnd2Changed();

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged(double newTime);
	/// Handles updating the configurations detector infos.
	void onDetectorSelectionChanged(QAbstractButton *button);

protected:
	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateScanInformation();
	/// Creates a double spin box for the position spin boxes.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);

	/// Holds the pointer to the configuration we are visualizing.
	SGMMapScanConfiguration *configuration_;

	/// The combo box that holds the controls for the first axis.
	QComboBox *axisControlChoice_;
	/// The start 1 position.
	QDoubleSpinBox *axisStart1_;
	/// The end 1 position for axis.
	QDoubleSpinBox *axisEnd1_;
	/// The start 2 position.
	QDoubleSpinBox *axisStart2_;
	/// The step 2 position.
	QDoubleSpinBox *axisStep2_;
	/// The end 2 position for axis.
	QDoubleSpinBox *axisEnd2_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Pointer to the label that holds the current map settings.
	QLabel *scanInformation_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// The detectors being displayed.
	AMDetectorSet *detectors_;
	/// The detectors button group.
	QButtonGroup *detectorGroup_;
	/// The detectors widget layout.
	QVBoxLayout *detectorLayout_;
	/// The mapping between detector and detector button.
	QMap<AMDetector*, QAbstractButton*> detectorButtonMap_;
};

#endif // SGMMAPSCANCONFIGURATIONVIEW_H
