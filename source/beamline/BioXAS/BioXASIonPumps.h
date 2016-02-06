#ifndef BIOXASIONPUMPS_H
#define BIOXASIONPUMPS_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

#define BIOXASIONPUMPS_BAD_STATE 2800

class BioXASIonPumps : public BioXASBiStateGroup
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Bad = 0, Good = 1 };

	/// Constructor.
	explicit BioXASIonPumps(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASIonPumps();

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const;

	/// Returns true if the given control is one of the ion pumps, false otherwise.
	bool hasIonPump(AMControl *control) const;

	/// Returns the list of ion pump controls.
	QList<AMControl*> ionPumpList() const { return children_; }
	/// Returns the list of ion pump controls in a Bad state.
	QList<AMControl*> badIonPumpsList() const;
	/// Returns the list of ion pump controls in a Good state.
	QList<AMControl*> goodIonPumpsList() const;

signals:
	/// Notifier that the ion pumps have changed.
	void ionPumpsChanged();

public slots:
	/// Adds an ion pump control.
	bool addIonPump(AMControl *newPump);
	/// Removes an ion pump control.
	bool removeIonPump(AMControl *pump);
	/// Clears all ion pump controls.
	bool clearIonPumps();

protected slots:
	/// Updates the current value. Reimplemented to display warning message if the state is not Good.
	virtual void updateValue();

protected:
	/// Creates and returns a move action. Always returns 0 as this control does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASIONPUMPS_H
