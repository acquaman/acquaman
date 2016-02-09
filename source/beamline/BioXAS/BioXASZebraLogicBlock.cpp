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

#include <QDebug>
#include <QTime>
void BioXASZebraLogicBlock::onOutputStateValueChanged()
{
	qDebug() << "\n\n" << name() << "output state changed." << QTime::currentTime();

	emit outputStateChanged(outputStateValue());
	emit outputStateHighChanged(isOutputStateHigh());
}
