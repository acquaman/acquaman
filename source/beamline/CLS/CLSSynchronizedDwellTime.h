#ifndef CLSSYNCHRONIZEDDWELLTIME_H
#define CLSSYNCHRONIZEDDWELLTIME_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

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
	explicit CLSSynchronizedDwellTimeElement(QString baseName, int index, QObject *parent = 0);

	/// Returns the name.
	QString name() const { return name_->getString(); }
	/// Returns the units.
	QString units() const { return time_->units(); }
	/// Returns the scan status.  True means scanning, false means idle.
	bool status() const { return status_->getInt() == 1 ? true : false; }
	/// Returns the time in the units given by units().
	double time() const { return time_->getDouble(); }
	/// Returns whether the dwell element is enabled.
	bool isEnable() const { return enable_->getInt() == 1 ? true : false; }

public slots:
	/// Set the time (in seconds).  This will automatically be converted to match whatever the units of the element are.
	void setTime(double time) { time_->setValue(units() == "ms" ? time*1000 : time); }
	/// Enable/Disable the detector.
	void setEnabled(bool enable) { enable_->setValue(enable ? 1 : 0); }

signals:
	/// Notifier that the name changed.
	void nameChanged(QString);
	/// Notifier that the enabled state has changed.
	void enabledChanged(bool);
	/// Notifier that the time has changed.
	void timeChanged(double);
	/// Notifier that the status has changed.
	void statusChanged(bool);

protected slots:
	/// Transforms the int value for the enable status into a bool.
	void onEnabledChanged(int status) { emit enabledChanged(status == 1 ? true : false); }
	/// Transforms the int value for the scan status into a bool.
	void onStatusChanged(int status) { emit statusChanged(status == 1 ? true : false); }

protected:
	/// The process variable that holds the name.
	AMProcessVariable *name_;
	/// The process variable that holds the enabled status.
	AMProcessVariable *enable_;
	/// The process variable that holds the scan status.
	AMProcessVariable *status_;
	/// The process variable that holds the time.
	AMProcessVariable *time_;
};

/*!
  This class is meant to encapsulate the Synchronized dwell time implementation used at the CLS.  It is comprised of an overall status, a master dwell time, and a start scan command.
  The individual detectors have the following general properties:  a name, whether it is enabled/disabled, scan status, units, and a time.  The implementation of the element is hidden
  by the master dwell time controller class.

  It is important to know that this is currently not a complete abstraction.  It only offers the standard usage of this application.  If you want to configure the dwell time more specifically
  you need to go into the EDM screen and change it from there.
  */

class CLSSynchronizedDwellTime : public QObject
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
	double time() const { return dwellTime_->getDouble(); }
	/// Returns the time in an individual element.  May or may not be the same as time().  Value must be between 0 and elementCount()-1.
	double timeAt(int index) { return elements_.at(index)->time(); }

	/// Returns the number of elements in the dwell time list.
	int elementCount() const { return elements_.size(); }

	/// Returns the scan status.  True is scanning, false is idle.
	bool status() const
	{
		for (int i = 0; i < elements_.size(); i++)
			if (elements_.at(i)->status())
				return true;

		return false;
	}

	/// Returns the current mode.
	Mode mode() const { return mode_->getInt() == 0 ? Continuous : SingleShot; }
	/// Returns whether the dwell time is scanning.
	bool isScanning() const { return startScan_->getInt() == 1 ? true : false; }

	/// Adds an element based on the given \param index and the existing base name.  Assumes the the index given is valid because there is no way of knowing a priori how many have been configured.
	void addElement(int index);

signals:
	/// Notifier that the Mode has changed.
	void modeChanged(Mode);
	/// Notifier that the time has changed.
	void timeChanged(double);
	/// Notifier that the scan status has changed.
	void scanningChanged(bool);
	/// Notifier if the overall status has changed.
	void statusChanged(bool);

public slots:
	/*! Sets the time for the entire synchronized dwell time.  Needs to be in seconds.  \note Implementation detail: due to the way that the PVs are hooked up, this function does not have to write the new value
	 to each individual element.  The PVs are already hooked up to handle propogate the change.  */
	void setTime(double time) { dwellTime_->setValue(time); }
	/// Start scanning.
	void start() { startScan_->setValue(1); }
	/// Stop scanning.
	void stop() { startScan_->setValue(0); }
	/// Set the scan mode.
	void setMode(Mode mode) { mode_->setValue(mode == Continuous ? 0 : 1); }

protected slots:
	/// Handles changes in the startScan PV and turns the int into a bool.  True is scanning, false is idle.
	void onScanningChanged(int status) { emit scanningChanged(status == 1 ? true : false); }
	/// Handles changes in the status.
	void onStatusChanged() { emit statusChanged(status()); }
	/// Handles changes in Mode.  Turns the int into the Mode enum.
	void onModeChanged(int mode) { emit modeChanged(mode == 0 ? Continuous : SingleShot); }

protected:
	/// List holding the individual elements.
	QList<CLSSynchronizedDwellTimeElement *> elements_;
	/// Process variable holding the overall dwell time.
	AMProcessVariable *dwellTime_;
	/// Process variable holding the the scan trigger.
	AMProcessVariable *startScan_;
	/// Process variable holding the scan mode.
	AMProcessVariable *mode_;

	/// Holds the base name.  Used to build extra elements.
	QString baseName_;
};

#endif // CLSSYNCHRONIZEDDWELLTIME_H
