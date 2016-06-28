#ifndef BIOXASWIGGLERGAPSTATUS_H
#define BIOXASWIGGLERGAPSTATUS_H

#include "beamline/AMSingleEnumeratedControl.h"

#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MIN 0
#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MAX 11.78
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MIN 11.79
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MAX 50

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
};

#endif // BIOXASWIGGLERGAPSTATUS_H
