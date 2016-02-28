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

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const { return connected_; }

	/// Returns the read mode.
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }
	/// Returns the armed status.
	bool armed() const { return armed_; }

	/// Returns the trigger source.
	virtual AMDetectorTriggerSource* triggerSource() const { return triggerSource_; }

signals:
	/// Notifier that the read mode has changed.
	void readModeChanged(AMDetectorDefinitions::ReadMode newMode);
	/// Notifier that the trigger source has changed.
	void triggerSourceChanged(AMDetectorTriggerSource *newSource);
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the detector managers have changed.
	void detectorManagersChanged();
	/// Notifier that the detector manager has been armed.
	void armedChanged(bool isArmed);
	/// Notifier that the detector manager has been triggered.
	void triggered();

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
	/// Triggers an acquistion on all detectors and detector managers, using the given read mode.
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Sets the connected status.
	void setConnected(bool isConnected);
	/// Sets the read mode.
	void setReadMode(AMDetectorDefinitions::ReadMode newMode);
	/// Sets the armed status.
	void setArmed(bool isArmed);
	/// Sets the triggered status.
	void setTriggered(bool isTriggered);

	/// Updates the connected status.
	void updateConnected();

	/// Updates the armed status.
	void updateArmed();

	/// Handles updating the armed status when a detector reports as armed.
	void onDetectorArmed(QObject *detector);
	/// Handles updating the armed status when a detector manager reports as armed.
	void onDetectorManagerArmed(QObject *manager);

	/// Updates the triggered status.
	void updateTriggered();

	/// Handles updating the triggered status when a detector reports as triggered.
	void onDetectorTriggered(QObject *detectorObject);
	/// Handles updating the triggered status when a detector manager reports as triggered.
	void onDetectorManagerTriggered(QObject *managerObject);

protected:
	/// Returns true if all detectors are connected.
	bool detectorsConnected() const;
	/// Returns true if all detector managers are connected.
	bool detectorManagersConnected() const;

protected:
	/// The connected status.
	bool connected_;
	/// The read mode.
	AMDetectorDefinitions::ReadMode readMode_;
	/// The armed status.
	bool armed_;
	/// The triggered status.
	bool triggered_;

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
