#include "BioXASStripTool.h"

BioXASStripTool::BioXASStripTool(QObject *parent) :
	AMStripTool(parent)
{
	addItem("PCT1402-01:mA:fbk");
}

BioXASStripTool::~BioXASStripTool()
{

}
