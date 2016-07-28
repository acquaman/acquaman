#ifndef PGMENERGYCONTROL_H
#define PGMENERGYCONTROL_H

#include "beamline/AMPVControl.h"

class PGMEnergyControl : public AMPVwStatusControl
{
	Q_OBJECT

public:
	/// Constructor.
	PGMEnergyControl(const QString& name,
					 const QString& readPVname,
					 const QString& writePVname,
					 const QString& movingPVname,
					 const QString& stopPVname = QString(),
					 QObject* parent = 0,
					 double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
					 double moveStartTimeoutSeconds = 2.0,
					 AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
					 int stopValue = 1,
					 const QString &description = "");
	/// Destructor.
	virtual ~PGMEnergyControl();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns true if this control can currently be measured, false otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can currently be moved, false otherwise.
	virtual bool canMove() const;
};

#endif // PGMENERGYCONTROL_H
