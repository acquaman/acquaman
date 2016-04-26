#ifndef BIOXASUTILITIES_H
#define BIOXASUTILITIES_H

#include "beamline/BioXAS/BioXASUtilitiesGroup.h"

class AMControl;
class AMReadOnlyPVControl;
class AMBeamlineControl;
class AMBeamlineControlGroup;

class BioXASFrontEndShutters;
class BioXASShutters;
class BioXASValves;

class BioXASUtilities : public BioXASUtilitiesGroup
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASUtilities(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilities();

	/// Returns true if the control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the shutters value.
	double shuttersValue() const;

	/// Returns true if the given control is one of the shutters, false otherwise.
	bool hasShutter(AMControl *control) const;

	/// Returns the shutters control.
	BioXASShutters* shutters() const { return shutters_; }

signals:
	/// Notifier that the shutters state value has changed.
	void shuttersValueChanged(double newValue);

public slots:
	/// Adds a shutter.
	bool addShutter(AMControl *newControl, double openValue, double closedValue);
	/// Removes a shutter.
	bool removeShutter(AMControl *control);
	/// Clears the shutters.
	bool clearShutters();

protected slots:
	/// Updates the moving state. Reimplemented to never show the utilities as moving.
	virtual void updateMoving();

protected:
	/// Creates and returns a new move action. Always returns 0 as this control does not support moving (yet?).
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the current value index.
	virtual int currentIndex() const;

protected:
	/// The shutters control.
	BioXASShutters *shutters_;
};

#endif // BIOXASUTILITIES_H
