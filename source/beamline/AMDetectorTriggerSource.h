/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMDETECTORTRIGGERSOURCE_H
#define AMDETECTORTRIGGERSOURCE_H

#include <QObject>
#include <QSignalMapper>

#include "dataman/info/AMDetectorInfo.h"

class AMDetectorTriggerSource : public QObject
{
Q_OBJECT
public:
	/// Constructor takes the programmer unique name for identification
	AMDetectorTriggerSource(const QString &name, QObject *parent = 0);
	virtual ~AMDetectorTriggerSource();

	/// Returns the programmer name
	QString name() const { return name_; }

public slots:
	/// Call this slot to trigger the source (cause detectors connected to it to acquire)
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

	void setSucceeded();
	void setFailed();

signals:
	/// This signal is emitted once trigger is called. Detectors and other classes that need to instantiate an AMDetectorTriggerSource should listen for this signal and cause triggering/acquisition once it is received.
	void triggered(AMDetectorDefinitions::ReadMode readMode);

	void succeeded();
	void failed();

protected:
	/// Holds the programmer name
	QString name_;
};

class AMDetector;
class AMControl;

class AMZebraDetectorTriggerSource : public AMDetectorTriggerSource
{
Q_OBJECT
public:
	/// Constructor
	AMZebraDetectorTriggerSource(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMZebraDetectorTriggerSource();

	/// Returns the list of detectors.
	QList<AMDetector*> detectors() const { return triggerSourceDetectors_; }
	/// Returns the list of detector managers.
	QList<QObject*> detectorManagers() const { return detectorManagers_; }

public slots:
	/// Call this slot to trigger the source (cause detectors connected to it to acquire). First, all detectors will be armed and we will wait for all of them to reply that they have armed.
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

	/// Adds a detector to this source so we can track which ones have been armed successfully
	bool addDetector(AMDetector *detector);
	/// Adds a detector manager.
	bool addDetectorManager(QObject *source);
	/// Removes a detector from this source.
	bool removeDetector(AMDetector *detector);
	/// Removes a detector manager from this source.
	bool removeDetectorManager(QObject *source);
	/// Removes all the detectors.
	bool removeAllDetectors();
	/// Removes all detector managers.
	bool removeAllDetectorManagers();

	/// Sets the specific control that acts as the trigger.
	void setTriggerControl(AMControl *triggerControl);
	/// Sets that the given detector has succeeded.  Only when all detectors have setSucceeded will the succeeded signal be emitted.  This uses a QObject pointer non-AMDetector's could be managing the trigger source.
	void setSucceeded(QObject *source);

protected slots:
	/// Handles detectors being successfully armed
	void onDetectorArmed(QObject *detector);

protected:
	/// The read mode.
	AMDetectorDefinitions::ReadMode readMode_;
	/// The list of detectors that need to trigger.
	QList<AMDetector*> triggerSourceDetectors_;
	/// The list of detector managers that can call setSucceeded.
	QList<QObject *> detectorManagers_;
	/// The list of detector managers that have yet to call setSucceeded.
	QList<QObject *> detectorManagersWaiting_;
	/// The list of armed detectors.
	QList<AMDetector*> armedDetectors_;
	/// The signal mapper to make sure that the detectors are all armed.
	QSignalMapper *detectorArmingMapper_;
	/// The control that triggers acquisition.
	AMControl *triggerControl_;
};

class AMDetectorDwellTimeSource : public QObject
{
Q_OBJECT
public:
 	virtual ~AMDetectorDwellTimeSource();
	AMDetectorDwellTimeSource(const QString &name, QObject *parent = 0);

	QString name() const { return name_; }

public slots:
	void requestSetDwellTime(double dwellSeconds);
    void requestSetDarkCurrentCorrectionTime(double timeSeconds);

	void setSucceeded();
	void setFailed();

signals:
	void setDwellTime(double dwellSeconds);
	void setDarkCurrentCorrectionTime(double timeSeconds);
	void darkCurrentTimeChanged(double timeSeconds);

	void succeeded();
	void failed();

protected:
	QString name_;
};

#endif // AMDETECTORTRIGGERSOURCE_H
