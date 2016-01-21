#include "BioXASZebraLogicBlockInput.h"

BioXASZebraLogicBlockInput::BioXASZebraLogicBlockInput(const QString &name, const QString &baseName, int inputIndex, QObject *parent) :
	BioXASZebraInput(name, QString("%1_INP%2").arg(baseName).arg(inputIndex), parent)
{
	enabledStatusControl_ = new AMSinglePVControl(
				QString("InputControlEnabled"),
				QString("%1_ENA:B%2").arg(baseName).arg(inputIndex - 1),
				this,
				1.0);

	invertedStatusControl_ = new AMSinglePVControl(
				QString("InputControlInverted"),
				QString("%1_INV:B%2").arg(baseName).arg(inputIndex - 1),
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
	if (isEnabled && !enabledStatusControl_->withinTolerance(Enabled))
		enabledStatusControl_->move(Enabled);
	else if (!isEnabled && !enabledStatusControl_->withinTolerance(NotEnabled))
		enabledStatusControl_->move(NotEnabled);
}

void BioXASZebraLogicBlockInput::setInverted(bool isInverted)
{
	if (isInverted && !invertedStatusControl_->withinTolerance(Inverted))
		invertedStatusControl_->move(Inverted);
	else if (!isInverted && !invertedStatusControl_->withinTolerance(NotInverted))
		invertedStatusControl_->move(NotInverted);
}

void BioXASZebraLogicBlockInput::onEnabledStatusValueChanged()
{
	emit enabledStatusChanged(enabledStatus());
}

void BioXASZebraLogicBlockInput::onInvertedStatusValueChanged()
{
	emit invertedStatusChanged(invertedStatus());
}
