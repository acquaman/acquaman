#include "BioXASZebraLogicBlockInput.h"

BioXASZebraLogicBlockInput::BioXASZebraLogicBlockInput(const QString &name, const QString &baseName, int inputIndex, QObject *parent) :
	BioXASZebraInput(name, QString("%1_INP%2").arg(baseName).arg(inputIndex), parent)
{
	enabledStateControl_ = new AMSinglePVControl(
				QString("InputControlEnabled"),
				QString("%1_ENA:B%2").arg(baseName).arg(inputIndex - 1),
				this,
				1.0);

	invertedStateControl_ = new AMSinglePVControl(
				QString("InputControlInverted"),
				QString("%1_INV:B%2").arg(baseName).arg(inputIndex - 1),
				this,
				1.0);

	allControls_->addControl(enabledStateControl_);
	allControls_->addControl(invertedStateControl_);

	connect( enabledStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onEnabledStateValueChanged()) );
	connect( invertedStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onInvertedStateValueChanged()) );
}

BioXASZebraLogicBlockInput::~BioXASZebraLogicBlockInput()
{

}

double BioXASZebraLogicBlockInput::enabledState() const
{
	return enabledStateControl_->value();
}

double BioXASZebraLogicBlockInput::invertedState() const
{
	return invertedStateControl_->value();
}

bool BioXASZebraLogicBlockInput::isEnabled() const
{
	return (int(enabledStateControl_->value()) == Enabled);
}

bool BioXASZebraLogicBlockInput::isInverted() const
{
	return (int(invertedStateControl_->value()) == Inverted);
}

void BioXASZebraLogicBlockInput::setEnabled(bool isEnabled)
{
	if (isEnabled && !enabledStateControl_->withinTolerance(Enabled))
		enabledStateControl_->move(Enabled);
	else if (!isEnabled && !enabledStateControl_->withinTolerance(NotEnabled))
		enabledStateControl_->move(NotEnabled);
}

void BioXASZebraLogicBlockInput::setInverted(bool isInverted)
{
	if (isInverted && !invertedStateControl_->withinTolerance(Inverted))
		invertedStateControl_->move(Inverted);
	else if (!isInverted && !invertedStateControl_->withinTolerance(NotInverted))
		invertedStateControl_->move(NotInverted);
}

void BioXASZebraLogicBlockInput::onEnabledStateValueChanged()
{
	emit enabledStateChanged(enabledState());
}

void BioXASZebraLogicBlockInput::onInvertedStateValueChanged()
{
	emit invertedStateChanged(invertedState());
}
