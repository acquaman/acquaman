#include "SGMXPSLadder.h"

SGMXPSLadder::SGMXPSLadder(const QString &name, const QString &baseName, QObject *parent) :
	SGMLadderControl(name, baseName, parent)
{
	addValueOption(Photodiode, "Photodiode", SGMXPSLADDER_PHOTODIODE_SETPOINT, SGMXPSLADDER_PHOTODIODE_MIN, SGMXPSLADDER_PHOTODIODE_MAX);
	addValueOption(Vanadium, "Vanadium filter", SGMXPSLADDER_VANADIUM_SETPOINT, SGMXPSLADDER_VANADIUM_MIN, SGMXPSLADDER_VANADIUM_MAX);
	addValueOption(AuMesh, "Au mesh", SGMXPSLADDER_MESH_SETPOINT, SGMXPSLADDER_MESH_MIN, SGMXPSLADDER_MESH_MAX);
	addValueOption(Retracted, "Retracted", SGMXPSLADDER_RETRACTED_SETPOINT, SGMXPSLADDER_RETRACTED_MIN, SGMXPSLADDER_RETRACTED_MAX);
}

SGMXPSLadder::~SGMXPSLadder()
{

}
