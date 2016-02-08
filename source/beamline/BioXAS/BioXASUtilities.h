#ifndef BIOXASUTILITIES_H
#define BIOXASUTILITIES_H

#include "beamline/BioXAS/BioXASUtilitiesGroup.h"

class AMControl;
class AMReadOnlyPVControl;
class BioXASFrontEndShutters;
class BioXASShutters;
class BioXASValves;

class BioXASUtilities : public BioXASUtilitiesGroup
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASUtilities(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilities();

	/// Returns true if the control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the shutters value.
	double shuttersValue() const;
	/// Returns the valves state value for beampath valves.
	double beampathValvesValue() const;
	/// Returns the valves state value for all valves.
	double valvesValue() const;
	/// Returns the ion pumps state value.
	double ionPumpsValue() const;
	/// Returns the flow switches state value.
	double flowSwitchesValue() const;

	/// Returns true if the given control is one of the shutters, false otherwise.
	bool hasShutter(AMControl *control) const;
	/// Returns true if the given control is one of the beampath valves, false otherwise.
	bool hasBeampathValve(AMControl *control) const;
	/// Returns true if the given control is one of the valves, false otherwise.
	bool hasValve(AMControl *control) const;
	/// Returns true if the given control is one of the ion pumps, false otherwise.
	bool hasIonPump(AMControl *control) const;
	/// Returns true if the given control is one of the flow switches, false otherwise.
	bool hasFlowSwitch(AMControl *control) const;

	/// Returns the shutters control.
	BioXASShutters* shutters() const { return shutters_; }
	/// Returns the beampath valves control.
	BioXASValves* beampathValves() const { return beampathValves_; }
	/// Returns the valves control.
	BioXASValves* valves() const { return valves_; }
	/// Returns the ion pumps control.
	BioXASUtilitiesGroup* ionPumps() const { return ionPumps_; }
	/// Returns the flow switches control.
	BioXASUtilitiesGroup* flowSwitches() const { return flowSwitches_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();
	/// Notifier that the shutters state value has changed.
	void shuttersValueChanged(double newValue);
	/// Notifier that the beampath valves have changed.
	void beampathValvesChanged();
	/// Notifier that the beampath valves value has changed.
	void beampathValvesValueChanged(double newValue);
	/// Notifier that the valves have changed.
	void valvesChanged();
	/// Notifier that the valves state value has changed.
	void valvesValueChanged(double newValue);
	/// Notifier that the ion pumps have changed.
	void ionPumpsChanged();
	/// Notifier that the ion pumps state value has changed.
	void ionPumpsValueChanged(double newValue);
	/// Notifier that the flow switches have changed.
	void flowSwitchesChanged();
	/// Notifier that the flow switches state value has changed.
	void flowSwitchesValueChanged(double newValue);

public slots:
	/// Adds a shutter.
	bool addShutter(AMControl *newControl, double openValue, double closedValue);
	/// Removes a shutter.
	bool removeShutter(AMControl *control);
	/// Clears the shutters.
	bool clearShutters();

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

	/// Adds an ion pump.
	bool addIonPump(AMControl *newControl);
	/// Removes an ion pump.
	bool removeIonPump(AMControl *control);
	/// Clears the ion pumps.
	bool clearIonPumps();

	/// Adds a flow switch.
	bool addFlowSwitch(AMControl *newControl);
	/// Removes a flow switch.
	bool removeFlowSwitch(AMControl *control);
	/// Clears the flow switches.
	bool clearFlowSwitches();

protected slots:
	/// Updates the moving state. Reimplemented to never show the utilities as moving.
	virtual void updateMoving();

protected:
	/// Creates and returns a new move action. Always returns 0 as this control does not support moving (yet?).
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the current value index.
	virtual int currentIndex() const;

protected:
	/// The shutters control.
	BioXASShutters *shutters_;
	/// The beampath valves control.
	BioXASValves *beampathValves_;
	/// The valves control.
	BioXASValves *valves_;
	/// The ion pumps control.
	BioXASUtilitiesGroup *ionPumps_;
	/// The flow switches control.
	BioXASUtilitiesGroup *flowSwitches_;
};

#endif // BIOXASUTILITIES_H
