#ifndef CLSIONPUMP_H
#define CLSIONPUMP_H

#include "beamline/AMBeamlineControl.h"

class CLSIonPump : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSIonPump(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSIonPump();
};

#endif // CLSIONPUMP_H
