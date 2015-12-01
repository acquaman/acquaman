#include "SGMVATValve.h"
#include "source/actions3/AMListAction3.h"
#include "source/actions3/AMActionSupport.h"

SGMVATValve::SGMVATValve(const QString &deviceName, const QString &baseName, QObject *parent) :
	AMSingleEnumeratedControl(deviceName, "", parent)
{
	// Setup value options.

	addValueOption(Closed, "Closed", SGMVATVALVE_CLOSED_SETPOINT);
	addValueOption(Open, "Open", SGMVATVALVE_OPEN_SETPOINT, SGMVATVALVE_OPEN_MIN, SGMVATVALVE_OPEN_MAX);

	// Setup controls.

	setBaseControl(new AMPVControl(name(), baseName+":position", baseName+":ctrl:posn", "", this));
	speed_ = new AMSinglePVControl(name(), baseName+":valveSpeed", this);
}

SGMVATValve::~SGMVATValve()
{

}
