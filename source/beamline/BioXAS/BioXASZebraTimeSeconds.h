#ifndef BIOXASZEBRATIMESECONDS_H
#define BIOXASZEBRATIMESECONDS_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"

#define BIOXASZEBRATIMESECONDS_TIME_VALUE_MIN 0.0
#define BIOXASZEBRATIMESECONDS_TIME_VALUE_MAX 6.5

#define BIOXASZEBRATIMESECONDS_VALUE_MIN BIOXASZEBRATIMESECONDS_TIME_VALUE_MIN
#define BIOXASZEBRATIMESECONDS_VALUE_MAX BIOXASZEBRATIMESECONDS_TIME_VALUE_MAX * 10

class BioXASZebraTimeSeconds : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraTimeSeconds(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraTimeSeconds();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }
	/// Returns true if a calibration is always possible, provided this control is connected. False otherwise.
	virtual bool shouldCalibrate() const { return false; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;

	/// Returns the time value control.
	AMPVControl* timeValueControl() const { return timeValue_; }
	/// Returns the time units control.
	AMPVControl* timeUnitsControl() const { return timeUnits_; }

signals:
	/// Notifier that the time value control has changed.
	void timeValueControlChanged(AMPVControl*);
	/// Notifier that the time units control has changed.
	void timeUnitsControlChanged(AMPVControl*);

public slots:
	/// Sets the time value control.
	void setTimeValueControl(AMPVControl *newControl);
	/// sets the time units control.
	void setTimeUnitsControl(AMPVControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns true if the given time value falls within the range for acceptable values, false otherwise.
	bool validTimeValue(double timeValue) const;
	/// Returns true if the given time units are valid, false otherwise.
	bool validTimeUnits(double timeUnits) const;

	/// Returns the result of converting the given time value and time units to the desired time units.
	double convertTimeValue(double timeValue, double timeUnits, double desiredTimeUnits) const;

	/// Returns true if the control is connected, according to its child controls.
	bool childrenConnected() const;
	/// Returns true if the control is moving, according to its child controls.
	bool childrenMoving() const;

	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpointS);

protected:
	/// Enumeration of the different available time units.
	enum TimeUnits { MSeconds = 0, Seconds = 1, DSeconds = 2 };

	/// The time value control.
	AMPVControl *timeValue_;
	/// The time units control.
	AMPVControl *timeUnits_;
};

#endif // BIOXASZEBRATIMESECONDS_H
