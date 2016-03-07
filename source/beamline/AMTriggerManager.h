#ifndef AMTRIGGERMANAGER_H
#define AMTRIGGERMANAGER_H

#include <QSignalMapper>

#include "beamline/AMControl.h"
#include "beamline/AMDetector.h"
#include "beamline/AMDetectorTriggerSource.h"

#include "dataman/info/AMDetectorInfo.h"

class AMTriggerManager : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMTriggerManager(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTriggerManager();

	/// Returns true if the manager can be armed right now.
	virtual bool canArm() const;
	/// Returns true if the manager can be triggered right now.
	virtual bool canTrigger() const;

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns the armed status.
	virtual bool isArmed() const { return armed_; }

	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* triggerSource() const { return triggerSource_; }

	/// Returns the list of detectors.
	QList<AMDetector*> detectors() const { return detectors_; }
	/// Returns the list of trigger managers.
	QList<AMTriggerManager*> triggerManagers() const { return triggerManagers_; }

	/// Creates and returns an action that adds a detector.
	virtual AMAction3* createAddDetectorAction(AMDetector *detector);
	/// Creates and returns an action that removes a detector.
	virtual AMAction3* createRemoveDetectorAction(AMDetector *detector);
	/// Creates and returns an action that removes all detectors.
	virtual AMAction3* createClearDetectorsAction();

	/// Creates and returns an action that adds a trigger manager.
	virtual AMAction3* createAddTriggerManagerAction(AMTriggerManager *manager);
	/// Creates and returns an action that removes a trigger manager.
	virtual AMAction3* createRemoveTriggerManagerAction(AMTriggerManager *manager);
	/// Creates and returns an action that removes all trigger managers.
	virtual AMAction3* createClearTriggerManagersAction();

	/// Creates and returns an action that arms the trigger manager.
	virtual AMAction3* createArmAction();
	/// Creates and returns an action that arms the trigger manager and triggers an acquisition.
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode);

signals:
	/// Notifier that the trigger manager has been armed.
	void armed();
	/// Notifier that the trigger manager has been triggered.
	void triggered();
	/// Notifier that the trigger source has changed.
	void triggerSourceChanged(AMDetectorTriggerSource *newSource);
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the trigger managers have changed.
	void triggerManagersChanged();

public slots:
	/// Sets the trigger source.
	virtual void setTriggerSource(AMDetectorTriggerSource *newSource);

	/// Adds a detector. Returns true if successful, false otherwise.
	virtual bool addDetector(AMDetector *newDetector);
	/// Removes a detector. Returns true if successful, false otherwise.
	virtual bool removeDetector(AMDetector *detector);
	/// Clears all detectors.
	virtual bool clearDetectors();

	/// Adds a trigger manager. Returns true if successful, false otherwise.
	virtual bool addTriggerManager(AMTriggerManager *newManager);
	/// Removes a trigger manager. Returns true if successful, false otherwise.
	virtual bool removeTriggerManager(AMTriggerManager *manager);
	/// Clears all trigger managers.
	virtual bool clearTriggerManagers();

	/// Arms all detectors and trigger managers.
	virtual void arm();
	/// Arms all detectors and trigger managers, and triggers an acquistion on all detectors and trigger managers using the given read mode.
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Sets the connected status.
	void setConnected(bool isConnected);
	/// Updates the connected status.
	void updateConnected();

	/// Sets the armed status.
	void setArmed(bool isArmed);
	/// Updates the armed status.
	void updateArmed();

	/// Handles updating the armed status when a detector reports as armed.
	void onDetectorArmed(QObject *detector);
	/// Handles updating the armed status when a trigger manager reports as armed.
	void onTriggerManagerArmed(QObject *manager);

	/// Sets the triggered status.
	void setTriggered(bool isTriggered);
	/// Updates the triggered status.
	void updateTriggered();

	/// Handles initiating the acquisition when the arming action reports as succeeded.
	void onArmActionSucceeded();

	/// Handles updating the triggered status when a detector reports as triggered.
	void onDetectorTriggered(QObject *detectorObject);
	/// Handles updating the triggered status when a trigger manager reports as triggered.
	void onTriggerManagerTriggered(QObject *managerObject);

protected:
	/// Returns true if all detectors are connected.
	bool detectorsConnected() const;
	/// Returns true if all trigger managers are connected.
	bool triggerManagersConnected() const;
	/// Returns true if the manager is connected. By default, the manager is connected if all detectors and trigger managers are connected.
	virtual bool managerConnected() const;

	/// Returns true if all detectors are armed.
	bool detectorsArmed() const;
	/// Returns true if all trigger managers are armed.
	bool triggerManagersArmed() const;
	/// Returns true if the manager is armed. By default, the manager is armed if all detectors and trigger managers are armed.
	virtual bool managerArmed() const;

	/// Returns true if all detectors have been triggered.
	bool detectorsTriggered() const;
	/// Returns true if all trigger managers have been triggered.
	bool triggerManagersTriggered() const;
	/// Returns true if the manager has been triggered. By default, the manager is triggered if all detectors and trigger managers have been triggered.
	virtual bool managerTriggered() const;

protected:
	/// The connected status.
	bool connected_;

	/// The trigger source.
	AMDetectorTriggerSource *triggerSource_;

	/// The list of detectors.
	QList<AMDetector*> detectors_;
	/// The list of trigger managers.
	QList<AMTriggerManager*> triggerManagers_;

	/// The armed status.
	bool armed_;

	/// The list of armed detectors.
	QList<AMDetector*> armedDetectors_;
	/// The list of armed trigger managers.
	QList<AMTriggerManager*> armedTriggerManagers_;

	/// The detectors arming signal mapper.
	QSignalMapper *detectorArmingMapper_;
	/// The trigger managers arming signal mapper.
	QSignalMapper *triggerManagerArmingMapper_;

	/// The triggered status.
	bool triggered_;
	/// The read mode.
	AMDetectorDefinitions::ReadMode readMode_;

	/// The list of triggered detectors.
	QList<AMDetector*> triggeredDetectors_;
	/// The list of triggered trigger managers.
	QList<AMTriggerManager*> triggeredTriggerManagers_;

	/// The detector triggering signal mapper.
	QSignalMapper *detectorTriggeringMapper_;
	/// The trigger manager triggering signal mapper.
	QSignalMapper *triggerManagerTriggeringMapper_;
};

#endif // AMTRIGGERMANAGER_H
