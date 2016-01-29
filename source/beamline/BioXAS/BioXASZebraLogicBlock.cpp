#include "BioXASZebraLogicBlock.h"

BioXASZebraLogicBlock::BioXASZebraLogicBlock(const QString &name, const QString &baseName, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	// Create input controls.

	for (int i = 1; i <= BIOXASZEBRALOGICBLOCK_INPUT_NUM; i++) {
		BioXASZebraLogicBlockInput *inputControl = new BioXASZebraLogicBlockInput(
					QString("LogicBlockInput%1").arg(i),
					baseName,
					i,
					this);

		inputControls_ << inputControl;
		addChildControl(inputControl);
	}

	// Create output control.

	outputStateControl_ = new AMReadOnlyPVControl(
				QString("LogicBlockOutput"),
				QString("%1_OUT").arg(baseName),
				this);

	connect( outputStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputStateValueChanged()) );
	addChildControl(outputStateControl_);
}

BioXASZebraLogicBlock::~BioXASZebraLogicBlock()
{

}

double BioXASZebraLogicBlock::outputStateValue() const
{
	return outputStateControl_->value();
}

bool BioXASZebraLogicBlock::isOutputStateHigh() const
{
	return (outputStateControl_->value() == High);
}

void BioXASZebraLogicBlock::onOutputStateValueChanged()
{
	emit outputStateChanged(outputStateValue());
	emit outputStateHighChanged(isOutputStateHigh());
}
