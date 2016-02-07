#ifndef BIOXASUTILITIESSTATE_H
#define BIOXASUTILITIESSTATE_H

#include "beamline/AMEnumeratedControl.h"

class BioXASIonPumps;
class BioXASValves;
class BioXASShutters;

class BioXASUtilitiesState : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Good = 0, Bad = 1 };

	/// Constructor.
	BioXASUtilitiesState(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilitiesState();

	/// Returns true if a move is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return false; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise. Subclasses should reimplement if they want to consider only a subset of children.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if this control is in Good state. Returns false otherwise.
	bool isGood() const;
	/// Returns true if this control is in Bad state. Returns false otherwise.
	bool isBad() const;

	/// Returns the shutters.
	BioXASShutters* shutters() const { return shutters_; }
	/// Returns the valves.
	BioXASValves* valves() const { return valves_; }
	/// Returns the ion pumps.
	BioXASIonPumps* ionPumps() const { return ionPumps_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged(BioXASShutters *newControl);
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASValves *newControl);
	/// Notifier that the ion pumps have changed.
	void ionPumpsChanged(BioXASIonPumps *newControl);

public slots:
	/// Sets the shutters.
	void setShutters(BioXASShutters *newControl);
	/// Sets the valves.
	void setValves(BioXASValves *newControl);
	/// Sets the ion pumps.
	void setIonPumps(BioXASIonPumps *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates a new move action. Always returns 0, as the state does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the current index.
	virtual int currentIndex() const;

protected:
	/// The shutters.
	BioXASShutters *shutters_;
	/// The valves.
	BioXASValves *valves_;
	/// The ion pumps.
	BioXASIonPumps *ionPumps_;
};

#endif // BIOXASUTILITIESSTATE_H
