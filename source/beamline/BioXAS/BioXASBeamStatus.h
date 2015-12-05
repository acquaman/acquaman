#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/AMEnumeratedControl.h"

class BioXASFrontEndValves;
class BioXASSideValves;
class BioXASMainValves;
class BioXASImagingValves;

class BioXASBeamStatus : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { On = 0, Off = 1 };

	/// Constructor.
	explicit BioXASBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatus();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the valves are open, false otherwise.
	virtual bool isOn() const { return false; }
	/// Returns true if the valves are closed, false otherwise.
	virtual bool isOff() const { return false; }

	/// Returns the front end valves.
	BioXASFrontEndValves* frontEndValves() const { return frontEndValves_; }
	/// Returns the Side valves.
	BioXASSideValves* sideValves() const { return sideValves_; }
	/// Returns the Main valves.
	BioXASMainValves* mainValves() const { return mainValves_; }
	/// Returns the Imaging valves.
	BioXASImagingValves* imagingValves() const { return imagingValves_; }

signals:
	/// Notifier that the front end valves have changed.
	void frontEndValvesChanged(BioXASFrontEndValves *newValves);
	/// Notifier that the Side valves have changed.
	void sideValvesChanged(BioXASSideValves *newValves);
	/// Notifier that the Main valves have changed.
	void mainValvesChanged(BioXASMainValves *newValves);
	/// Notifier that the Imaging valves have changed.
	void imagingValvesChanged(BioXASImagingValves *newValves);

public slots:
	/// Sets the front end valves.
	void setFrontEndValves(BioXASFrontEndValves *newValves);
	/// Sets the Side valves.
	void setSideValves(BioXASSideValves *newValves);
	/// Sets the Main valves.
	void setMainValves(BioXASMainValves *newValves);
	/// Sets the Imaging valves.
	void setImagingValves(BioXASImagingValves *newValves);

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

	/// Returns the current index. Subclasses must reimplement for their specific behavior/interaction.
	virtual int currentIndex() const;

protected:
	/// The front end valves.
	BioXASFrontEndValves *frontEndValves_;
	/// The Side valves.
	BioXASSideValves *sideValves_;
	/// The Main valves.
	BioXASMainValves *mainValves_;
	/// The Imaging valves.
	BioXASImagingValves *imagingValves_;
};

#endif // BIOXASBEAMSTATUS_H
