/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef CLSSYNCHRONIZEDDWELLTIME_H
#define CLSSYNCHRONIZEDDWELLTIME_H

#include <QObject>

#include "beamline/AMSynchronizedDwellTime.h"
#include "beamline/AMPVControl.h"
#include "dataman/info/AMDetectorInfo.h"
#include "dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h"
#include "beamline/CLS/CLSSynchronizedDwellTimeConfiguration.h"

class AMDetectorTriggerSource;
class AMAction3;

#define CLSSYNCHRONIZEDWELLTIME_INVALID_TRIGGER 289001

/*!
  This class encapulates the process variables used for the detectors as a synchronized dwell element.  It assumes a standardized naming convention for the elements and builds all the
  process variables for you.  It requires the base name (ex: BL1607-B2-1:dwell) and the index that the detector of interest occupies (0 being the first element by standard C/C++ array
  conventions).  It has some standard functions that give access to the process variables, but does not allow direct access.
  */
class CLSSynchronizedDwellTimeElement : public QObject{

	Q_OBJECT

public:
	/// Constructor.
	/*!
	  \param baseName is the base name used to build all the process varaibles.
	  \param index is the index in the list of the current synchronized dwell time.
	  */
	virtual ~CLSSynchronizedDwellTimeElement();
	explicit CLSSynchronizedDwellTimeElement(QString baseName, int index, QObject *parent = 0);

	/// Returns the name.
	QString name() const { return name_->getString(); }
	/// Returns the units.
	QString units() const { return time_->units(); }
	/// Returns the scan status.  True means scanning, false means idle.
	bool status() const { return status_->getInt() == 1; }
	/// Returns the time in the units given by units().
	double time() const { return time_->value(); }
	/// Returns whether the dwell element is enabled.
	bool isEnabled() const { return ((int)enable_->value()) == 1; }
	/// Returns whether all the controls are connected.
	bool isConnected() const { return name_->isConnected() && enable_->isConnected() && status_->isConnected() && time_->isConnected() && configuration_->isConnected(); }

	/// Returns the trigger string, which acts as the key for identifying elements
	virtual QString key() const;

	/// Configures the element based on the provided configuration info.
	void configure(const CLSSynchronizedDwellTimeConfigurationInfo &info);

	/// Returns a newly created action that sets the time to \param time.  Returns 0 if not connected.
	AMAction3 *createTimeAction3(double time);
	/// Returns a newly created action that enables/disables the dwell time element.  Returns 0 if not connected.
	AMAction3 *createEnableAction3(bool enable);

public slots:
	/// Set the time (in seconds).  This will automatically be converted to match whatever the units of the element are.
	void setTime(double time) { time_->move(units() == "ms" ? time*1000 : time); }
	/// Enable/Disable the detector.
	void setEnabled(bool enable) { enable_->move(enable ? 1.0 : 0.0); }

signals:
	/// Notifier that the name changed.
	void nameChanged(QString);
	/// Notifier that the enabled state has changed.
	void enabledChanged(bool);
	/// Notifier that the time has changed.
	void timeChanged(double);
	/// Notifier that the status has changed.
	void statusChanged(bool);
	/// Notifier that the element is fully connected or not.
	void connected(bool);
	/// Notifier that the trigger string has changed
	void triggerChanged(QString);

protected slots:
	/// Transforms the int value for the enable status into a bool.
	void onEnabledChanged(double status) { emit enabledChanged(((int)status) == 1); }
	/// Transforms the int value for the scan status into a bool.
	void onStatusChanged(int status) { emit statusChanged(status == 1); }
	/// Handles changes to the connectivity of the element.
	void onConnectedChanged() { emit connected(isConnected()); }

protected:
	/// The process variable that holds the name.
	AMProcessVariable *name_;
	/// The process variable that holds the enabled status.
	AMControl *enable_;
	/// The process variable that holds the scan status.
	AMProcessVariable *status_;
	/// The process variable that holds the time.
	AMControl *time_;
	/// The process variable that holds the trigger string (for key identification)
	AMProcessVariable *trigger_;
	/// The configuration of this element.
	CLSSynchronizedDwellTimeConfiguration *configuration_;
};

/*!
  This class is meant to encapsulate the Synchronized dwell time implementation used at the CLS.  It is comprised of an overall status, a master dwell time, and a start scan command.
  The individual detectors have the following general properties:  a name, whether it is enabled/disabled, scan status, units, and a time.  The implementation of the element is hidden
  by the master dwell time controller class.

  It is important to know that this is currently not a complete abstraction.  It only offers the standard usage of this application.  If you want to configure the dwell time more specifically
  you need to go into the EDM screen and change it from there.
  */

class CLSSynchronizedDwellTime : public AMSynchronizedDwellTime
{
	Q_OBJECT
public:
	/// Enum holding whether the dwell time should be in continuous or single shot mode.
	enum Mode { Continuous = 0, SingleShot };

	/// Constructor.
	/*!
	  \param baseName holds the base name used for building the process variables and the elements.
	  */
	explicit CLSSynchronizedDwellTime(QString baseName, QObject *parent = 0);

	/// Return the overall dwell time.  Value is in seconds.
	double time() const { return dwellTime_->value(); }
	/// Returns the scan status.  True is scanning, false is idle.
	bool status() const
	{
		for (int i = 0; i < elements_.size(); i++)
			if (elements_.at(i)->status())
				return true;

		return false;
	}
	/// Returns the current mode.
	Mode mode() const { return ((int)mode_->value()) == 0 ? Continuous : SingleShot; }
	/// Returns whether the dwell time is scanning.
	bool isScanning() const { return ((int)startScan_->value()) == 1; }
	/// Returns whether all the controls are connected.
	bool isConnected() const
	{
		bool connected = dwellTime_->isConnected() && startScan_->isConnected() && mode_->isConnected();

		for (int i = 0; i < elementCount(); i++)
			connected &= elements_.at(i)->isConnected();

		return connected;
	}

	/// Convenience getter.  Returns the time in an individual element.  May or may not be the same as time().  \param index must be between 0 and elementCount()-1.
	double timeAt(int index) const { return elements_.at(index)->time(); }
	/// Convenience getter.  Returns the status of an individual element.  May or may not be the same as status().  \param index must be between 0 and elementCount()-1.
	bool statusAt(int index) const { return elements_.at(index)->status(); }
	/// Convenience getter.  Returns the enabled state of an individual element.  \param index must be between 0 and elementCount()-1.
	bool enabledAt(int index) const { return elements_.at(index)->isEnabled(); }
	/// Convenience getter.  Returns the name of an individual element.  \param index must be between 0 and elementCount()-1.
	QString nameAt(int index) const { return elements_.at(index)->name(); }

	/// Returns the key for a given element
	virtual QString keyAt(int index) const;
	/// Returns all of the keys in element order
	virtual QStringList keys() const;

	/// Returns the number of elements in the dwell time list.
	int elementCount() const { return elements_.size(); }
	/// Adds an element based on the given \param index and the existing base name.  Assumes that the index given is valid because there is no way of knowing a priori how many have been configured.
	void addElement(int index);
	/// Returns the element at \param index.
	CLSSynchronizedDwellTimeElement *elementAt(int index) const { return elements_.at(index); }
	/// Returns the element based on the name provided.  Returns 0 if not found.
	CLSSynchronizedDwellTimeElement *elementByName(const QString &name) const;

	/// Returns the trigger source for the whole synchronized dwell time object
	virtual AMDetectorTriggerSource* triggerSource();
	/// Returns the dwell time source for the whole synchronzied dwell time object
	virtual AMDetectorDwellTimeSource* dwellTimeSource();

	/// Returns a newly created action that sets the master time for the synchronized dwell time to \param time.  Returns 0 if not connected.
	AMAction3* createMasterTimeAction3(double time);
	/// Returns a newly created action that starts or stops the synchronized dwell time scan based on \param scan.  Returns 0 if not connected.
	AMAction3* createScanningAction3(bool scan);
	/// Returns a newly created action that changes the mode of the synchronized dwell time based on \param mode.  Returns 0 if not connected.
	AMAction3* createModeAction3(CLSSynchronizedDwellTime::Mode mode);
	/// Returns a newly created action that changes the enabled state of the synchronized dwell time.
	AMAction3* createEnableAtAction3(int index, bool isEnabled);

	/// Returns the control for the master dwell time.
	AMControl *dwellTimeControl() const { return dwellTime_; }
	/// Returns the scan trigger control.
	AMControl *startScanControl() const { return startScan_; }
	/// Returns the scan mode control.
	AMControl *scanModeControl() const { return mode_; }

signals:
	/// Notifier that the Mode has changed.
	void modeChanged(CLSSynchronizedDwellTime::Mode);
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
	void setTime(double time) { dwellTime_->move(time); }
	/// Sets the synchronized dwell time scanning based on the \param scan.  If true, it starts scanning, false stops scanning.
	void startScanning(bool scan) { startScan_->move(scan == true ? 1.0 : 0.0); }
	/// Start scanning.
	void start() { startScan_->move(1.0); }
	/// Stop scanning.
	void stop() { startScan_->move(0.0); }
	/// Set the scan mode.
	void setMode(Mode mode) { mode_->move(mode == Continuous ? 0.0 : 1.0); }

protected slots:
	/// Handles changes in the startScan PV and turns the int into a bool.  True is scanning, false is idle.
	void onScanningChanged(double status);
	/// Handles changes in the time PV and passes along the signal as well as interacts with the dwell time source
	void onDwellTimeChanged(double dwellTime);
	/// Handles changes in the status.
	void onStatusChanged() { emit statusChanged(status()); }
	/// Handles changes in Mode.  Turns the int into the Mode enum.
	void onModeChanged(double mode) { emit modeChanged((int)mode == 0 ? Continuous : SingleShot); }
	/// Determines whether or not the synchronized dwell time is connected.
	void onConnectedChanged() { emit connected(isConnected()); }

	/// Handles forwarding the trigger source triggered() signal to starting the synchronized dwell time object
	void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	void triggerSynchronizedDwellTimeAcquisition(CLSSynchronizedDwellTime::Mode newMode);

	/// Handles forwarding the dwell time source setDwellTime() signal to the synchronized dwell time object
	void onDwellTimeSourceSetDwellTime(double dwellSeconds);

protected:
	/// List holding the individual elements.
	QList<CLSSynchronizedDwellTimeElement *> elements_;
	/// Process variable holding the overall dwell time.
	AMControl *dwellTime_;
	/// Process variable holding the the scan trigger.
	AMControl *startScan_;
	/// Process variable holding the scan mode.
	AMControl *mode_;

	/// Holds the base name.  Used to build extra elements.
	QString baseName_;

	/// The trigger source for the whole synchronized dwell time object
	AMDetectorTriggerSource *triggerSource_;
	/// The dwell time source for the whole synchronized dwell time object
	AMDetectorDwellTimeSource *dwellTimeSource_;
};

#endif // CLSSYNCHRONIZEDDWELLTIME_H
