#include "BioXASZebraLogicBlockInput.h"

BioXASZebraLogicBlockInput::BioXASZebraLogicBlockInput(const QString &name, const QString &baseName, QObject *parent) :
	BioXASZebraInput(name, baseName, parent)
{
	enabledStatusControl_ = new AMSinglePVControl(
				QString("InputControlEnabled"),
				QString("%1_ENA:B0").arg(baseName),
				this,
				1.0);

	invertedStatusControl_ = new AMSinglePVControl(
				QString("InputControlInverted"),
				QString("%1_INV:B0").arg(baseName),
				this,
				1.0);

	allControls_->addControl(enabledStatusControl_);
	allControls_->addControl(invertedStatusControl_);

	connect( enabledStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onEnabledStatusValueChanged()) );
	connect( invertedStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onInvertedStatusValueChanged()) );
}

BioXASZebraLogicBlockInput::~BioXASZebraLogicBlockInput()
{

}

bool BioXASZebraLogicBlockInput::enabledStatus() const
{
	return (int(enabledStatusControl_->value()) == Enabled);
}

bool BioXASZebraLogicBlockInput::invertedStatus() const
{
	return (int(invertedStatusControl_->value()) == Inverted);
}

void BioXASZebraLogicBlockInput::setEnabled(bool isEnabled)
{
	if (!enabledStatusControl_->withinTolerance(isEnabled))
		enabledStatusControl_->move(isEnabled);
}

void BioXASZebraLogicBlockInput::setInverted(bool isInverted)
{
	if (!invertedStatusControl_->withinTolerance(isInverted))
		invertedStatusControl_->move(isInverted);
}

void BioXASZebraLogicBlockInput::onEnabledStatusValueChanged()
{
	emit enabledStatusChanged(enabledStatus());
}

void BioXASZebraLogicBlockInput::onInvertedStatusValueChanged()
{
	emit invertedStatusChanged(invertedStatus());
}
