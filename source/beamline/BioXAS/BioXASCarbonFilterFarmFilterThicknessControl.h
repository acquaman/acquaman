#ifndef BIOXASCARBONFILTERFARMFILTERTHICKNESSCONTROL_H
#define BIOXASCARBONFILTERFARMFILTERTHICKNESSCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl.h"

class BioXASCarbonFilterFarmFilterThicknessControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmFilterThicknessControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *upstreamFilterThickness, BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *downstreamFilterThickness, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmFilterThicknessControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value corresponds to a valid total filter thickness, false otherwise.
	virtual bool validValue(double value) const;

	/// Returns the upstream actuator filter thickness control.
	AMControl* upstreamFilterThicknessControl() const { return upstreamFilterThickness_; }
	/// Returns the downstream actuator filter thickness control.
	AMControl* downstreamFilterThicknessControl() const { return downstreamFilterThickness_; }

	/// Returns the upstream filter thickness contribution associated with the given total filter thickness.
	double upstreamFilterThickness(double totalFilterThickness);
	/// Returns the downstream filter thickness contribution associated with the given total filter thickness.
	double downstreamFilterThickness(double totalFilterThickness);

signals:
	/// Notifier that the upstream actuator filter thickness control has changed.
	void upstreamFilterThicknessControlChanged(AMControl *newControl);
	/// Notifier that the downstream actuator filter thickness control has changed.
	void downstreamFilterThicknessControlChanged(AMControl *newControl);

public slots:
	/// Sets the upstream actuator filter thickness control.
	void setUpstreamFilterThicknessControl(BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *newControl);
	/// Sets the downstream actuator filter thickness control.
	void setDownstreamFilterThicknessControl(BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *newControl);
	/// Sets a total filter thickness to upstream and downstream filter thicknesses mapping.
	void setFilterThickness(double totalFilterThickness, double upstreamFilterThickness, double downstreamFilterThickness);

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
	/// The upstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *upstreamFilterThickness_;
	/// The downstream actuator filter thickness control.
	BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *downstreamFilterThickness_;
};

#endif // BIOXASCARBONFILTERFARMFILTERTHICKNESSCONTROL_H
