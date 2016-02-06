#ifndef BIOXASUTILITIES_H
#define BIOXASUTILITIES_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASUtilitiesState;
class BioXASIonPumps;
class BioXASValves;

class BioXASUtilities : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASUtilities(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilities();

	/// Returns true if the control is connected.
	virtual bool isConnected() const;

	/// Returns the state value.
	double stateValue() const;
	/// Returns the ion pumps state value.
	double ionPumpsStateValue() const;
	/// Returns the valves state value.
	double valvesStateValue() const;

	/// Returns the state control.
	BioXASUtilitiesState* state() const { return state_; }
	/// Returns the ion pumps control.
	BioXASIonPumps* ionPumps() const { return ionPumps_; }
	/// Returns the valves control.
	BioXASValves* valves() const { return valves_; }

signals:
	/// Notifier that the state value has changed.
	void stateValueChanged(double newValue);
	/// Notifier that the ion pumps have changed.
	void ionPumpsChanged();
	/// Notifier tha the ion pumps state value has changed.
	void ionPumpsStateValueChanged(double newValue);
	/// Notifier that the valves have changed.
	void valvesChanged();
	/// Notifier that the valves state value has changed.
	void valvesStateValueChanged(double newValue);

public slots:
	/// Adds an ion pump.
	void addIonPump(AMControl *newControl);
	/// Removes an ion pump.
	void removeIonPump(AMControl *control);
	/// Clears the ion pumps.
	void clearIonPumps();

	/// Adds a valve.
	void addValve(AMControl *newControl, double openValue, double closedValue);
	/// Removes a valve.
	void removeValve(AMControl *control);
	/// Clears the valves.
	void clearValves();

protected:
	/// The state control.
	BioXASUtilitiesState *state_;
	/// The ion pumps control.
	BioXASIonPumps *ionPumps_;
	/// The valves control.
	BioXASValves *valves_;
};

#endif // BIOXASUTILITIES_H
