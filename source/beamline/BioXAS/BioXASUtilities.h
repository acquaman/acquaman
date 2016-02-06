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
	double ionPumpsValue() const;
	/// Returns the valves state value for beampath valves.
	double beampathValvesValue() const;
	/// Returns the valves state value for all valves.
	double valvesValue() const;

	/// Returns true if the given control is one of the ion pumps, false otherwise.
	bool hasIonPump(AMControl *control) const;
	/// Returns true if the given control is one of the beampath valves, false otherwise.
	bool hasBeampathValve(AMControl *control) const;
	/// Returns true if the given control is one of the valves, false otherwise.
	bool hasValve(AMControl *control) const;

	/// Returns the state control.
	BioXASUtilitiesState* state() const { return state_; }
	/// Returns the ion pumps control.
	BioXASIonPumps* ionPumps() const { return ionPumps_; }
	/// Returns the beampath valves control.
	BioXASValves* beampathValves() const { return beampathValves_; }
	/// Returns the valves control.
	BioXASValves* valves() const { return valves_; }

signals:
	/// Notifier that the state value has changed.
	void stateValueChanged(double newValue);
	/// Notifier that the ion pumps have changed.
	void ionPumpsChanged();
	/// Notifier tha the ion pumps state value has changed.
	void ionPumpsValueChanged(double newValue);
	/// Notifier that the beampath valves have changed.
	void beampathValvesChanged();
	/// Notifier that the beampath valves value has changed.
	void beampathValvesValueChanged(double newValue);
	/// Notifier that the valves have changed.
	void valvesChanged();
	/// Notifier that the valves state value has changed.
	void valvesValueChanged(double newValue);

public slots:
	/// Adds an ion pump.
	bool addIonPump(AMControl *newControl);
	/// Removes an ion pump.
	bool removeIonPump(AMControl *control);
	/// Clears the ion pumps.
	bool clearIonPumps();

	/// Adds a beampath valve.
	bool addBeampathValve(AMControl *newControl, double openValue, double closedValue);
	/// Removes a beampath valve.
	bool removeBeampathValve(AMControl *control);
	/// Clears the beampath valves.
	bool clearBeampathValves();

	/// Adds a valve.
	bool addValve(AMControl *newControl, double openValue, double closedValue);
	/// Removes a valve.
	bool removeValve(AMControl *control);
	/// Clears the valves.
	bool clearValves();

protected:
	/// The state control.
	BioXASUtilitiesState *state_;
	/// The ion pumps control.
	BioXASIonPumps *ionPumps_;
	/// The beampath valves control.
	BioXASValves *beampathValves_;
	/// The valves control.
	BioXASValves *valves_;
};

#endif // BIOXASUTILITIES_H
