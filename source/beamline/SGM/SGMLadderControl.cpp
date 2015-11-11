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

void SGMLadderControl::addLadderOption(const QString &optionName, double setpoint, double min, dobule max)
{
	addValueOption(indices_.count(), optionName, setpoint, min, max);
}

void SGMLadderControl::removeLadderOption(const QString &optionName)
{
	QList<int> indices = indicesNamed(optionName);

	foreach (int index, indices)
		removeOption(index);
}

void SGMLadderControl::clearLadderOptions()
{
	clearOptions();
}
