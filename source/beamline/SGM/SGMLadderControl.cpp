#include "SGMLadderControl.h"

SGMLadderControl::SGMLadderControl(const QString &name, const QString &baseName, QObject *parent) :
	AMSingleEnumeratedControl(name, "steps", parent)
{
	setAllowsMovesWhileMoving(false);

	setBaseControl(new AMPVControl(baseName, baseName+":step:fbk", baseName+":step", baseName+":emergStop", this));
}

SGMLadderControl::~SGMLadderControl()
{

}
