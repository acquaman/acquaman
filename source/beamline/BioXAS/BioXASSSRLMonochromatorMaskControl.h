#ifndef BIOXASSSRLMONOCHROMATORMASKCONTROL_H
#define BIOXASSSRLMONOCHROMATORMASKCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"

class BioXASSSRLMonochromatorMaskControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum describing the different mask control values.
	enum Value { Open = 0, Closed = 1, None = 2 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMaskControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMaskControl();

	/// Returns true if this control is always measurable, provided it is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided this control is connected.
	virtual bool shouldMove() const { return false; }
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const { Q_UNUSED(value) return false; }

	/// Returns true if this control is open, false otherwise.
	bool isOpen() const { return !isClosed(); }
	/// Returns true if this control is closed, false otherwise.
	bool isClosed() const;

	/// Returns a string representation of the given value.
	static QString valueToString(BioXASSSRLMonochromatorMaskControl::Value value);

	/// Returns the mask upper blade control.
	AMPVwStatusControl* upperBlade() const { return upperBlade_; }
	/// Returns the mask lower blade control.
	AMPVwStatusControl* lowerBlade() const { return lowerBlade_; }

signals:
	/// Notifier that the mask upper blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the mask lower blade control has changed.
	void lowerBladeChanged(AMControl *newControl);

public slots:
	/// Sets the mask upper blade control.
	void setUpperBlade(AMPVwStatusControl *newControl);
	/// Sets the mask lower blade control.
	void setLowerBlade(AMPVwStatusControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action to the given setpoint.
	AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

protected:
	/// The mask upper blade control.
	AMPVwStatusControl *upperBlade_;
	/// The mask lower blade control.
	AMPVwStatusControl *lowerBlade_;
};

#endif // BIOXASSSRLMONOCHROMATORMASKCONTROL_H
