#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/AMEnumeratedControl.h"

class AMControlSet;

class BioXASValves : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASValves(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the valves are open, false otherwise.
	virtual bool isOpen() const;
	/// Returns true if the valves are closed, false otherwise.
	virtual bool isClosed() const;

	/// Returns the list of valves.
	QList<AMControl*> valvesList() const;

signals:
	/// Notifier that the valves have changed.
	void valvesChanged();

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

	/// Adds a valve to the valve set.
	void addValve(AMControl *newValve);
	/// Removes a valve from the valve set.
	void removeValve(AMControl *newValve);
	/// Clears the valve set.
	void clearValves();

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returns a new move action to Open.
	virtual AMAction3* createOpenValvesAction();

	/// Creates and returns a list of move enum states for this control, based on the current options provided.
	virtual QStringList generateEnumStates() const;

	/// Returns the current index. Subclasses must reimplement for their specific behavior/interaction.
	virtual int currentIndex() const;

protected:
	/// The set of valve controls.
	AMControlSet *valveSet_;
};

#endif // BIOXASVALVES_H
