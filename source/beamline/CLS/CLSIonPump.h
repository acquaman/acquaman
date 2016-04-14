#ifndef CLSIONPUMP_H
#define CLSIONPUMP_H

#include "beamline/CLS/CLSBeamlineControl.h"

class CLSIonPump : public CLSBeamlineControl
{
	Q_OBJECT

public:
	/// Constructor.
	CLSIonPump(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSIonPump();
};

#endif // CLSIONPUMP_H
