#ifndef AMIONCHAMBER_H
#define AMIONCHAMBER_H

#include "dataman/info/AMIonChamberInfo.h"
#include "beamline/AMDetector.h"

/*!
  This class is an abstraction for general ion chambers.
  */
class AMIonChamber : public AMIonChamberInfo, public AMDetector
{
	Q_OBJECT
public:
	/// Constructor.  Builds an ion chamber.
	explicit AMIonChamber(const QString &name, const QString &description, QObject *parent = 0);

	/// Get the current reading
	virtual double reading() const { return counts(); }
	/// Returns whether the detector is connected.
	virtual bool isConnected() const { return false; }

	/// AMDetector sub classes need to reimplement this to return their own detectorInfo class. NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	virtual AMDetectorInfo* toInfo() const { return new AMIonChamberInfo(*this); }
	/// Info specific to the ion chamber.
	AMIonChamberInfo toIonChamberInfo() const { return AMIonChamberInfo(*this); }

	/// The description() of a detector is a human-readable, free-form string.
	virtual QString description() const { return AMIonChamberInfo::description(); }
	/// Descriptions can be changed at will, and the detector will emit infoChanged() when this happens.
	virtual void setDescription(const QString& description) { AMIonChamberInfo::setDescription(description); }

	/// Sets the current detector from the given detector info.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Sets the current detector from the given detector info.
	bool setFromIonChamberInfo(const AMIonChamberInfo &info);

	/// Pure virtual function.  Returns the current counts for the ion chamber.
	virtual double counts() const = 0;
	/// Pure virtual function.  Returns the voltage for the ion chamber.
	virtual double voltage() const = 0;

	/// Returns whether the voltage is within the linear range of the ion chamber.
	bool withinLinearRange() const { return voltage() <= maximumVoltage() && voltage() >= minimumVoltage(); }
	/// Specific helper function that returns whether the voltage is too low.  Returns false if withinLinearRange is true.
	bool voltageTooLow() const { return !withinLinearRange() && voltage() < minimumVoltage(); }
	/// Specific helper function that returns whether the voltage is too high.  Returns false if withinLinearRange is true.
	bool voltageTooHigh() const { return !withinLinearRange() && voltage() > maximumVoltage(); }

	/// Pure-virtual function.  Returns whether the ion chamber is at maximum senstivity.
	virtual bool atMaximumSensitivity() const = 0;
	/// Pure-virtual function.  Returns whether the ion chamber is at minimum sensitivity.
	virtual bool atMinimumSensitivity() const = 0;

signals:
	/// Notifier that the ion chamber is connected.  True only when all the components of the ion chamber are connected.
	void connected(bool);
	/// General notifier that the reading (output) has changed.  Emitted when either counts or voltage has changed.
	void readingsChanged();
	/// Notifier that the counts have changed.
	void countsChanged(double);
	/// Notifier that the voltage has changed.
	void voltageChanged(double);
	/// Notifier that the sensitivity has changed.
	void sensitivityChanged();
	/// Notifier that the ion chamber is at the minimum sensitivity.  Passes the truth value.
	void minimumSensitivity(bool);
	/// Notifier that the ion chamber is at the maximums sensitivity.  Passes the truth value.
	void maximumSensitivity(bool);

public slots:
	/// Pure virtual function.  Increases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool increaseSensitivity() = 0;
	/// Pure virtual function.  Decreases the sensitivity of the ion chamber.  How this is done is implementation specific.
	virtual bool decreaseSensitivity() = 0;
};

#endif // AMIONCHAMBER_H
