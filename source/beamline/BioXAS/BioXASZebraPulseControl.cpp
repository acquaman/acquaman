#include "BioXASZebraPulseControl.h"

#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraPulseControl::BioXASZebraPulseControl(const QString &baseName, int pulseIndex, QObject *parent)
	: QObject(parent)
{
	name_ = QString("Pulse Control %1").arg(pulseIndex);

	inputControl_ = new AMSinglePVControl(QString("PulseControl%1Input").arg(pulseIndex),
					      QString("%1:PULSE%2_INP").arg(baseName).arg(pulseIndex),
					      this,
					      1.0);

	inputStatusControl_ = new AMReadOnlyPVControl(QString("PulseControl%1InputStatus").arg(pulseIndex),
						      QString("%1:PULSE%2_INP:STA").arg(baseName).arg(pulseIndex),
						      this);

	edgeTriggerControl_ = new AMSinglePVControl(QString("PulseControl%1EdgeTrigger").arg(pulseIndex),
						    QString("%1:POLARITY:B%2").arg(baseName).arg(letterFromPulseIndex(pulseIndex)),
						    this,
						    0.5);

	delayBeforeControl_ = new AMSinglePVControl(QString("PulseControl%1DelayBefore").arg(pulseIndex),
						    QString("%1:PULSE%2_DLY").arg(baseName).arg(pulseIndex),
						    this,
						    0.0001);

	pulseWidthControl_ = new AMSinglePVControl(QString("PulseControl%1Width").arg(pulseIndex),
						   QString("%1:PULSE%2_WID").arg(baseName).arg(pulseIndex),
						   this,
						   0.0001);

	timeUnitsControl_ = new AMSinglePVControl(QString("PulseControl%1TimeUnits").arg(pulseIndex),
						  QString("%1:PULSE%2_PRE").arg(baseName).arg(pulseIndex),
						  this,
						  0.5);

	triggerWhileActiveControl_ = new AMReadOnlyPVControl(QString("PulseControl%1TriggerActive").arg(pulseIndex),
							     QString("%1:SYS_STATERR.B%2").arg(baseName).arg(pulseIndex-1),
							     this);

	outputPulseControl_ = new AMReadOnlyPVControl(QString("PulseControl%1OutputPulse").arg(pulseIndex),
						      QString("%1:PULSE%2_OUT").arg(baseName).arg(pulseIndex),
						      this);

	connected_ = false;

	allControls_ = new AMControlSet(this);
	allControls_->addControl(inputControl_);
	allControls_->addControl(inputStatusControl_);
	allControls_->addControl(edgeTriggerControl_);
	allControls_->addControl(delayBeforeControl_);
	allControls_->addControl(pulseWidthControl_);
	allControls_->addControl(timeUnitsControl_);
	allControls_->addControl(triggerWhileActiveControl_);
	allControls_->addControl(outputPulseControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(inputControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueChanged()));
	connect(inputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueStatusChanged()));
	connect(edgeTriggerControl_, SIGNAL(valueChanged(double)), this, SLOT(onEdgeTriggerValueChanged()));
	connect(delayBeforeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDelayBeforeValueChanged()));
	connect(pulseWidthControl_, SIGNAL(valueChanged(double)), this, SLOT(onPulseWidthValueChanged()));
	connect(timeUnitsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTimeUnitsValueChanged()));
	connect(triggerWhileActiveControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerWhileActiveValueChanged()));
	connect(outputPulseControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputValueStatusChanged()));
}

BioXASZebraPulseControl::~BioXASZebraPulseControl()
{

}

QString BioXASZebraPulseControl::name() const
{
	return name_;
}

bool BioXASZebraPulseControl::isConnected() const
{
	return connected_;
}

int BioXASZebraPulseControl::inputValue() const
{
	return int(inputControl_->value());
}

QString BioXASZebraPulseControl::inputValueString() const
{
	return BioXASZebraCommand::nameFromCommand(inputValue());
}

bool BioXASZebraPulseControl::inputStatusValue() const
{
	return inputStatusControl_->value() == 1;
}

int BioXASZebraPulseControl::edgeTriggerValue() const
{
	return int(edgeTriggerControl_->value());
}

double BioXASZebraPulseControl::delayBeforeValue() const
{
	if (timeUnitsValueString() == "ms")
		return delayBeforeControl_->value()/1000;

	else if (timeUnitsValueString() == "s")
		return delayBeforeControl_->value();

	else if (timeUnitsValueString() == "10s")
		return delayBeforeControl_->value()*10;

	return 0;
}

double BioXASZebraPulseControl::pulseWidthValue() const
{
	if (timeUnitsValueString() == "ms")
		return pulseWidthControl_->value()/1000;

	else if (timeUnitsValueString() == "s")
		return pulseWidthControl_->value();

	else if (timeUnitsValueString() == "10s")
		return pulseWidthControl_->value()*10;

	return 0;
}

int BioXASZebraPulseControl::timeUnitsValue() const
{
	return int(timeUnitsControl_->value());
}

QString BioXASZebraPulseControl::timeUnitsValueString() const
{
	QString unitsString = "";

	switch(timeUnitsValue()){

	case 0:
		unitsString = "ms";
		break;

	case 1:
		unitsString = "s";
		break;

	case 2:
		unitsString = "10s";
		break;
	}

	return unitsString;
}

bool BioXASZebraPulseControl::triggerWhileActiveValue() const
{
	return int(triggerWhileActiveControl_->value()) == 1;
}

bool BioXASZebraPulseControl::outputValue() const
{
	return int(outputPulseControl_->value()) == 1;
}

void BioXASZebraPulseControl::setInputValue(int value)
{
	if (!inputControl_->withinTolerance(double(value)))
		inputControl_->move(double(value));
}

void BioXASZebraPulseControl::setEdgeTriggerValue(int value)
{
	if (!edgeTriggerControl_->withinTolerance(double(value)))
		edgeTriggerControl_->move(double(value));
}

void BioXASZebraPulseControl::setDelayBeforeValue(double value)
{
	if (!delayBeforeControl_->withinTolerance(value)){

		if (timeUnitsValueString() == "ms")
			delayBeforeControl_->move(value*1000);

		else if (timeUnitsValueString() == "s")
			delayBeforeControl_->move(value);

		else if (timeUnitsValueString() == "10s")
			delayBeforeControl_->move(value/10);
	}
}

void BioXASZebraPulseControl::setPulseWidthValue(double value)
{
	if (!pulseWidthControl_->withinTolerance(value)){

		if (timeUnitsValueString() == "ms")
			pulseWidthControl_->move(value*1000);

		else if (timeUnitsValueString() == "s")
			pulseWidthControl_->move(value);

		else if (timeUnitsValueString() == "10s")
			pulseWidthControl_->move(value/10);
	}
}

void BioXASZebraPulseControl::setTimeUnitsValue(int value)
{
	if (!timeUnitsControl_->withinTolerance(double(value)))
		timeUnitsControl_->move(double(value));
}

void BioXASZebraPulseControl::onControlSetConnectedChanged(bool connected)
{
	if (connected_ != connected){

		connected_ = connected;
		emit connectedChanged(connected_);
	}
}

void BioXASZebraPulseControl::onInputValueChanged()
{
	emit inputValueChanged(inputValue());
	emit inputValueStringChanged(inputValueString());
}

void BioXASZebraPulseControl::onInputValueStatusChanged()
{
	emit inputValueStatusChanged(inputStatusValue());
}

void BioXASZebraPulseControl::onEdgeTriggerValueChanged()
{
	emit edgeTriggerValueChanged(edgeTriggerValue());
}

void BioXASZebraPulseControl::onDelayBeforeValueChanged()
{
	emit delayBeforeValueChanged(delayBeforeValue());
}

void BioXASZebraPulseControl::onPulseWidthValueChanged()
{
	emit pulseWidthValueChanged(pulseWidthValue());
}

void BioXASZebraPulseControl::onTimeUnitsValueChanged()
{
	emit timeUnitsValueChanged(timeUnitsValue());
}

void BioXASZebraPulseControl::onTriggerWhileActiveValueChanged()
{
	emit triggerWhileActiveValueChanged(triggerWhileActiveValue());
}

void BioXASZebraPulseControl::onOutputValueStatusChanged()
{
	emit outputValueChanged(outputValue());
}

QString BioXASZebraPulseControl::letterFromPulseIndex(int index) const
{
	QString letter = "";

	switch(index){

	case 1:
		letter = "C";
		break;

	case 2:
		letter = "D";
		break;

	case 3:
		letter = "E";
		break;

	case 4:
		letter = "F";
		break;
	}

	return letter;
}

