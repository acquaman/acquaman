#ifndef AMDETECTORMANAGER_H
#define AMDETECTORMANAGER_H

#include <QSignalMapper>

#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorTriggerSource.h"

#include "dataman/info/AMDetectorInfo.h"

class AMDetectorManager : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMDetectorManager(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMDetectorManager();

	/// Returns true if the manager can be armed right now.
	virtual bool canArm() const { return true; }
	/// Returns true if the manager can be triggered right now.
	virtual bool canTrigger() const { return canArm(); }

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns the armed status.
	virtual bool isArmed() const { return armed_; }

	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* triggerSource() const { return triggerSource_; }

	/// Returns the list of detectors.
	QList<AMDetector*> detectors() const { return detectors_; }
	/// Returns the list of detector managers.
	QList<AMDetectorManager*> detectorManagers() const { return detectorManagers_; }

	/// Creates and returns an action that adds a detector.
	virtual AMAction3* createAddDetectorAction(AMDetector *detector);
	/// Creates and returns an action that removes a detector.
	virtual AMAction3* createRemoveDetectorAction(AMDetector *detector);
	/// Creates and returns an action that removes all detectors.
	virtual AMAction3* createClearDetectorsAction();

	/// Creates and returns an action that adds a detector manager.
	virtual AMAction3* createAddDetectorManagerAction(AMDetectorManager *manager);
	/// Creates and returns an action that removes a detector manager.
	virtual AMAction3* createRemoveDetectorManagerAction(AMDetectorManager *manager);
	/// Creates and returns an action that removes all detector managers.
	virtual AMAction3* createClearDetectorManagersAction();

	/// Creates and returns an action that adds a control.
	virtual AMAction3* createAddControlAction(AMControl *control, double triggerSetpoint) { Q_UNUSED(control) Q_UNUSED(triggerSetpoint) return 0; }
	/// Creates and returns an action that removes a control.
	virtual AMAction3* createRemoveControlAction(AMControl *control) { Q_UNUSED(control) return 0; }
	/// Creates and returns an action that removes all controls.
	virtual AMAction3* createClearControlsAction() { return 0; }

	/// Creates and returns an action that arms the detector manager.
	virtual AMAction3* createArmAction();
	/// Creates and returns an action that arms the detector manager and triggers an acquisition.
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode);

signals:
	/// Notifier that the detector manager has been armed.
	void armed();
	/// Notifier that the detector manager has been triggered.
	void triggered();
	/// Notifier that the trigger source has changed.
	void triggerSourceChanged(AMDetectorTriggerSource *newSource);
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the detector managers have changed.
	void detectorManagersChanged();

public slots:
	/// Sets the trigger source.
	virtual void setTriggerSource(AMDetectorTriggerSource *newSource);

	/// Adds a detector. Returns true if successful, false otherwise.
	virtual bool addDetector(AMDetector *newDetector);
	/// Removes a detector. Returns true if successful, false otherwise.
	virtual bool removeDetector(AMDetector *detector);
	/// Clears all detectors.
	virtual bool clearDetectors();

	/// Adds a detector manager. Returns true if successful, false otherwise.
	virtual bool addDetectorManager(AMDetectorManager *newManager);
	/// Removes a detector manager. Returns true if successful, false otherwise.
	virtual bool removeDetectorManager(AMDetectorManager *manager);
	/// Clears all detectors managers.
	virtual bool clearDetectorManagers();

	/// Arms all detectors and detector managers.
	virtual void arm();
	/// Arms all detectors and detector managers, and triggers an acquistion on all detectors and detector managers using the given read mode.
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Sets the connected status.
	void setConnected(bool isConnected);
	/// Sets the armed status.
	void setArmed(bool isArmed);

	/// Updates the connected status.
	void updateConnected();
	/// Updates the armed status.
	void updateArmed();

protected:
	/// Returns true if all detectors are connected.
	bool detectorsConnected() const;
	/// Returns true if all detector managers are connected.
	bool detectorManagersConnected() const;
	/// Returns true if the manager is connected. By default, the manager is connected if all detectors and detector managers are connected.
	virtual bool managerConnected() const;

	/// Returns true if all detectors are armed.
	bool detectorsArmed() const;
	/// Returns true if all detector managers are armed.
	bool detectorManagersArmed() const;
	/// Returns true if the manager is armed. By default, the manager is armed if all detectors and detector managers are armed.
	virtual bool managerArmed() const;

protected:
	/// The connected status.
	bool connected_;
	/// The armed status.
	bool armed_;

	/// The trigger source.
	AMDetectorTriggerSource *triggerSource_;

	/// The list of detectors that need to trigger.
	QList<AMDetector*> detectors_;
	/// The list of detector managers that can call setSucceeded.
	QList<AMDetectorManager*> detectorManagers_;
};

#endif // AMDETECTORMANAGER_H
