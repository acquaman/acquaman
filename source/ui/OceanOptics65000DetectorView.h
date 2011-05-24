#ifndef OCEANOPTICS65000DETECTORVIEW_H
#define OCEANOPTICS65000DETECTORVIEW_H

#include "AMDetectorView.h"
#include "beamline/OceanOptics65000Detector.h"

class OceanOptics65000BriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	/// General invokable constructor
	Q_INVOKABLE explicit OceanOptics65000BriefDetectorView(OceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns pointer to the detector object
	AMDetector* detector();

protected:
	/// Control editor to display the current reading
	AMControlEditor *readingCE_;
	QHBoxLayout *hl_;
	/// Pointer to the detector
	OceanOptics65000Detector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class OceanOptics65000DetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	/// General invokable constructor
	Q_INVOKABLE explicit OceanOptics65000DetailedDetectorView(OceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns pointer to the detector object
	AMDetector *detector();

	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;

protected slots:
	/// Handles when the control setpoint is requested
	void onControlSetpointRequested();

protected:
	/// Copies the settings from the detector
	void setConfigurationSettingsFromDetector();

protected:
	/// Pointer to the detector
	OceanOptics65000Detector *detector_;
	/// Pointer to the detector settings
	OceanOptics65000DetectorInfo *configurationSettings_;
	QGridLayout *gl_;
	/// Control editor to display the current reading
	AMControlEditor *readingCE_;
	/// Control editor to read/set the integration time
	AMControlEditor *integrationTimeCE_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // OCEANOPTICS65000DETECTORVIEW_H
