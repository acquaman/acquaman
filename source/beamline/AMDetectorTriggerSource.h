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

class AMArmedDetectorTriggerSource : public AMDetectorTriggerSource
{
Q_OBJECT
public:
	/// Constructor
	AMArmedDetectorTriggerSource(const QString &name, QObject *parent = 0);

	virtual ~AMArmedDetectorTriggerSource();

public slots:
	/// Call this slot to trigger the source (cause detectors connected to it to acquire). First, all detectors will be armed and we will wait for all of them to reply that they have armed.
	virtual void trigger(AMDetectorDefinitions::ReadMode readMode);

	/// Adds a detector to this source so we can track which ones have been armed successfully
	void addDetector(AMDetector *detector);

	void setTriggerControl(AMControl *triggerControl);

protected slots:
	/// Handles detectors being successfully armed
	void onDetectorArmed(QObject *detector);

protected:
	AMDetectorDefinitions::ReadMode readMode_;
	QList<AMDetector*> triggerSourceDetectors_;
	QList<AMDetector*> armedDetectors_;
	QSignalMapper *detectorArmingMapper_;

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
