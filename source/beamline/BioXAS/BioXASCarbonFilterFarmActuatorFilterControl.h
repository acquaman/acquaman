#ifndef BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"

class BioXASCarbonFilterFarmActuatorFilterControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorFilterControl();

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

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the list of filters.
	QList<double> filters() const { return filters_; }
	/// Returns the filter at the given (enum) index. Returns -1 if there isn't a valid filter associated with the given index.
	double filterAt(int index) const;

	/// Returns the index of the given filter. Returns -1 if the given filter isn't valid or no match found.
	int indexOf(double filter) const;
	/// Returns the index of the given filter (string), handles all enum states (including "Unknown"). Returns -1 if no match found.
	int indexOf(const QString filterString) const;

	/// Returns the current window control.
	AMControl* currentWindow() { return currentWindow_; }

signals:
	/// Notifier that the window control has changed.
	void currentWindowChanged(AMControl *newControl);
	/// Notifier that the filters have changed.
	void filtersChanged();

public slots:
	/// Sets the current window control.
	void setCurrentWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Sets the filter window combination preference for a filter already added. This is used to set a preference when many windows may have the same filter.
	void setFilterWindowPreference(double filter, BioXASCarbonFilterFarmWindowOption *window);
	/// Clears the filter window combination preference for the given filter.
	void clearFilterWindowPreference(double filter);
	/// Clears the filter window preference for the given window option.
	void clearFilterWindowPreference(BioXASCarbonFilterFarmWindowOption *option);
	/// Clears the filter window preferences.
	void clearFilterWindowPreferences();

protected slots:
	/// Updates the current states. Reimplemented to make sure the filters list and enum states are updated before the current value.
	virtual void updateStates();
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();
	/// Updates the maximum value.
	virtual void updateMaximumValue();

	/// Adds a filter to the filters list.
	void addFilter(BioXASCarbonFilterFarmWindowOption *newOption);
	/// Clears the existing filters list.
	void clearFilters();
	/// Updates the filters list with the filters that are available from the window control.
	void updateFilters();

	/// Updates the control's enumerated states.
	void updateEnumStates();

protected:
	/// Returns a new action that moves the actuator to the desired filter index setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Generates a list of the enum states with the given list of filter options.
	QStringList generateEnumStates(const QList<double> &filterOptions) const;
	/// Generates a list of the move enum states with the given list of filter options.
	QStringList generateMoveEnumStates(const QList<double> &filterOptions) const;

	/// Returns a string representation of the given filter.
	QString filterToString(double filter) const;

protected:
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *currentWindow_;

	/// The list of valid filter values. Indexed by enum index.
	QList<double> filters_;
	/// The mapping between filters and windows. All filters have at least one entry in this map.
	QMultiMap<double, BioXASCarbonFilterFarmWindowOption*> filterWindowMap_;
	/// The preferred mapping between filter and window. A filter will only have an entry in this map if a preference has been set.
	QMap<double, BioXASCarbonFilterFarmWindowOption*> filterWindowPreferenceMap_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
