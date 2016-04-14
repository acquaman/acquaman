#ifndef BIOXASUTILITIES_H
#define BIOXASUTILITIES_H

#include "beamline/BioXAS/BioXASUtilitiesGroup.h"

class AMControl;
class AMReadOnlyPVControl;
class AMBeamlineControl;
class AMBeamlineControlGroup;

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

	/// Returns true if the given control is one of the shutters, false otherwise.
	bool hasShutter(AMControl *control) const;
	/// Returns true if the given control is one of the beampath valves, false otherwise.
	bool hasBeampathValve(AMControl *control) const;
	/// Returns true if the given control is one of the valves, false otherwise.
	bool hasValve(AMControl *control) const;

	/// Returns the shutters control.
	BioXASShutters* shutters() const { return shutters_; }
	/// Returns the beampath valves control.
	BioXASValves* beampathValves() const { return beampathValves_; }
	/// Returns the valves control.
	BioXASValves* valves() const { return valves_; }

signals:
	/// Notifier that the shutters state value has changed.
	void shuttersValueChanged(double newValue);
	/// Notifier that the beampath valves value has changed.
	void beampathValvesValueChanged(double newValue);
	/// Notifier that the valves state value has changed.
	void valvesValueChanged(double newValue);

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
};

#endif // BIOXASUTILITIES_H
