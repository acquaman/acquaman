#ifndef BIOXASXIAFILTERSCONTROL_H
#define BIOXASXIAFILTERSCONTROL_H

#include "beamline/CLS/CLSXIAFiltersControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASXIAFiltersControl : public CLSXIAFiltersControl
{
    Q_OBJECT

public:
	/// Enum describing the possible IN/OUT state of each filter actuator.
	class Filter { public: enum State { Out = 1, In = 2 }; };
	/// Constructor.
	explicit BioXASXIAFiltersControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersControl();

	/// Returns true if this cont rol is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;

	/// Returns a string representation of the given thickness, an empty string if no match is found.
	virtual QString thicknessToString(double thickness) const;

signals:
	/// Notifier that the first filter actuator control has changed.
	void firstFilterActuatorControlChanged(CLSBiStateControl *newControl);
	/// Notifier that the second filter actuator control has changed.
	void secondFilterActuatorControlChanged(CLSBiStateControl *newControl);
	/// Notifier that the third filter actuator control has changed.
	void thirdFilterActuatorControlChanged(CLSBiStateControl *newControl);
	/// Notifier that the fourth filter actuator control has changed.
	void fourthFilterActuatorControlChanged(CLSBiStateControl *newControl);

public slots:
	/// Sets the first filter actuator.
	void setFirstFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness);
	/// Sets the second filter actuator.
	void setSecondFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness);
	/// Sets the third filter actuator.
	void setThirdFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness);
	/// Sets the fourth filter actuator.
	void setFourthFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

	/// Returns true if the given filter is in, false if the filter is not found or if it is not in.
	virtual bool filterIsIn(AMControl *control) const;
	/// Returns true if the given filter is out, false if the filter is not found or if it is not out.
	virtual bool filterIsOut(AMControl *control) const;

protected:
	/// Returns a new action that moves this control.
	AMAction3* createMoveAction(double setpoint);

protected:
	/// The first filter actuator control.
	CLSBiStateControl *firstFilterActuator_;
	/// The second filter actuator control.
	CLSBiStateControl *secondFilterActuator_;
	/// The third filter actuator control.
	CLSBiStateControl *thirdFilterActuator_;
	/// The fourth filter actuator control.
	CLSBiStateControl *fourthFilterActuator_;
};

#endif // BIOXASXIAFILTERSCONTROL_H
