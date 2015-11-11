#include "SGMBypassLadder.h"

SGMBypassLadder::SGMBypassLadder(const QString &name, const QString &baseName, QObject *parent) :
	SGMLadderControl(name, baseName, parent)
{
	addLadderOption("Slit", SGMBYPASSLADDER_SLIT_SETPOINT, SGMBYPASSLADDER_SLIT_MIN, SGMBYPASSLADDER_SLIT_MAX);
	addLadderOption("Passthrough", SGMBYPASSLADDER_PASSTHROUGH_SETPOINT, SGMBYPASSLADDER_PASSTHROUGH_MIN, SGMBYPASSLADDER_PASSTHROUGH_MAX);
	addLadderOption("Phosphor", SGMBYPASSLADDER_SLIT_SETPOINT, SGMBYPASSLADDER_SLIT_MIN, SGMBYPASSLADDER_SLIT_MAX);
}

SGMBypassLadder::~SGMBypassLadder()
{

}
