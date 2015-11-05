#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/AMPseudoMotorLadderControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmWindowOption.h"

class BioXASCarbonFilterFarmActuatorWindowControl : public AMPseudoMotorLadderControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns the list of windows.
	QList<BioXASCarbonFilterFarmWindowOption*> windows() const { return windows_; }
	/// Returns the list of windows that contain the given position value.
	QList<BioXASCarbonFilterFarmWindowOption*> windowsContaining(double position) const;
	/// Returns the list of window options that have the given filter.
	QList<BioXASCarbonFilterFarmWindowOption*> windowsWithFilter(double filter) const;
	/// Returns the window at the given index. Returns -1 if there is no window for that index.
	BioXASCarbonFilterFarmWindowOption* windowAt(int index) const;

	/// Returns the index of the given filter. Returns -1 if the given filter isn't valid or no match found.
	int indexOf(BioXASCarbonFilterFarmWindowOption *window) const;

signals:
	/// Notifier that the list of windows have changed.
	void windowsChanged();

public slots:
	/// Adds a window option. The window option must be valid and not already in the list of windows to be added successfully.
	void addWindow(BioXASCarbonFilterFarmWindowOption *newOption);
	/// Removes a window option.
	void removeWindow(BioXASCarbonFilterFarmWindowOption *option);
	/// Clears all window controls.
	void clearWindows();

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Generates a list of the enum states with the given list of filter options.
	virtual QStringList generateEnumStates() const;
	/// Generates a list of the move enum states with the given list of filter options.
	virtual QStringList generateMoveEnumStates() const;

protected:
	/// The list of window controls.
	QList<BioXASCarbonFilterFarmWindowOption*> windows_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
