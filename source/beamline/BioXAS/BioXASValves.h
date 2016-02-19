#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASValves : public BioXASBiStateGroup
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASValves(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	/// Returns true if this control is open, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isOpen() const;
	/// Returns true if this control is closed, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isClosed() const;

	/// Returns true if the given control is a child of the valves, false otherwise.
	bool hasValve(AMControl *control) const;

	/// Returns the list of valve controls.
	QList<AMControl*> valvesList() const { return children_; }
	/// Returns the list of valve controls that are open.
	QList<AMControl*> openValvesList() const;
	/// Returns the list of valve controls that are closed.
	QList<AMControl*> closedValvesList() const;

signals:
	/// Notifier that the valves have changed.
	void valvesChanged();

public slots:
	/// Adds a valve control.
	bool addValve(AMControl *newValve, double openValue, double closedValue);
	/// Removes a valve control.
	bool removeValve(AMControl *newValve);
	/// Clears all valve controls.
	bool clearValves();

protected slots:
	/// Updates the moving state. Reimplemented so that the valves never report as moving.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returns a new move action to Open.
	AMAction3* createMoveToOpenAction();

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASVALVES_H
