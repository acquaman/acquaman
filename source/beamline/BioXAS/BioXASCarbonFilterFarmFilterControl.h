#ifndef BIOXASCARBONFILTERFARMFILTERCONTROL_H
#define BIOXASCARBONFILTERFARMFILTERCONTROL_H

#include "beamline/AMEnumeratedControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

/*
  This is a control that's meant to control two actuators of a carbon filter farm
  through mapping the contributions of both actuator filters to a single total
  'effective' filter. The available value options for this control are derived
  from the filter options for each of the actuator controls.
  */
class BioXASCarbonFilterFarmFilterControl : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmFilterControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmFilterControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the upstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* upstreamFilter() const { return upstreamFilter_; }
	/// Returns the downstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl* downstreamFilter() const { return downstreamFilter_; }

	/// Returns the filter value at the given index.
	double filterAt(int index) const;

signals:
	/// Notifier that the upstream actuator filter control has changed.
	void upstreamFilterChanged(BioXASCarbonFilterFarmActuatorFilterControl *newControl);
	/// Notifier that the downstream actuator filter control has changed.
	void downstreamFilterChanged(BioXASCarbonFilterFarmActuatorFilterControl *newControl);
	/// Notifier that the filter options have changed.
	void filtersChanged();

public slots:
	/// Sets the upstream actuator filter control.
	void setUpstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);
	/// Sets the downstream actuator filter control.
	void setDownstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl);

protected slots:
	/// Updates the enumeration options.
	virtual void updateOptions();

	/// Adds an enum value option, assumes that the index will be the next available value (incrementing from zero).
	void addFilterOption(const QString &optionString, double filter, double upstreamFilterIndex, double downstreamFilterIndex);
	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	void addFilterOption(double index, const QString &optionString, double filter, double upstreamFilterIndex, double downstreamFilterIndex);
	/// Removes an enum value option.
	virtual bool removeOption(double index);
	/// Clears all value options.
	virtual bool clearOptions();

protected:
	/// Returns the current connected state.
	virtual bool currentConnected() const;
	/// Returns the current value.
	virtual double currentValue() const;
	/// Returns the current moving state.
	virtual bool currentMoving() const;

	/// Returns a new action that moves both actuators to the desired filter.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Returns the total filter value, given the upstream and downstream filter indices.
	double totalFilterByIndices(int upstreamIndex, int downstreamIndex) const;
	/// Returns the total filter value, given the upstream and downstream filter values. Returns -1 if invalid filter values are provided.
	double totalFilter(double upstreamFilter, double downstreamFilter) const;

protected:
	/// The upstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *upstreamFilter_;
	/// The downstream actuator filter control.
	BioXASCarbonFilterFarmActuatorFilterControl *downstreamFilter_;

	/// The mapping between a filter option's index and its filter value.
	QMap<double, double> indexFilterMap_;
	/// The mapping between a filter option's index and the upstream actuator's filter index setpoint.
	QMap<double, double> indexUpstreamFilterIndexMap_;
	/// The mapping between a filter option's index and the downstream actuator's filter index setpoint.
	QMap<double, double> indexDownstreamFilterIndexMap_;
};

#endif // BIOXASCARBONFILTERFARMFILTERCONTROL_H
