#ifndef AMBRANCHSELECTIONCONTROL_H
#define AMBRANCHSELECTIONCONTROL_H

#include <QObject>

#include "beamline/AMEnumeratedControl.h"

class AMPVwStatusControl;
class AMBranchSelectionControl : public AMEnumeratedControl
{
	Q_OBJECT
public:
	enum Branch {
		BranchA = 0,
		BranchB = 1,
		Between = 2
	};

	explicit AMBranchSelectionControl(QObject *parent = 0);

	virtual ~AMBranchSelectionControl() {}

	/// Returns true. The branch should be able to measure when connected
	virtual bool shouldMeasure() const;
	/// Returns true. The branch should be able to move when connected
	virtual bool shouldMove() const;
	/// Returns false. A branch selection cannot be stopped mid move.
	virtual bool shouldStop() const;

	/// Returns true if the branch can be determined right now.
	virtual bool canMeasure() const;
	/// Returns true if the branch can be moved right now.
	virtual bool canMove() const;
	/// Returns false. The branch move can never be stopped.
	virtual bool canStop() const;

	/// Returns true if the given value is a valid branch feedback value (ie A, B, Between or Unknown)
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid branch setpoint value (ie A or B only)
	virtual bool validSetpoint(double value) const;

signals:

public slots:

protected slots:
	virtual void updateConnected();

	virtual void updateMoving();

protected:

	virtual int currentIndex() const;

	AMPVwStatusControl *branchSelectionPVControl_;
};

#endif // AMBRANCHSELECTIONCONTROL_H
