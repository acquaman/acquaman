#include "BioXASZebraLogicBlock.h"

BioXASZebraLogicBlock::BioXASZebraLogicBlock(const QString &name, const QString &baseName, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;

	allControls_ = new AMControlSet(this);

	// Create input controls.

	for (int i = 1; i <= BIOXASZEBRALOGICBLOCK_INPUT_NUM; i++) {
		BioXASZebraLogicBlockInput *inputControl = new BioXASZebraLogicBlockInput(
					QString("LogicBlockInput%1").arg(i),
					baseName,
					i,
					this);

		inputControls_ << inputControl;
		allControls_->addControl(inputControl);
	}

	// Create output control.

	outputStatusControl_ = new AMReadOnlyPVControl(
				QString("LogicBlockOutput"),
				QString("%1_OUT").arg(baseName),
				this);

	allControls_->addControl(outputStatusControl_);

	// Make connections.

	connect( allControls_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( outputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputStatusValueChanged()) );
}

BioXASZebraLogicBlock::~BioXASZebraLogicBlock()
{

}

bool BioXASZebraLogicBlock::outputStatus() const
{
	bool result = false;

	if (outputStatusControl_ && outputStatusControl_->canMeasure())
		result = (outputStatusControl_->value() == On);

	return result;
}

void BioXASZebraLogicBlock::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void BioXASZebraLogicBlock::updateConnected()
{
	setConnected(allControls_->isConnected());
}

void BioXASZebraLogicBlock::onOutputStatusValueChanged()
{
	emit outputStatusChanged(outputStatus());
}
