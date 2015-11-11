#include "SGMXASLadder.h"

SGMXASLadder::SGMXASLadder(const QString &name, const QString &baseName, QObject *parent) :
	SGMLadderControl(name, baseName, parent)
{
	addLadderOption("Vanadium filter", SGMXASLADDER_VANADIUM_SETPOINT, SGMXASLADDER_VANADIUM_MIN, SGMXASLADDER_VANADIUM_MAX);
	addLadderOption("Photodiode", SGMXASLADDER_PHOTODIODE_SETPOINT, SGMXASLADDER_PHOTODIODE_MIN, SGMXASLADDER_PHOTODIODE_MAX);
	addLadderOption("Au mesh", SGMXASLADDER_MESH_SETPOINT, SGMXASLADDER_MESH_MIN, SGMXASLADDER_MESH_MAX);
	addLadderOption("Pinhole", SGMXASLADDER_PINHOLE_SETPOINT, SGMXASLADDER_PINHOLE_MIN, SGMXASLADDER_PINHOLE_MAX);
	addLadderOption("Passthrough", SGMXASLADDER_PASSTHROUGH_SETPOINT, SGMXASLADDER_PASSTHROUGH_MIN, SGMXASLADDER_PASSTHROUGH_MAX);
	addLadderOption("Phosphor", SGMXASLADDER_PHOSPHOR_SETPOINT, SGMXASLADDER_PHOSPHOR_MIN, SGMXASLADDER_PHOSPHOR_MAX);
}

SGMXASLadder::~SGMXASLadder()
{

}
