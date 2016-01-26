#ifndef BIOXASENDSTATIONSHUTTER_H
#define BIOXASENDSTATIONSHUTTER_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class BioXASEndstationShutter : public AMExclusiveStatesEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of possible values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	explicit BioXASEndstationShutter(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASEndstationShutter();
};

#endif // BIOXASENDSTATIONSHUTTER_H
