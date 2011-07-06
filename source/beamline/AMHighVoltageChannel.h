/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMHIGHVOLTAGECHANNEL_H
#define AMHIGHVOLTAGECHANNEL_H

#include <QObject>

class AMHighVoltageChannel : public QObject
{
Q_OBJECT
public:
	/// Enum for the polarity of the channel (either positive, negative, or none)
	enum highVoltageChannelPolarity{
		negative = 0,
		positive = 1,
		none = 2
	};

	/// Enum for the power state of the channel (either off, on, or invalid)
	enum highVoltageChannelPowerState{
		isPowerOff = 0,
		isPowerOn = 1,
		isPowerInvalid = 2
	};

	/// Constructor
	/*! \param name A unique programmer description
		\param parent QObject parent class
	  */
	AMHighVoltageChannel(const QString &name, QObject *parent = 0);

	/// Returns the programmer name for this HV Channel
	QString name() const;
	/// Returns the human-readable description for this HV Channel (virtual because subclasses might write this value to the network)
	virtual QString description() const = 0;

	/// Returns whether or not the channel is connected and ready
	virtual bool isConnected() const;

	/// Returns the requested voltage for this channel (the setpoint voltage when the HV Channel is toggled to the power on state)
	virtual double demand() const = 0;
	/// Returns the actual voltage reading for this channel
	virtual double voltage() const = 0;

	/// Returns the enum for power state (subclasses should set this member based on their state)
	virtual highVoltageChannelPowerState powerState() const;
	/// Convenience function for powerState() == isOn
	virtual bool isOn() const;
	/// Convenience function for powerState() == isOff
	virtual bool isOff() const;

	/// Returns the status string for the channel (maybe "On", "Off", "Ramping Up", or "Ramping Down")
	virtual QString status() const = 0;

	/// Returns the supply current that this channel is using if the information is available
	virtual double current() const = 0;
	/// Returns whether or not this channel can report its current usage
	bool hasCurrent() const;

	/// Returns the polarity that this channel is set to
	virtual highVoltageChannelPolarity polarity() const;
	/// Returns whether or not this channel reports its polarity
	bool hasPolarity()const ;

public slots:
	/// Sets the human-readable description for this channel (sub-classes may write this to the network)
	virtual void setDescription(const QString &description) = 0;
	/// Sets the demand voltage (the setpoint to achieve when the channel is powered on)
	virtual void setDemand(double demand) = 0;
	/// Sets the power state of the channel (isOff and isOn are valid inputs; however, isInvalid IS NOT)
	virtual void setPowerState(highVoltageChannelPowerState powerState);
	/// Convenience function that set to isOn for true and isOff for false
	void setOn(bool on);
	/// Convenience function for setOn(false)
	void setOff();
	/// Sets the polarity of the channel if possible (negative and positive are valid inputs; however, none IS NOT)
	virtual bool setPolarity(highVoltageChannelPolarity polarity);

signals:
	/// Emitted when the human-readable description changes
	void descriptionChanged(const QString &description);
	/// Emitted when the isConnected() state changes
	void connected(bool connected);
	/// Emitted when the demand voltage (setpoint) changes
	void demandChanged(double demand);
	/// Emitted when the actual voltage feedback changes
	void voltageChanged(double voltage);
	/// Emitted when the channel changes power states
	void powerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState powerState);
	/// Emitted when the channel's actual voltage reaches the demand voltage (conveniece signal)
	void fullyPowered();
	/// Convenience signal for powerStateChanged(isOn)
	void turnedOn();
	/// Convenience signal for powerStateChanged(isOff)
	void turnedOff();
	/// Emitted when the channel status changes
	void statusChanged(const QString &status);
	/// Emitted when the supply current changes (if this channel reports its current)
	void currentChanged(double current);
	/// Emitted when the polarity of the channel changes (if this channel has a polarity AND can change polarities)
	void polarityChanged(AMHighVoltageChannel::highVoltageChannelPolarity polarity);

protected slots:
	/// Setter for subclasses to change whether or not the channel reports its current
	void setNoCurrent(bool noCurrent);
	/// Setter for subclasses to change whether or not the channel reports its polarity
	void setNoPolarity(bool noPolarity);

	/// Handles convenience signals for power state
	void onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState powerState);

protected:
	/// Programmer description
	QString name_;
	/// Power state of the channel (see enum)
	highVoltageChannelPowerState powerState_;
	/// Polarity of the channel (see enum)
	highVoltageChannelPolarity polarity_;

private:
	bool noCurrent_;
	bool noPolarity_;
};

#endif // AMHIGHVOLTAGECHANNEL_H
