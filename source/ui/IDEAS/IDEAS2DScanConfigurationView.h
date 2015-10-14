#ifndef IDEAS2DSCANCONFIGURATIONVIEW_H
#define IDEAS2DSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "acquaman/IDEAS/IDEAS2DScanConfiguration.h"

#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QButtonGroup>
#include <QCheckBox>

class IDEAS2DScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	IDEAS2DScanConfigurationView(IDEAS2DScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~IDEAS2DScanConfigurationView();

	/// Returns the scan configuration this view represents.
	virtual const AMScanConfiguration *configuration() const;

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();

protected slots:

	/// Sets the x-axis start position.
	void setXAxisStart(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisStart(const AMNumber &value);
	/// Sets the x-axis step position.
	void setXAxisStep(const AMNumber &value);
	/// Sets the y-axis step position.
	void setYAxisStep(const AMNumber &value);
	/// Sets the x-axis end position.
	void setXAxisEnd(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisEnd(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Handles setting the start position when the "Use Current" button is pushed.
	void onSetStartPosition();
	/// Handles setting the end position when the "Use Current" button is pushed.
	void onSetEndPosition();
	/// Helper slot that manages setting the x axis start position.
	void onXStartChanged();
	/// Helper slot that manages setting the x axis end position.
	void onXEndChanged();
	/// Helper slot that manages setting the x axis step size.
	void onXStepChanged();
	/// Helper slot that manages setting the y axis start position.
	void onYStartChanged();
	/// Helper slot that manages setting the y axis end position.
	void onYEndChanged();
	/// Helper slot that manages setting the y axis step size.
	void onYStepChanged();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();

	/// Handles setting scan energy when the spin box is changed
	void onScanEnergySpinBoxEditingFinished();
	/// Handles setting the scan energy when the button is clicked
	void onSetScanEnergyFromBeamlineButtonClicked();

	/// Handles changes from the beamline energy to check the warning label
	void onBeamlineEnergyChanged(double value);

	/// Handles changes to detector selection
	void onFluorescenceDetectorChanged(int detector);
	/// Slot that updates the fluorescence detector buttons.
	void updateFluorescenceDetectorComboBox(IDEAS::FluorescenceDetectors detector);
	/// Slot that updates the fluorescence detector comboBox items when the Ge13Elemet connects.
	void updateFluorescenceDetectorComboBoxGe13Element(bool connected);


	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Helper method that checks to see if the map dimensions are acceptable.  Makes visual changes if not valid.
	void checkScanAxisValidity();

protected:
	/// Add the scan name view.  Returns the widget.
	QLineEdit *createScanNameView(const QString &name);
	/// Build a position QDoubleSpinBox based on the prefix, suffix and value.  They have the same format, this should cut down on duplicate code.
	QDoubleSpinBox *createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals);
	/// Add the dwell time box.  Returns a pointer to the widget.
	QDoubleSpinBox *createDwellTimeSpinBox(double time);
	/// create spinbox
	QDoubleSpinBox *createEnergySpinBox(QString units, double minimumValue, double maximumValue, double defaultValue);
	/// helper function to create the comboBox for bruker detector
	QComboBox *createFluorescenceComboBox();
	/// helper function to create and layout detector settins
	QGroupBox *createAndLayoutDetectorSettings(IDEASScanConfiguration *configuration);

	/// Configuration this view represents.
	IDEAS2DScanConfiguration *configuration_;

	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;
	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Pointer to the horizontal start point.
	QDoubleSpinBox *hStart_;
	/// Pointer to the horizontal end point.
	QDoubleSpinBox *hEnd_;
	/// Pointer to the vertical start point.
	QDoubleSpinBox *vStart_;
	/// Pointer to the vertical end point.
	QDoubleSpinBox *vEnd_;
	/// Pointer to the horizontal step size.
	QDoubleSpinBox *hStep_;
	/// Pointer to the vertical step size.
	QDoubleSpinBox *vStep_;

	/// ComboBox for choosing Bruker detector
	QComboBox *fluorescenceDetectorComboBox_;
	/// Double spin box to set the scan energy for 2D scan
	QDoubleSpinBox *scanEnergySpinBox_;
	/// Label used to warn the user when the current beamline settings don't match the request settings in the configuration
	QLabel *scanEnergySettingWarningLabel_;
	/// Button to quickly set the scan energy from the current beamline settings
	QPushButton *setScanEnergyFromBeamlineButton_;

	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// This is an error display label.
	QLabel *errorLabel_;
	/// Holds the beamline settings (ie,, excitation energy)
	QGroupBox *beamlineSettingsGroupBox_;
};

#endif // IDEAS2DSCANCONFIGURATIONVIEW_H
