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
	explicit CLSXIAFiltersControl(const QString &name, const QString &units, QObject *parent = 0);
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
	virtual bool validSetpoint(double setpoint) const { return validValue(setpoint); }

signals:
	/// Notifier that a filter actuator control has been added.
	void filterActuatorControlAdded(AMControl *newControl);
	/// Notifier that a filter actuator control has been removed.
	void filterActuatorControlRemoved(AMControl *control);
	/// Notifier that the filter thickness for a control has changed.
	void filterThicknessChanged(AMControl *control, double newThickness);

public slots:
	/// Adds a filter actuator control.
	void addFilterActuatorControl(AMControl *newControl, double filterThickness);
	/// Removes a filter actuator control.
	void removeFilterActuatorControl(AMControl *control);
	/// Sets the filter thickness associated with the given control.
	void setFilterThickness(AMControl *control, double newThickness);
	/// Removes the control and associated filter thickness.
	void removeFilterThickness(AMControl *control);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected() = 0;
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves this control.
	AMAction3* createMoveAction(double setpoint) = 0;

	/// Returns true if the given control is a valid filter actuator.
	bool validFilterActuator(AMControl *control) const;

	/// Returns the filter thickness associated with the given control, 0 if the control is not found.
	double filterThickness(AMControl *control) const;
	/// Returns the active filter thickness associated with the given control: its thickness if the control is in, 0 if not, and -1 if the control isn't found.
	double activeFilterThickness(AMControl *control) const;
	/// Returns the total active filter thickness: the sum of the filter thicknesses of the actuator controls that are in. Returns -1 if there are invalid children.
	double activeFilterThickness() const;

	/// Returns true if the given filter control is valid and in, false otherwise.
	virtual bool filterIsIn(AMControl *control) const = 0;
	/// Returns true if the given filter control is valid and out, false otherwise.
	virtual bool filterIsOut(AMControl *control) const = 0;

protected:
	/// The mapping between each filter actuator control and associated filter thickness.
	QMap<AMControl*, double> filterThicknessMap_;
};

#endif // CLSXIAFILTERSCONTROL_H
