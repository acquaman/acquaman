#ifndef BIOXASVALVESSETCONTROL_H
#define BIOXASVALVESSETCONTROL_H

#include "beamline/BioXAS/BioXASValvesControl.h"

class BioXASValvesSetControl : public BioXASValvesControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValvesSetControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesSetControl();

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;

	/// Returns true if this control's value is Open, false otherwise.
	virtual bool isOpen() const;
	/// Returns true if this control's value is Closed, false otherwise.
	virtual bool isClosed() const;

	/// Returns the valve set.
	AMControlSet* valveSet() const { return valveSet_; }

signals:
	/// Notifier that the valve set has changed.
	void valveSetChanged(AMControlSet *newSet);

public slots:
	/// Sets the valve set.
	void setValveSet(AMControlSet *newSet);

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();

protected:
	/// Creates and returns an action that opens the valves in the given control set.
	virtual AMAction3* createOpenValvesAction();

protected:
	/// The set of front-end valves.
	AMControlSet *valveSet_;
};

#endif // BIOXASVALVESSETCONTROL_H
