#ifndef OCEANOPTICS65000DETECTOR_H
#define OCEANOPTICS65000DETECTOR_H

#include "AMDetector.h"
#include "dataman/OceanOptics65000DetectorInfo.h"
#include "AMControlSet.h"

class OceanOptics65000Detector : public OceanOptics65000DetectorInfo, public AMDetector
{
Q_OBJECT
public:
	/// Constructor with control sets
	OceanOptics65000Detector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	/// Constructor with individual controls
	OceanOptics65000Detector(const QString &name, AMControl *dataWaveform, AMControl *integrationTime, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	/// Destructor
	~OceanOptics65000Detector();

	/// Returns the meta object
	const QMetaObject* getMetaObject();

	/// General reading value (in this case, the total counts in all bins)
	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	/// Converts to a new instance of general detector info (snapshot of current state)
	AMDetectorInfo* toInfo() const;
	/// Converts to a by value copy of specific detector info type (snapshot of current state)
	OceanOptics65000DetectorInfo toOceanOptics65000Info() const;

	/// Sets current state from a pointer to specific detector info type
	bool setFromInfo(const AMDetectorInfo *info);
	/// Sets current state from a reference to specific detector info type
	bool setFromInfo(const OceanOptics65000DetectorInfo &info);

	/// Returns pointer to the data waveform control
	AMControl* dataWaveformControl() const;
	/// Returns pointer to the integration time contorl
	AMControl* integrationTimeControl() const;

	/// Checks the settings passed in to confirm they match the current values within tolerance
	bool settingsMatchFbk(OceanOptics65000DetectorInfo *settings);

	/// Returns the description
	QString description() const;

public slots:
	/// Sets the description for the detector
	void setDescription(const QString &description);
	/// Sets the detector contorls to the state passed in
	bool setControls(OceanOptics65000DetectorInfo *settings);

protected slots:
	/// Handles when controls change their connected state
	void onControlsConnected(bool connected);
	/// Handles when the values for the reading controls change
	void onReadingsControlValuesChanged();
	/// Handles when the values for the settings controls change
	void onSettingsControlValuesChanged();

protected:
	/// Main portion of the constructor, once we have the control sets we need
	bool initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls);

protected:
	/// Control set for the readings (just the data waveform in this case)
	AMControlSet *readingsControls_;
	/// Control set for the settings (just the integration time in this case)
	AMControlSet *settingsControls_;
	/// Toggle state for whether we're managing our own control sets
	bool ownsControlSets_;

};

#endif // OCEANOPTICS65000DETECTOR_H
