#include "SGMBypassLadder.h"

SGMBypassLadder::SGMBypassLadder(const QString &name, const QString &baseName, QObject *parent) :
	SGMLadderControl(name, baseName, parent)
{
	addValueOption(Slit, "Slit", SGMBYPASSLADDER_SLIT_SETPOINT, SGMBYPASSLADDER_SLIT_MIN, SGMBYPASSLADDER_SLIT_MAX);
	addValueOption(Passthrough, "Passthrough", SGMBYPASSLADDER_PASSTHROUGH_SETPOINT, SGMBYPASSLADDER_PASSTHROUGH_MIN, SGMBYPASSLADDER_PASSTHROUGH_MAX);
	addValueOption(Phosphor, "Phosphor", SGMBYPASSLADDER_PHOSPHOR_SETPOINT, SGMBYPASSLADDER_PHOSPHOR_MIN, SGMBYPASSLADDER_PHOSPHOR_MAX);
}

SGMBypassLadder::~SGMBypassLadder()
{

}
