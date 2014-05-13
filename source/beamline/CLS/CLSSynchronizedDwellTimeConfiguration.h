#ifndef CLSSYNCHRONIZEDDWELLTIMECONFIGURATION_H
#define CLSSYNCHRONIZEDDWELLTIMECONFIGURATION_H

#include <QObject>

#include "dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h"
#include "beamline/AMProcessVariable.h"

/*!
	This class encapsulates all of the PV's necessary to configure a CLS synchronized dwell time element.
	The configuration requires knowledge of which element it is associated with because that determines
	all of the PV names.  For simplicity, the element association is not configurable.  There are two ways
	that this class can be used.  If wanting to set individual pieces of the configuration, you can use
	the getters and setters.  If you have saved CLSSynchronizedDwellTimeConfigurationInfo's, you can just
	use the configure() method by passing in the info and it will handle the rest.

	An important thing to note is that this class uses AMProcessVariables instead of AMControl due to the need
	of setting string based PV's.
*/
class CLSSynchronizedDwellTimeConfiguration : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Requires the base name and element number.
 	virtual ~CLSSynchronizedDwellTimeConfiguration();
	CLSSynchronizedDwellTimeConfiguration(const QString &name, int number, QObject *parent = 0);

	/// Returns whether the configuration is connected.
	bool isConnected() const;

	/// Returns the name of the element.
	QString name() const { return name_->getString(); }
	/// Returns the dwell time PV name.
	QString dwellTimePV() const { return dwellTimePV_->getString(); }
	/// Returns the time scaling.
	QString scale() const { return scale_->getString(); }
	/// Returns the time offset.
	QString offset() const { return offset_->getString(); }
	/// Returns the time units.
	QString units() const { return units_->getString(); }
	/// Returns the mode PV name.
	QString modePV() const { return modePV_->getString(); }
	/// Returns the value for single shot mode.
	QString singleShot() const { return singleShot_->getString(); }
	/// Returns the value for continuous mode.
	QString continuous() const { return continuous_->getString(); }
	/// Returns the trigger PV name.
	QString triggerPV() const { return triggerPV_->getString(); }
	/// Returns whether the trigger is normal or inverted.
	CLSSynchronizedDwellTimeConfigurationInfo::Trigger trigger() const { return CLSSynchronizedDwellTimeConfigurationInfo::Trigger(trigger_->getInt()); }
	/// Returns the amount of time the pre-trigger needs.
	double preTrigger() const { return preTrigger_->getDouble(); }
	/// Returns the amount of time the dwell needs to be held up.
	double dwellHold() const { return dwellHold_->getDouble(); }
	/// Returns the status PV name.
	QString statusPV() const { return statusPV_->getString(); }
	/// Returns the wait for enum.
	CLSSynchronizedDwellTimeConfigurationInfo::WaitFor waitFor() const { return CLSSynchronizedDwellTimeConfigurationInfo::WaitFor(waitFor_->getInt()); }
	/// Returns the amount of time the dwell time element is meant to wait for.
	double delay() const { return delay_->getDouble(); }
	/// Returns the wait PV name.
	QString waitPV() const { return waitPV_->getString(); }
	/// Returns the wait value.
	QString waitValue() const { return waitValue_->getString(); }

signals:
	/// Notifier that the configuration is fully connected.
	void connected(bool);
	/// Notifier that the name has changed.
	void nameChanged();
	/// Notifier that the dwell time PV name changed.
	void dwellTimePVChanged();
	/// Notifier that the scale has changed.
	void scaleChanged();
	/// Notifier that the offset has changed.
	void offsetChanged();
	/// Notifier that the units have changed.
	void unitsChanged();
	/// Notifier that the mode PV name has changed.
	void modePVChanged();
	/// Notifier that the single shot value has changed.
	void singleShotChanged();
	/// Notifier that the continuous value has changed.
	void continuousChanged();
	/// Notifier that the trigger PV name has changed.
	void triggerPVChanged();
	/// Notifier that the trigger has changed.
	void triggerChanged();
	/// Notifier that the pre-trigger has changed.
	void preTriggerChanged();
	/// Notifier that the dwell hold value has changed.
	void dwellHoldChanged();
	/// Notifier that the status PV name changed.
	void statusPVChanged();
	/// Notifier that the wait for enum value has changed.
	void waitForChanged();
	/// Notifier that the delay value has changed.
	void delayChanged();
	/// Notifier that the wait PV name has changed.
	void waitPVChanged();
	/// Notifier that the wait value has changed.
	void waitValueChanged();

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
	void setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Trigger value);
	/// Sets the amount of time the pre-trigger needs.
	void setPreTrigger(double value);
	/// Sets the amount of time the dwell needs to be held up.
	void setDwellHold(double value);
	/// Sets the status PV name.
	void setStatusPV(const QString &value);
	/// Sets the wait for enum.
	void setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::WaitFor value);
	/// Sets the amount of time the dwell time element is meant to wait for.
	void setDelay(double value);
	/// Sets the wait PV name.
	void setWaitPV(const QString &value);
	/// Sets the wait value.
	void setWaitValue(const QString &value);

	/// Convenience method that sets every aspect of a scan configuration based on the passed in CLSSynchronizedDwellTimeConfigurationInfo.
	void configure(const CLSSynchronizedDwellTimeConfigurationInfo &info);

protected slots:
	/// Handles the connectivity of the configuration.
	void onConnectedChanged();

protected:
	/// Flag holding the current state of connectivity.
	bool connected_;

	/// The name of the element.
	AMProcessVariable *name_;
	/// The dwell time PV name.
	AMProcessVariable *dwellTimePV_;
	/// The time scaling.
	AMProcessVariable *scale_;
	/// The time offset.
	AMProcessVariable *offset_;
	/// The time units.
	AMProcessVariable *units_;
	/// The mode PV name.
	AMProcessVariable *modePV_;
	/// Value for single shot mode.
	AMProcessVariable *singleShot_;
	/// Value for continuous mode.
	AMProcessVariable *continuous_;
	/// The trigger PV name.
	AMProcessVariable *triggerPV_;
	/// Whether the trigger is normal or inverted.
	AMProcessVariable *trigger_;
	/// The amount of time the pre-trigger needs.
	AMProcessVariable *preTrigger_;
	/// The amount of time the dwell needs to be held up.
	AMProcessVariable *dwellHold_;
	/// The status PV name.
	AMProcessVariable *statusPV_;
	/// The wait for enum.
	AMProcessVariable *waitFor_;
	/// The amount of time the dwell time element is meant to wait for.
	AMProcessVariable *delay_;
	/// The wait PV name.
	AMProcessVariable *waitPV_;
	/// The wait value.
	AMProcessVariable *waitValue_;
};

#endif // CLSSYNCHRONIZEDDWELLTIMECONFIGURATION_H
