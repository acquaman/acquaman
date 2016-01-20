#include "BioXASZebraAndControl.h"

BioXASZebraAndControl::BioXASZebraAndControl(const QString &baseName, int blockIndex, QObject *parent) :
    QObject(parent)
{
	name_ = QString("AND Control %1").arg(blockIndex);

	connected_ = false;

	// Create input controls.

	allControls_ = new AMControlSet(this);

	for (int i = 0; i < BIOXASZEBRAANDCONTROL_INPUT_CONTROL_NUM; i++) {
		BioXASZebraAndInputControl *inputControl = new BioXASZebraAndInputControl(
					QString("%1:AND%2_INP%3").arg(baseName).arg(blockIndex).arg(i),
					this);

//		allControls_->addControl(inputControl);
		inputControls_.append(inputControl);
	}

	// Create output control.

	outputStatusControl_ = new AMReadOnlyPVControl(
				QString("ANDControl%1OutputPulse").arg(blockIndex),
				QString("%1:AND%2_OUT").arg(baseName).arg(blockIndex),
				this);

	allControls_->addControl(outputStatusControl_);

	// Make connections.

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(outputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputValueStatusChanged()));
}

BioXASZebraAndControl::~BioXASZebraAndControl()
{

}

bool BioXASZebraAndControl::outputStatus() const
{
	return (outputStatusControl_->value() == On);
}

void BioXASZebraAndControl::onControlSetConnectedChanged(bool connected)
{
	if (connected_ != connected) {
		connected_ = connected;
		emit connectedChanged(connected_);
	}
}

void BioXASZebraAndControl::onOutputStatusValueChanged()
{
	emit outputStatusChanged(outputStatus());
}


