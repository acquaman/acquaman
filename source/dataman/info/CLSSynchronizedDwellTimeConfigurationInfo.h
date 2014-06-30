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


#ifndef CLSSYNCHRONIZEDDWELLTIMECONFIGURATIONINFO_H
#define CLSSYNCHRONIZEDDWELLTIMECONFIGURATIONINFO_H

#include <QObject>

/// The following is an encapsulation of all the information that is required to configure a synchronized dwell time element.
class CLSSynchronizedDwellTimeConfigurationInfo : public QObject
{
	Q_OBJECT

public:
	/// The trigger enum.
	enum Trigger { Normal = 0, Invert = 1 };
	/// The enum that holds what the dwell time element should wait for.
	enum WaitFor { Nothing = 0, Delay = 1, Value = 2 };

	/// Constructor.  Creates an empty info.
 	virtual ~CLSSynchronizedDwellTimeConfigurationInfo();
	CLSSynchronizedDwellTimeConfigurationInfo(QObject *parent = 0);

	/// Returns the name of the element.
	QString name() const { return name_; }
	/// Returns the dwell time PV name.
	QString dwellTimePV() const { return dwellTimePV_; }
	/// Returns the time scaling.
	QString scale() const { return scale_; }
	/// Returns the time offset.
	QString offset() const { return offset_; }
	/// Returns the time units.
	QString units() const { return units_; }
	/// Returns the mode PV name.
	QString modePV() const { return modePV_; }
	/// Returns the value for single shot mode.
	QString singleShot() const { return singleShot_; }
	/// Returns the value for continuous mode.
	QString continuous() const { return continuous_; }
	/// Returns the trigger PV name.
	QString triggerPV() const { return triggerPV_; }
	/// Returns whether the trigger is normal or inverted.
	Trigger trigger() const { return trigger_; }
	/// Returns the amount of time the pre-trigger needs.
	double preTrigger() const { return preTrigger_; }
	/// Returns the amount of time the dwell needs to be held up.
	double dwellHold() const { return dwellHold_; }
	/// Returns the status PV name.
	QString statusPV() const { return statusPV_; }
	/// Returns the wait for enum.
	WaitFor waitFor() const { return waitFor_; }
	/// Returns the amount of time the dwell time element is meant to wait for.
	double delay() const { return delay_; }
	/// Returns the wait PV name.
	QString waitPV() const { return waitPV_; }
	/// Returns the wait value.
	QString waitValue() const { return waitValue_; }

public slots:
	/// Sets the name of the element.
	void setName(const QString &value);
	/// Sets the dwell time PV name.
	void setDwellTimePV(const QString &value);
	/// Sets the time scaling.
	void setScale(const QString &value);
	/// Sets the time offset.
	void setOffset(const QString &value);
	/// Sets the time units.
	void setUnits(const QString &value);
	/// Sets the mode PV name.
	void setModePV(const QString &value);
	/// Sets the value for single shot mode.
	void setSingleShot(const QString &value);
	/// Sets the value for continuous mode.
	void setContinuous(const QString &value);
	/// Sets the trigger PV name.
	void setTriggerPV(const QString &value);
	/// Sets whether the trigger is normal or inverted.
	void setTrigger(Trigger value);
	/// Sets the amount of time the pre-trigger needs.
	void setPreTrigger(double value);
	/// Sets the amount of time the dwell needs to be held up.
	void setDwellHold(double value);
	/// Sets the status PV name.
	void setStatusPV(const QString &value);
	/// Sets the wait for enum.
	void setWaitFor(WaitFor value);
	/// Sets the amount of time the dwell time element is meant to wait for.
	void setDelay(double value);
	/// Sets the wait PV name.
	void setWaitPV(const QString &value);
	/// Sets the wait value.
	void setWaitValue(const QString &value);

protected:
	/// The name of the element.
	QString name_;
	/// The dwell time PV name.
	QString dwellTimePV_;
	/// The time scaling.
	QString scale_;
	/// The time offset.
	QString offset_;
	/// The time units.
	QString units_;
	/// The mode PV name.
	QString modePV_;
	/// Value for single shot mode.
	QString singleShot_;
	/// Value for continuous mode.
	QString continuous_;
	/// The trigger PV name.
	QString triggerPV_;
	/// Whether the trigger is normal or inverted.
	Trigger trigger_;
	/// The amount of time the pre-trigger needs.
	double preTrigger_;
	/// The amount of time the dwell needs to be held up.
	double dwellHold_;
	/// The status PV name.
	QString statusPV_;
	/// The wait for enum.
	WaitFor waitFor_;
	/// The amount of time the dwell time element is meant to wait for.
	double delay_;
	/// The wait PV name.
	QString waitPV_;
	/// The wait value.
	QString waitValue_;
};

#endif // CLSSYNCHRONIZEDDWELLTIMECONFIGURATIONINFO_H
