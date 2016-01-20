#include "BioXASZebraOrBlock.h"

BioXASZebraOrBlock::BioXASZebraOrBlock(const QString &name, const QString &baseName, int blockIndex, QObject *parent) :
	BioXASZebraLogicBlock(name, parent)
{
	// Create and set input controls.

	AMControlSet *inputControls = new AMControlSet(this);

	for (int i = 0; i < BIOXASZEBRAORBLOCK_INPUT_NUM; i++) {
		BioXASZebraLogicBlockInput *inputControl = new BioXASZebraLogicBlockInput(
					QString("ORBlock%1Input%2").arg(blockIndex).arg(i),
					QString("%1:OR%2_INP%3").arg(baseName).arg(blockIndex).arg(i),
					this);

		inputControls->addControl(inputControl);
	}

	setInputControlsSet(inputControls);

	// Create and set output control.

	AMReadOnlyPVControl *outputStatusControl = new AMReadOnlyPVControl(
				QString("ORBlock%1Output").arg(blockIndex),
				QString("%1:OR%2_OUT").arg(baseName).arg(blockIndex),
				this);

	setOutputStatusControl(outputStatusControl);
}

BioXASZebraOrBlock::~BioXASZebraOrBlock()
{

}
