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

	/// Returns true if the total filter is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value (enum index) corresponds to a valid value option (includes "Unknown"), false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value (enum index) corresponds to a valid setpoint option (does not include "Unknown"), false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the list of filters.
	QList<double> filters() const { return filters_; }
	/// Returns the filter at the given (enum) index. Returns -1 if there isn't a valid filter associated with the given index.
	double filterAt(int index) const;

	/// Returns the index of the given filter. Returns -1 if the given filter isn't valid or no match found.
	int indexOf(double filter) const;
	/// Returns the index of the given filter (string), handles all enum states (including "Unknown"). Returns -1 if no match found.
	int indexOf(const QString &filterString) const;

	/// Returns the upstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* upstreamFilter() const { return upstreamFilter_; }
	/// Returns the downstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* downstreamFilter() const { return downstreamFilter_; }

signals:
	/// Notifier that the upstream actuator filter control has changed.
	void upstreamFilterChanged(AMControl *newControl);
	/// Notifier that the downstream actuator filter control has changed.
	void downstreamFilterChanged(AMControl *newControl);
	/// Notifier that the filters have changed.
	void filtersChanged();

public slots:
	/// Sets the upstream actuator filter control.
	void setUpstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);
	/// Sets the downstream actuator filter control.
	void setDownstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);

protected slots:
	/// Updates the current states.
	virtual void updateStates();
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();
	/// Updates the maximum value.
	virtual void updateMaximumValue();

	/// Adds a filter to the internal list of filter options.
	void addFilter(double filter);
	/// Clears the existing filters.
	void clearFilters();
	/// Updates the list of filter options.
	void updateFilters();

	/// Updates the control's enumerated states.
	void updateEnumStates();

protected:
	/// Returns a new action that moves both actuators to the desired filter.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Generates a list of the enum states with the given list of filter options.
	QStringList generateEnumStates(const QList<double> &filterOptions) const;
	/// Generates a list of the move enum states with the given list of filter options.
	QStringList generateMoveEnumStates(const QList<double> &filterOptions) const;

	/// Returns the total filter (enum) index, given the upstream and downstream filter values.
	int totalFilterIndex(double upstreamFilter, double downstreamFilter) const;
	/// Returns the total filter value given the upstream and downstream filter values.
	double totalFilter(double upstreamFilter, double downstreamFilter) const;

	/// Returns a string representation of the given filter.
	QString filterToString(double filter) const;

protected:
	/// The upstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *upstreamFilter_;
	/// The downstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *downstreamFilter_;

	/// The list of valid effective filter values. These represent the different combinations of upstream and downstream filters. Indexed by enum value.
	QList<double> filters_;
	/// The mapping between total filter and the upstream filter contribution.
	QMap<double, double> upstreamFilterMap_;
	/// The mapping between total filter and the downstream filter contribution.
	QMap<double, double> downstreamFilterMap_;
};

#endif // BIOXASCARBONFILTERFARMFILTERCONTROL_H
