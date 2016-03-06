#ifndef BIOXASTRIGGERMANAGER_H
#define BIOXASTRIGGERMANAGER_H

#include <QSignalMapper>

#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"

#include "dataman/info/AMDetectorInfo.h"

class BioXASTriggerManager : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASTriggerManager(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASTriggerManager();

	/// Returns true if the manager can be triggered right now.
	virtual bool canTrigger() const;
	/// Returns true if the manager can be armed right now.
	virtual bool canArm() const;
	/// Returns true if the manager can acquire right now.
	virtual bool canAcquire() const;

	/// Returns true if the manager is armed.
	virtual bool isArmed() const { return armed_; }
	/// Returns true if the manager is acquiring.
	virtual bool isAcquiring() const { return acquiring_; }

	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* triggerSource() const { return triggerSource_; }
	/// Returns the read mode.
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }
	/// Returns the list of detectors.
	QList<AMDetector*> detectors() const { return detectors_; }

	/// Creates and returns an action that adds a detector.
	virtual AMAction3* createAddDetectorAction(AMDetector *detector) { return 0; }
	/// Creates and returns an action that removes a detector.
	virtual AMAction3* createRemoveDetectorAction(AMDetector *detector) { return 0; }
	/// Creates and returns an action that removes all detectors.
	virtual AMAction3* createClearDetectorsAction() { return 0; }

	/// Creates and returns an action that arms the trigger manager and triggers an acquisition.
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode) { return 0; }
	/// Creates and returns an action that arms the trigger manager.
	virtual AMAction3* createArmAction() { return 0; }
	/// Creates and returns an action that acquires on all detectors.
	virtual AMAction3* createAcquireAction() { return 0; }

signals:
	/// Notifier that the trigger source has changed.
	void triggerSourceChanged(AMDetectorTriggerSource *newSource);
	/// Notifier that the read mode has changed.
	void readModeChanged(AMDetectorDefinitions::ReadMode newMode);
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the trigger manager has been triggered.
	void triggered();
	/// Notifier that the armed status has changed.
	void armedChanged(bool isArmed);
	/// Notifier that the acquiring state has changed.
	void acquiringChanged(bool isAcquiring);

public slots:
	/// Sets the trigger source. This source initiates the triggering (arming + acquisition) sequence.
	void setTriggerSource(AMDetectorTriggerSource *newSource);

	/// Adds a detector. Returns true if successful, false otherwise.
	virtual bool addDetector(AMDetector *newDetector);
	/// Removes a detector. Returns true if successful, false otherwise.
	virtual bool removeDetector(AMDetector *detector);
	/// Clears all detectors.
	virtual bool clearDetectors();

	/// Triggers all detectors (arms + acquires).
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);
	/// Arms all detectors.
	virtual void arm();
	/// Acquires on all detectors.
	virtual void acquire(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Sets the read mode.
	void setReadMode(AMDetectorDefinitions::ReadMode newMode);

	/// Updates the connected state. Reimplemented to consider the managerConnected() state.
	virtual void updateConnected();

	/// Sets the armed status.
	void setArmed(bool isArmed);
	/// Updates the armed status.
	void updateArmed();

	/// Handles initiating the acquisition component of triggering when the arming action reports as succeeded.
	void onArmActionSucceeded();

	/// Sets the acquisition state.
	void setAcquiring(bool isAcquiring);
	/// Updates the acquisition state.
	void updateAcquiring();
	/// Handles updating the acquisition state and the flag for whether the manager is executing a triggered acquisition.
	void onDetectorAcquiringChanged();

protected:
	/// The acquisition implementation.
	virtual void acquisitionImplementation(AMDetectorDefinitions::ReadMode readMode);

	/// Returns true if all detectors are connected.
	bool detectorsConnected() const;
	/// Returns true if the manager is connected. By default, the manager is connected if all detectors are connected.
	virtual bool managerConnected() const;

	/// Returns true if all detectors are armed.
	bool detectorsArmed() const;
	/// Returns true if the manager is armed. By default, the manager is armed if all detectors are armed.
	virtual bool managerArmed() const;

	/// Returns true if there are detectors acquiring.
	bool detectorsAcquiring() const;
	/// Returns true if the manager is acquiring. By default, the manager is acquiring if any of the detectors are acquiring.
	virtual bool managerAcquiring() const;

protected:
	/// The trigger source.
	AMDetectorTriggerSource *triggerSource_;
	/// The read mode.
	AMDetectorDefinitions::ReadMode readMode_;
	/// The list of detectors.
	QList<AMDetector*> detectors_;

	/// The armed status.
	bool armed_;
	/// The acquiring status.
	bool acquiring_;
	/// Flag indicating whether the manager is executing a triggered acquisition.
	bool triggeredAcquisition_;
};

#endif // BIOXASTRIGGERMANAGER_H
