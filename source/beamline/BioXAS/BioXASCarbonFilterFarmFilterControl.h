#ifndef BIOXASCARBONFILTERFARMFILTERCONTROL_H
#define BIOXASCARBONFILTERFARMFILTERCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASCarbonFilterFarmFilterControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmFilterControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmFilterControl();

	/// Returns true if the total filter thickness is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter thickness is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value corresponds to a valid total filter thickness, false otherwise.
	virtual bool validValue(double value) const;

	/// Returns the upstream actuator filter control.
	BioXASCarbonFilterFarmActuatorControl* upstreamActuator() const { return upstreamActuator_; }
	/// Returns the downstream actuator filter control.
	BioXASCarbonFilterFarmActuatorControl* downstreamActuator() const { return downstreamActuator_; }

//	/// Returns the upstream filter thickness contribution associated with the given total filter thickness.
//	double upstreamFilter(double totalFilterThickness);
//	/// Returns the downstream filter thickness contribution associated with the given total filter thickness.
//	double downstreamFilter(double totalFilterThickness);

signals:
	/// Notifier that the upstream actuator filter thickness control has changed.
	void upstreamActuatorChanged(AMControl *newControl);
	/// Notifier that the downstream actuator filter thickness control has changed.
	void downstreamActuatorChanged(AMControl *newControl);

public slots:
	/// Sets the upstream actuator filter thickness control.
	void setUpstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl);
	/// Sets the downstream actuator filter thickness control.
	void setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl);
//	/// Sets a total filter thickness to upstream and downstream filter thicknesses mapping.
//	void setFilterThickness(double totalFilterThickness, double upstreamFilterThickness, double downstreamFilterThickness);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves both actuators to the desired filter thickness.
	virtual AMAction3* createMoveAction(double setpoint);

protected:
	/// The mapping between total filter thickness and the desired upstream filter thickness.
	QMap<double, double> upstreamFilterThicknessesMap_;
	/// The mapping between total filter thickness and the desired downstream filter thickness.
	QMap<double, double> downstreamFilterThicknessesMap_;
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
};

#endif // BIOXASCARBONFILTERFARMFILTERCONTROL_H
