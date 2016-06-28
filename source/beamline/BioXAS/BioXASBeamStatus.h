#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/AMEnumeratedControl.h"

class BioXASBeamStatus : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible beam status values.
	enum Value { Off = 0, On = 1 };

	/// Constructor.
	BioXASBeamStatus(const QString &name, QObject *parent);
	/// Destructor.
	virtual ~BioXASBeamStatus();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now, false otherwise.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now, false otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if the beam is off, false otherwise.
	virtual bool isOff() const;
	/// Returns true if the beam is on, false otherwise.
	virtual bool isOn() const;

public slots:
	/// Adds a component related to the beam status.
	bool addComponent(AMControl *control, double beamOffValue, double beamOnValue);
	/// Removes a component.
	bool removeComponent(AMControl *control);
	/// Clears all components.
	bool clearComponents();

protected:
	/// Returns the index for the current value.
	virtual int currentIndex() const;

protected:
	/// The component control and beam off value mapping.
	QMap<AMControl*, double> componentBeamOffValueMap_;
	/// The component control and beam on value mapping.
	QMap<AMControl*, double> componentBeamOnValueMap_;
};

#endif // BIOXASBEAMSTATUS_H
