#include "BioXASZebraOutputControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraOutputControl::BioXASZebraOutputControl(const QString &name, const QString &baseName, QObject *parent) :
	AMControl(name, "", parent)
{
	name_ = name;

	allControls_ = new AMControlSet(this);
	connect( allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged()) );

	outputValueControl_ = new AMSinglePVControl(QString(baseName), QString(baseName), this, 0.1);
	connect( outputValueControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputValueChanged()) );
	allControls_->addControl(outputValueControl_);

	outputStatusControl_ = new AMReadOnlyPVControl(QString("%1:%2").arg(baseName).arg("STA"), QString("%1:%2").arg(baseName).arg("STA"), this);
	connect( outputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputStatusChanged()) );
	allControls_->addControl(outputStatusControl_);

	outputValuePreferenceSet_ = false;
	outputValuePreference_ = 0;
}

BioXASZebraOutputControl::~BioXASZebraOutputControl()
{

}

bool BioXASZebraOutputControl::isConnected() const
{
	return allControls_->isConnected();
}

int BioXASZebraOutputControl::outputValue() const
{
	return int(outputValueControl_->value());
}

QString BioXASZebraOutputControl::outputValueString() const
{
	return BioXASZebraCommand::nameFromCommand(outputValue());
}

double BioXASZebraOutputControl::outputStatusValue() const
{
	return outputStatusControl_->value();
}

bool BioXASZebraOutputControl::isOutputStateHigh() const
{
	return (int(outputStatusControl()->value()) == 1);
}

void BioXASZebraOutputControl::setOutputValue(int newValue)
{
	if (!outputValueControl_->withinTolerance(double(newValue)))
		outputValueControl_->move(double(newValue));
}

void BioXASZebraOutputControl::setOutputValuePreference(int newValue)
{
	if (outputValuePreference_ != newValue || !outputValuePreferenceSet_) {
		outputValuePreferenceSet_ = true;
		outputValuePreference_ = newValue;
		updateOutputValueControl();

		emit outputValuePreferenceChanged(outputValuePreference_);
	}
}

void BioXASZebraOutputControl::onControlSetConnectedChanged()
{
	updateOutputValueControl();

	emit connectedChanged(allControls_->isConnected());
}

void BioXASZebraOutputControl::onOutputValueChanged()
{
	emit outputValueChanged(outputValue());
	emit outputValueStringChanged(outputValueString());
}

void BioXASZebraOutputControl::onOutputStatusChanged()
{
	emit outputStatusChanged(outputStatusValue());
}

void BioXASZebraOutputControl::updateOutputValueControl()
{
	if (outputValuePreferenceSet_)
		setOutputValue(outputValuePreference_);
}

