#include "BioXASZebraAndControl.h"

BioXASZebraAndControl::BioXASZebraAndControl(const QString &name, const QString &baseName, int blockIndex, QObject *parent) :
	BioXASZebraLogicBlockControl(name, parent)
{
	// Create and set input controls.

	AMControlSet *inputControls = new AMControlSet(this);

	for (int i = 0; i < BIOXASZEBRAANDCONTROL_INPUT_CONTROL_NUM; i++) {
		BioXASZebraLogicBlockInputControl *inputControl = new BioXASZebraLogicBlockInputControl(
					QString("And Input %1").arg(i),
					QString("%1:AND%2_INP%3").arg(baseName).arg(blockIndex).arg(i),
					this);

		inputControls->addControl(inputControl);
	}

	setInputControlsSet(inputControls);

	// Create and set output control.

	AMReadOnlyPVControl *outputStatusControl = new AMReadOnlyPVControl(
				QString("ANDControl%1OutputPulse").arg(blockIndex),
				QString("%1:AND%2_OUT").arg(baseName).arg(blockIndex),
				this);

	setOutputStatusControl(outputStatusControl);
}

BioXASZebraAndControl::~BioXASZebraAndControl()
{

}

