#include "BioXASZebraInput.h"

#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraInput::BioXASZebraInput(const QString &name, const QString &baseName, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;

	valueControl_ = new AMSinglePVControl(
				QString("InputControlValue"),
				QString("%1").arg(baseName),
				this,
				1.0);

	statusControl_ = new AMReadOnlyPVControl(
				QString("InputControlStatus"),
				QString("%1:STA").arg(baseName),
				this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(valueControl_);
	allControls_->addControl(statusControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(valueControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueChanged()));
	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputStatusChanged()));
}

BioXASZebraInput::~BioXASZebraInput()
{

}

int BioXASZebraInput::inputValue() const
{
	return int(valueControl_->value());
}

QString BioXASZebraInput::inputValueString() const
{
	return BioXASZebraCommand::nameFromCommand(inputValue());
}

bool BioXASZebraInput::inputStatus() const
{
	return statusControl_->value() == 1;
}

void BioXASZebraInput::setInputValue(int value)
{
	if (!valueControl_->withinTolerance(double(value)))
		valueControl_->move(double(value));
}

void BioXASZebraInput::onControlSetConnectedChanged(bool connected)
{
	if (connected_ != connected){
		connected_ = connected;
		emit connectedChanged(connected_);
	}
}

void BioXASZebraInput::onInputValueChanged()
{
	emit inputValueChanged(inputValue());
	emit inputValueStringChanged(inputValueString());
}

void BioXASZebraInput::onInputStatusChanged()
{
	emit inputStatusChanged(inputStatus());
}

