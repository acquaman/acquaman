#ifndef BIOXASWIGGLERGAPSTATUS_H
#define BIOXASWIGGLERGAPSTATUS_H

#include "beamline/AMSingleEnumeratedControl.h"

#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MIN 0.0
#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MAX 11.7
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MIN 11.8
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MAX 300.0

class BioXASWigglerGapStatus : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible status values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	BioXASWigglerGapStatus(const QString &name, const QString &baseName, QObject *parent);
	/// Destructor.
	virtual ~BioXASWigglerGapStatus();

	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return false; }
};

#endif // BIOXASWIGGLERGAPSTATUS_H
