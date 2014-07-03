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


#ifndef AMSYNCHRONIZEDDWELLTIME_H
#define AMSYNCHRONIZEDDWELLTIME_H

#include <QObject>

class AMAction3;
class AMDetector;
class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;
class AMAction3;

class AMSynchronizedDwellTime : public QObject
{
	Q_OBJECT

public:
	AMSynchronizedDwellTime(QObject *parent = 0);
	virtual ~AMSynchronizedDwellTime();

	/// Return the overall dwell time.  Value is in seconds.
	virtual double time() const = 0;
	/// Returns the scan status.  True is scanning, false is idle.
	virtual bool status() const = 0;
	/// Returns whether the dwell time is scanning.
	virtual bool isScanning() const = 0;
	/// Returns whether all the controls are connected.
	virtual bool isConnected() const = 0;

	/// Convenience getter.  Returns the time in an individual element.  May or may not be the same as time().  \param index must be between 0 and elementCount()-1.
	virtual double timeAt(int index) const = 0;
	/// Convenience getter.  Returns the status of an individual element.  May or may not be the same as status().  \param index must be between 0 and elementCount()-1.
	virtual bool statusAt(int index) const = 0;
	/// Convenience getter.  Returns the enabled state of an individual element.  \param index must be between 0 and elementCount()-1.
	virtual bool enabledAt(int index) const = 0;

	/// Returns the key for a given element
	virtual QString keyAt(int index) const = 0;
	/// Returns all of the keys in element order
	virtual QStringList keys() const = 0;

	/// Returns the element index for a given detector based on the key. If the detector is not found -1 is returned.
	int indexOfDetector(const AMDetector *detector) const;

	/// Returns the number of elements in the dwell time list.
	virtual int elementCount() const = 0;

	/// Returns the trigger source for this synchronized dwell time
	virtual AMDetectorTriggerSource* triggerSource() = 0;
	/// Returns the dwell time source for this synchronized dwell time
	virtual AMDetectorDwellTimeSource* dwellTimeSource() = 0;

	/// Returns a newly created action that sets the master time for the synchronized dwell time to \param time.  Returns 0 if not connected.
	virtual AMAction3* createMasterTimeAction3(double time) = 0;
	/// Returns a newly created action that starts or stops the synchronized dwell time scan based on \param scan.  Returns 0 if not connected.
	virtual AMAction3* createScanningAction3(bool scan) = 0;
	/// Returns a newly created action that changes the enabled state of the synchronized dwell time.
	virtual AMAction3* createEnableAtAction3(int index, bool isEnabled) = 0;

signals:
	/// Notifier that the time has changed.
	void timeChanged(double);
	/// Notifier that the scan status has changed.
	void scanningChanged(bool);
	/// Notifier if the overall status has changed.
	void statusChanged(bool);
	/// Notifier that the synchronized dwell time is connected or not.
	void connected(bool);

public slots:
	/*! Sets the time for the entire synchronized dwell time.  Needs to be in seconds.  \note Implementation detail: due to the way that the PVs are hooked up, this function does not have to write the new value
	 to each individual element.  The PVs are already hooked up to propogate the change.  */
	virtual void setTime(double time) = 0;
	/// Sets the synchronized dwell time scanning based on the \param scan.  If true, it starts scanning, false stops scanning.
	virtual void startScanning(bool scan) = 0;
	/// Start scanning.
	virtual void start() = 0;
	/// Stop scanning.
	virtual void stop() = 0;
};

#endif // AMSYNCHRONIZEDDWELLTIME_H
