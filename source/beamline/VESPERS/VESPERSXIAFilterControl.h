#ifndef VESPERSXIAFILTERCONTROL_H
#define VESPERSXIAFILTERCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

/// This class handles the filter thicknesses so that they can become actionable.
class VESPERSXIAFilterControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSXIAFilterControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSXIAFilterControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;
};

#endif // VESPERSXIAFILTERCONTROL_H
