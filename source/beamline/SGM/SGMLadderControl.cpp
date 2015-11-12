#include "SGMLadderControl.h"

SGMLadderControl::SGMLadderControl(const QString &name, const QString &baseName, QObject *parent) :
	AMSingleEnumeratedControl(name, "steps", parent)
{
	setContextKnownDescription("XPS Ladder");
	setAllowsMovesWhileMoving(false);

	setBaseControl(new AMPVControl(baseName, baseName+":steps:fbk", baseName+":steps", baseName+":emergStop", this));
}

SGMLadderControl::~SGMLadderControl()
{

}
