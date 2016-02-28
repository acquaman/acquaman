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
	virtual bool canTrigger() const { return true; }

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns the armed status.
	virtual bool isArmed() const { return armed_; }

	/// Returns the read mode.
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }
	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* triggerSource() const { return triggerSource_; }
	/// Returns the list of detectors.
	QList<AMDetector*> detectors() const { return detectors_; }
	/// Returns the list of detector managers.
	QList<AMDetectorManager*> detectorManagers() const { return detectorManagers_; }

	/// Returns true if all detectors are connected.
	bool detectorsConnected() const;
	/// Returns true if all detector managers are connected.
	bool detectorManagersConnected() const;

	/// Returns true if all detectors are armed.
	bool detectorsArmed() const { return detectorsArmed_; }
	/// Returns true if all detector managers are armed.
	bool detectorManagersArmed() const { return detectorManagersArmed_; }

	/// Returns true if all detectors have been triggered.
	bool detectorsTriggered() const { return detectorsTriggered_; }
	/// Returns true if all detector managers have been triggered.
	bool detectorManagersTriggered() const { return detectorManagersTriggered_; }

	/// Creates and returns an action that arms the detector manager.
	virtual AMAction3* createArmAction() { return 0; }
	/// Creates and returns an action that arms the detector manager and triggers an acquisition.
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode) { Q_UNUSED(readMode) return 0; }

signals:
	/// Notifier that the detector manager has been armed.
	void armedChanged(bool isArmed);
	/// Notifier that the detector manager has been triggered.
	void triggered();
	/// Notifier that the read mode has changed.
	void readModeChanged(AMDetectorDefinitions::ReadMode newMode);
	/// Notifier that the trigger source has changed.
	void triggerSourceChanged(AMDetectorTriggerSource *newSource);
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the detector managers have changed.
	void detectorManagersChanged();

public slots:
	/// Sets the trigger source.
	void setTriggerSource(AMDetectorTriggerSource *newSource);

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
	/// Sets the triggered status.
	void setTriggered(bool isTriggered);
	/// Sets the read mode.
	void setReadMode(AMDetectorDefinitions::ReadMode newMode);

	/// Updates the connected status.
	void updateConnected();
	/// Updates the armed status.
	void updateArmed();
	/// Updates the triggered status.
	void updateTriggered();

	/// Handles initiating the trigger chain when the trigger source reports as triggered.
	void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);

	/// Handles updating the armed status when a detector reports as armed.
	void onDetectorArmed(QObject *detector);
	/// Handles updating the armed status when a detector manager reports as armed.
	void onDetectorManagerArmed(QObject *manager);

	/// Handles updating the triggered status when a detector reports as triggered.
	void onDetectorTriggered(QObject *detectorObject);
	/// Handles updating the triggered status when a detector manager reports as triggered.
	void onDetectorManagerTriggered(QObject *managerObject);

protected:
	/// The connected status.
	bool connected_;
	/// The armed status.
	bool armed_;
	/// The triggered status.
	bool triggered_;
	/// The read mode.
	AMDetectorDefinitions::ReadMode readMode_;

	/// The trigger source.
	AMDetectorTriggerSource *triggerSource_;

	/// The list of detectors that need to trigger.
	QList<AMDetector*> detectors_;
	/// The list of detector managers that can call setSucceeded.
	QList<AMDetectorManager*> detectorManagers_;

	/// The detectors armed status.
	bool detectorsArmed_;
	/// The detector managers armed status.
	bool detectorManagersArmed_;

	/// The list of armed detectors.
	QList<AMDetector*> armedDetectors_;
	/// The list of armed detector managers.
	QList<AMDetectorManager*> armedDetectorManagers_;

	/// The signal mapper to make sure that the detectors are all armed.
	QSignalMapper *detectorArmingMapper_;
	/// The detector managers arming signal mapper.
	QSignalMapper *detectorManagerArmingMapper_;

	/// The detectors triggered status.
	bool detectorsTriggered_;
	/// The detector managers triggered status.
	bool detectorManagersTriggered_;

	/// The list of triggered detectors.
	QList<AMDetector*> triggeredDetectors_;
	/// The list of triggered detector managers.
	QList<AMDetectorManager*> triggeredDetectorManagers_;

	/// The detector triggering signal mapper.
	QSignalMapper *detectorTriggeringMapper_;
	/// The detector manager triggering signal mapper.
	QSignalMapper *detectorManagerTriggeringMapper_;
};

#endif // AMDETECTORMANAGER_H
