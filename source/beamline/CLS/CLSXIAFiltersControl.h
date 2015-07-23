#ifndef CLSXIAFILTERSCONTROL_H
#define CLSXIAFILTERSCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"

#include <QMap>

class CLSXIAFiltersControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSXIAFiltersControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSXIAFiltersControl();

	/// Returns true if this control is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const = 0;
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const = 0;
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const = 0;

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const = 0;
	/// Returns true if this control can move right now.
	virtual bool canMove() const = 0;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const = 0;

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const = 0;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const = 0;

signals:
	/// Notifier that a filter actuator control has been added.
	void filterActuatorControlAdded(AMPVControl *newControl);
	/// Notifier that a filter actuator control has been removed.
	void filterActuatorControlRemoved(AMPVControl *control);

public slots:
	/// Adds a filter actuator control.
	void addFilterActuatorControl(AMPVControl *newControl, double filterThickness);
	/// Removes a filter actuator control.
	void removeFilterActuatorControl(AMPVControl *control);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected() = 0;
	/// Updates the current value.
	virtual void updateValue() = 0;
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves this control.
	AMAction3* createMoveAction(double setpoint) = 0;

protected:
	/// The mapping between each filter actuator control and associated filter thickness.
	QMap<AMPVControl*, double> filterThicknessMap_;
};

#endif // CLSXIAFILTERSCONTROL_H
