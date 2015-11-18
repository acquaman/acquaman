#ifndef SGMBYPASSLADDER_H
#define SGMBYPASSLADDER_H

#include "beamline/SGM/SGMLadderControl.h"

#define SGMBYPASSLADDER_SLIT_SETPOINT 1260
#define SGMBYPASSLADDER_SLIT_MIN 0
#define SGMBYPASSLADDER_SLIT_MAX 1260

#define SGMBYPASSLADDER_PASSTHROUGH_SETPOINT 12000
#define SGMBYPASSLADDER_PASSTHROUGH_MIN 12000
#define SGMBYPASSLADDER_PASSTHROUGH_MAX 12000

#define SGMBYPASSLADDER_PHOSPHOR_SETPOINT 27000
#define SGMBYPASSLADDER_PHOSPHOR_MIN 27000
#define SGMBYPASSLADDER_PHOSPHOR_MAX 27000

class SGMBypassLadder : public SGMLadderControl
{
	Q_OBJECT

public:
	/// Enum defining the different value options.
	enum Option { Slit = 0, Passthrough = 1, Phosphor = 2 };

	/// Constructor.
	explicit SGMBypassLadder(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMBypassLadder();
};

#endif // SGMBYPASSLADDER_H
