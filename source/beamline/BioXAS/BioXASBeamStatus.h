#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/AMEnumeratedControl.h"

class BioXASBeamStatusState;

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

	/// Returns the components that contribute to the beam status.
	QList<AMControl*> components() const { return children_; }

	/// Returns true if the component is in the beam off state, false otherwise.
	virtual bool componentIsOff(AMControl *control) const;
	/// Returns true if the component is in the beam on state, false otherwise.
	virtual bool componentIsOn(AMControl *control) const;

	/// Returns the list of beam status states associated with a given component.
	QList<BioXASBeamStatusState> componentBeamStatusStates(AMControl *control) const;

signals:
	/// Notifier that the components have changed.
	void componentsChanged();

public slots:
	/// Adds a component related to the beam status.
	bool addComponent(AMControl *control, const QList<BioXASBeamStatusState> beamStatusStates);
	/// Removes a component.
	bool removeComponent(AMControl *control);
	/// Clears all components.
	bool clearComponents();

protected:
	/// Returns a newly created move action.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Returns a newly created move to 'beam off' action.
	virtual AMAction3* createMoveToOffAction() { return 0; }
	/// Returns a newly created move to 'beam on' action.
	virtual AMAction3* createMoveToOnAction() { return 0; }

	/// Returns the index for the current value.
	virtual int currentIndex() const;

protected:
	/// The component and beam status states mapping.
	QMultiMap<AMControl*, BioXASBeamStatusState> componentBeamStatusMap_;
};

class BioXASBeamStatusState
{
public:
	/// Constructor.
	BioXASBeamStatusState(BioXASBeamStatus::Value beamStatusValue, double controlMinValue, double controlMaxValue) {
		beamStatusValue_ = beamStatusValue;
		controlMinValue_ = controlMinValue;
		controlMaxValue_ = controlMaxValue;
	}

	/// Alternate constructor. Assumes that the control min and max values are the same, as is the case for most enumerated controls.
	BioXASBeamStatusState(BioXASBeamStatus::Value beamStatusValue, double controlValue) {
		beamStatusValue_ = beamStatusValue;
		controlMinValue_ = controlValue;
		controlMaxValue_ = controlValue;
	}

	/// Destructor.
	virtual ~BioXASBeamStatusState() {}

	/// Returns the beam status value.
	BioXASBeamStatus::Value beamStatusValue() const { return beamStatusValue_; }
	/// Returns the control min value.
	double controlMinValue() const { return controlMinValue_; }
	/// Returns the control max value.
	double controlMaxValue() const { return controlMaxValue_; }

protected:
	/// The beam status value.
	BioXASBeamStatus::Value beamStatusValue_;
	/// The control min value.
	double controlMinValue_;
	/// The control max value.
	double controlMaxValue_;
};


#endif // BIOXASBEAMSTATUS_H
