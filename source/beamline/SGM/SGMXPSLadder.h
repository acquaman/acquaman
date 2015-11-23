#ifndef SGMXPSLADDER_H
#define SGMXPSLADDER_H

#include "beamline/SGM/SGMLadderControl.h"

#define SGMXPSLADDER_PHOTODIODE_SETPOINT 1700
#define SGMXPSLADDER_PHOTODIODE_MIN 0
#define SGMXPSLADDER_PHOTODIODE_MAX 3400

#define SGMXPSLADDER_VANADIUM_SETPOINT 12000
#define SGMXPSLADDER_VANADIUM_MIN 10000
#define SGMXPSLADDER_VANADIUM_MAX 14000

#define SGMXPSLADDER_MESH_SETPOINT 21000
#define SGMXPSLADDER_MESH_MIN 18000
#define SGMXPSLADDER_MESH_MAX 24000

#define SGMXPSLADDER_RETRACTED_SETPOINT 27000
#define SGMXPSLADDER_RETRACTED_MIN 26000
#define SGMXPSLADDER_RETRACTED_MAX 40000

class SGMXPSLadder : public SGMLadderControl
{
	Q_OBJECT

public:
	/// Enum defining the different available filter options.
	enum Option { Photodiode = 0, Vanadium = 1, AuMesh = 2, Retracted = 3 };

	/// Constructor.
	explicit SGMXPSLadder(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMXPSLadder();
};

#endif // SGMXPSLADDER_H
