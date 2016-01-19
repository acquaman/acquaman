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

	case MSeconds:
		unitsString = "ms";
		break;

	case Seconds:
		unitsString = "s";
		break;

	case DSeconds:
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

bool BioXASZebraPulseControl::validPulseWidth(double pulseWidth)
{
	bool result = false;

	if (pulseWidth >= BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MIN && pulseWidth <= BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MAX)
		result = true;

	return result;
}

double BioXASZebraPulseControl::convertPulseWidth(double pulseWidth, double pulseWidthUnits, double desiredUnits)
{
	double result = pulseWidth;

	if (pulseWidthUnits != desiredUnits) {

		// Convert pulse width to seconds.

		double pulseWidthSeconds = 0;

		if (pulseWidthUnits == MSeconds) // from ms
			pulseWidthSeconds = pulseWidth / 1000.0;
		else if (pulseWidthUnits == Seconds) // from s
			pulseWidthSeconds = pulseWidth;
		else if (pulseWidthUnits == DSeconds) // from 10s
			pulseWidthSeconds = pulseWidth * 10.0;

		// Complete conversion to the desired units.

		if (desiredUnits == MSeconds) // to ms
			result = pulseWidthSeconds * 1000.0;
		else if (desiredUnits == Seconds) // to s
			result = pulseWidthSeconds;
		else if (desiredUnits == DSeconds) // to 10s
			result = pulseWidthSeconds / 10.0;
	}

	return result;
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
	if (!pulseWidthControl_->withinTolerance(value))
		pulseWidthControl_->move(value);
}

void BioXASZebraPulseControl::setTimeUnitsValue(int value)
{
	if (!timeUnitsControl_->withinTolerance(double(value)))
		timeUnitsControl_->move(double(value));
}

bool BioXASZebraPulseControl::setPulseWidth(double pulseWidth, double timeUnits)
{
	bool result = false;

	double setpoint = pulseWidth;
	double units = timeUnits;

	// Check to see if the pulse width setpoint/units combo is valid.
	// If not, attempt to convert them.

	bool setpointValid = validPulseWidth(pulseWidth);

	if (!setpointValid) {

		double newSetpoint;
		int newUnits = MSeconds;
		int newUnitsMax = DSeconds;

		// Iterate through available time units, checking to see
		// if different units will make the pulse width valid.

		for (newUnits; newUnits <= newUnitsMax && !setpointValid; newUnits++) {
			newSetpoint = convertPulseWidth(setpoint, units, newUnits);
			setpointValid = validPulseWidth(newSetpoint);
		}

		if (setpointValid) {
			setpoint = newSetpoint;
			units = newUnits;
		}
	}

	// If the setpoint is valid, apply setpoint to the pulse
	// width control and the units to the time units control.

	if (setpointValid) {
		setTimeUnitsValue(units); // Units should be changed first, to prevent errors related to setting an invalid pulse width for the current units.
		setPulseWidthValue(setpoint);

		result = true;
	}

	return result;
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

