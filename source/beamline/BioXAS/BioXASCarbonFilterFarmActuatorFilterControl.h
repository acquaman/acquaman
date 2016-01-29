#ifndef BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"

/*
  This is a control that's meant to control a carbon filter farm actuator by filter,
  through mapping window control values to filters.
  */
class BioXASCarbonFilterFarmActuatorFilterControl : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorFilterControl();

	/// Returns the list of indices that have the given filter.
	QList<int> indicesWithFilter(double filter) const;

	/// Returns true if the given filter is one of the value options, false otherwise.
	bool hasFilter(double filter) const;
	/// Returns the filter value at the given index.
	double filterAt(int index) const;

	/// Returns the window control.
	BioXASCarbonFilterFarmActuatorWindowControl* windowControl() const { return qobject_cast<BioXASCarbonFilterFarmActuatorWindowControl*>(control_); }

signals:
	/// Notifier that the window control has changed.
	void windowChanged(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Notifier that the filter options have changed.
	void filtersChanged();

public slots:
	/// Sets the control used as the base for this control's values.
	void setWindowControl(BioXASCarbonFilterFarmActuatorWindowControl *newControl);

	/// Adds a filter option, using the given actuator window index and its filter.
	void addFilter(int windowIndex, double filter);
	/// Removes a filter option.
	void removeFilter(int windowIndex);
	/// Clears all filter options.
	void clearFilters();

	/// Sets a filter option preference. Useful when there may be multiple windows with the same filter, and a 'favorite' filter should be specified.
	void setWindowPreference(double filter, int windowIndex);
	/// Removes a filter option preference.
	void removeWindowPreference(double filter);
	/// Clears all filter option preferences.
	void clearWindowPreferences();

protected:
	/// Returns a new action that moves the control to the setpoint that corresponds to the given index setpoint. Reimplemented to check if there is a preferred window for the desired filter.
	virtual AMAction3* createMoveAction(double indexSetpoint);

protected:
	/// The mapping between a filter option's index and its filter value.
	QMap<int, double> indexFilterMap_;
	/// The mapping between the filter value and a corresponding window index. Used to specify a preference.
	QMap<double, int> filterWindowPreferenceMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
