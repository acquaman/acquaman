#include "BioXASZebraInput.h"

#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraInput::BioXASZebraInput(const QString &name, const QString &baseName, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;

	valuePreferenceSet_ = false;
	valuePreference_ = 0;

	valueControl_ = new AMSinglePVControl(
				QString("InputControlValue"),
				QString("%1").arg(baseName),
				this,
				1.0);

	stateControl_ = new AMReadOnlyPVControl(
				QString("InputControlStatus"),
				QString("%1:STA").arg(baseName),
				this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(valueControl_);
	allControls_->addControl(stateControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(valueControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueChanged()));
	connect(stateControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputStateChanged()));

	connect( valueControl_, SIGNAL(connected(bool)), this, SLOT(updateValueControl()) );
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

double BioXASZebraInput::inputStateValue() const
{
	return stateControl_->value();
}

bool BioXASZebraInput::isInputStateHigh() const
{
	return (stateControl_->value() == High);
}

void BioXASZebraInput::setInputValue(int value)
{
	qDebug() << "Attempting to set input value for" << name() << "to" << value;
	if (!valueControl_->withinTolerance(double(value))) {
		qDebug() << "Setting input value for" << name() << "to" << value;
		valueControl_->move(double(value));

	} else {
		qDebug() << "Cannot set input value for" << name() << ":" << value << "is within tolerance of" << valueControl_->value();
	}
}

void BioXASZebraInput::setValuePreference(int preferredValue)
{
	if (valuePreference_ != preferredValue) {
		qDebug() << "Setting input value preference for" << name() << "to" << preferredValue;
		valuePreferenceSet_ = true;
		valuePreference_ = preferredValue;

		emit valuePreferenceChanged(valuePreference_);
	}

	updateValueControl();
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

void BioXASZebraInput::onInputStateChanged()
{
	emit inputStateChanged(inputStateValue());
}

void BioXASZebraInput::updateValueControl()
{
	if (valuePreferenceSet_) {
		qDebug() << "Updating" << name() << "to have the correct input value preference.";
		setInputValue(valuePreference_);

	} else {
		qDebug() << "No input value preference set for" << name();
	}
}

