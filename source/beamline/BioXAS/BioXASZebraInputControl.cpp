#include "BioXASZebraInputControl.h"

#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraInputControl::BioXASZebraInputControl(const QString &baseName, QObject *parent) :
    QObject(parent)
{
	name_ = QString("Input Control");

	valueControl_ = new AMSinglePVControl(
				QString("InputControlValue"),
				QString("%1").arg(baseName),
				this,
				1.0);

	statusControl_ = new AMReadOnlyPVControl(
				QString("InputControlStatus"),
				QString("%1:STA").arg(baseName),
				this);

	connected_ = false;

	allControls_ = new AMControlSet(this);
	allControls_->addControl(valueControl_);
	allControls_->addControl(statusControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(valueControl_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusChanged()));
}

BioXASZebraInputControl::~BioXASZebraInputControl()
{

}

int BioXASZebraInputControl::value() const
{
	return int(valueControl_->value());
}

QString BioXASZebraInputControl::valueString() const
{
	return BioXASZebraCommand::nameFromCommand(value());
}

bool BioXASZebraInputControl::status() const
{
	return statusControl_->value() == 1;
}

void BioXASZebraInputControl::setValue(int value)
{
	if (!valueControl_->withinTolerance(double(value)))
		valueControl_->move(double(value));
}

void BioXASZebraInputControl::onControlSetConnectedChanged(bool connected)
{
	if (connected_ != connected){
		connected_ = connected;
		emit connectedChanged(connected_);
	}
}

void BioXASZebraInputControl::onValueChanged()
{
	emit valueChanged(value());
	emit valueStringChanged(valueString());
}

void BioXASZebraInputControl::onStatusChanged()
{
	emit statusChanged(status());
}

