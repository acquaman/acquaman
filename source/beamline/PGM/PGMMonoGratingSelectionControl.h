#ifndef PGMMONOGRATINGSELECTIONCONTROL_H
#define PGMMONOGRATINGSELECTIONCONTROL_H

#include "beamline/AMEnumeratedControl.h"

#define PGMMONOGRATINGSELECTIONCONTROL_PV_HIGH 1
#define PGMMONOGRATINGSELECTIONCONTROL_PV_MEDIUM 2
#define PGMMONOGRATINGSELECTIONCONTROL_PV_LOW 3
#define PGMMONOGRATINGSELECTIONCONTROL_PV_UNKNOWN 4

class AMPVwStatusControl;

class PGMMonoGratingSelectionControl : public AMEnumeratedControl
{
	Q_OBJECT

public:
	enum Grating {
		Low = 0,
		Medium = 1,
		High = 2
	};

	/// Constructor.
	PGMMonoGratingSelectionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~PGMMonoGratingSelectionControl() {}

	/// Returns true. The grating should be able to measure when connected
	virtual bool shouldMeasure() const;
	/// Returns true. The grating should be able to move when connected
	virtual bool shouldMove() const;
	/// Returns false. A grating selection cannot be stopped mid move.
	virtual bool shouldStop() const;

	/// Returns true if the grating can be determined right now.
	virtual bool canMeasure() const;
	/// Returns true if the grating can be moved right now.
	virtual bool canMove() const;
	/// Returns false. The grating move can never be stopped.
	virtual bool canStop() const;

	/// Returns true if the given value is a valid grating feedback value (ie Low, Medium, High, or Unknown)
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid grating setpoint value (ie Low, Medium, or High only)
	virtual bool validSetpoint(double value) const;

signals:

public slots:

protected slots:
	virtual void updateConnected();

	virtual void updateMoving();

protected:

	virtual AMAction3* createMoveAction(double setpoint);

	virtual int currentIndex() const;

	AMPVwStatusControl *gratingSelectionPVControl_;
};

#endif // PGMMONOGRATINGSELECTIONCONTROL_H
