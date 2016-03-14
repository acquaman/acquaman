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

	outputStateControl_ = new AMReadOnlyPVControl(
				QString("LogicBlockOutput"),
				QString("%1_OUT").arg(baseName),
				this);

	allControls_->addControl(outputStateControl_);

	// Make connections.

	connect( allControls_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( outputStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputStateValueChanged()) );
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

BioXASZebraLogicBlockInput* BioXASZebraLogicBlock::inputControlAt(int index) const
{
	BioXASZebraLogicBlockInput *result = 0;

	if (index >= 0 && index < inputControls_.count())
		result = inputControls_.at(index);

	return result;
}

void BioXASZebraLogicBlock::setInputValue(int index, int newValue)
{
	BioXASZebraInput *input = inputControlAt(index);

	if (input)
		input->setInputValue(newValue);
}

void BioXASZebraLogicBlock::setInputValuePreference(int index, int newPreference)
{
	BioXASZebraInput *input = inputControlAt(index);

	if (input)
		input->setValuePreference(newPreference);
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

void BioXASZebraLogicBlock::onOutputStateValueChanged()
{
	emit outputStateChanged(outputStateValue());
	emit outputStateHighChanged(isOutputStateHigh());
}
