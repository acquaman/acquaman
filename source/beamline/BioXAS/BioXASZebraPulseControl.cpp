#include "BioXASZebraPulseControl.h"

#include "actions3/AMActionSupport.h"
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

	edgeTriggerPreference_ = 0;

	delayTime_ = 0;
	pulseTime_ = 0;

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
	connect(edgeTriggerControl_, SIGNAL(connected(bool)), this, SLOT(updateEdgeTriggerControl()) );
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
	return delayBeforeControl_->value();
}

double BioXASZebraPulseControl::pulseWidthValue() const
{
	return pulseWidthControl_->value();
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

int BioXASZebraPulseControl::edgeTriggerPreference() const
{
	return edgeTriggerPreference_;
}

bool BioXASZebraPulseControl::validTimeValue(double timeValue) const
{
	bool result = false;

	if (timeValue >= BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MIN && timeValue <= BIOXASZEBRAPULSECONTROL_PULSE_WIDTH_MAX)
		result = true;

	return result;
}

double BioXASZebraPulseControl::convertTimeValue(double timeValue, double timeUnits, double desiredTimeUnits) const
{
	double result = timeValue;

	if (timeUnits != desiredTimeUnits) {

		// Convert time value to seconds.

		double timeValueSeconds = 0;

		if (timeUnits == MSeconds) // from ms
			timeValueSeconds = timeValue / 1000.0;
		else if (timeUnits == Seconds) // from s
			timeValueSeconds = timeValue;
		else if (timeUnits == DSeconds) // from 10s
			timeValueSeconds = timeValue * 10.0;

		// Complete conversion to the desired units.

		if (desiredTimeUnits == MSeconds) // to ms
			result = timeValueSeconds * 1000.0;
		else if (desiredTimeUnits == Seconds) // to s
			result = timeValueSeconds;
		else if (desiredTimeUnits == DSeconds) // to 10s
			result = timeValueSeconds / 10.0;
	}

	return result;
}

AMAction3* BioXASZebraPulseControl::createSetInputValueAction(double newValue)
{
	AMAction3 *result = 0;

	if (inputControl_)
		result = AMActionSupport::buildControlMoveAction(inputControl_, newValue);

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
	if (!delayBeforeControl_->withinTolerance(value))
		delayBeforeControl_->move(value);
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

void BioXASZebraPulseControl::setEdgeTriggerPreference(int value)
{
	if (edgeTriggerPreference_ != value) {
		edgeTriggerPreference_ = value;
		updateEdgeTriggerControl();

		emit edgeTriggerPreferenceChanged(edgeTriggerPreference_);
	}
}

void BioXASZebraPulseControl::setDelayTime(double newTime)
{
	if (delayTime_ != newTime) {
		delayTime_ = newTime;
		updateDelayBeforeValue();

		emit delayTimeChanged(delayTime_);
	}
}

void BioXASZebraPulseControl::setPulseTime(double newTime)
{
	if (pulseTime_ != newTime) {
		pulseTime_ = newTime;
		updatePulseWidthValue();

		emit pulseTimeChanged(pulseTime_);
	}
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
	updateDelayTime();
	emit delayBeforeValueChanged(delayBeforeControl_->value());
}

void BioXASZebraPulseControl::onPulseWidthValueChanged()
{
	updatePulseTime();
	emit pulseWidthValueChanged(pulseWidthControl_->value());
}

void BioXASZebraPulseControl::onTimeUnitsValueChanged()
{
	updateDelayBeforeValue();
	updatePulseWidthValue();

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

void BioXASZebraPulseControl::updateEdgeTriggerControl()
{
	setEdgeTriggerValue(edgeTriggerPreference_);
}

void BioXASZebraPulseControl::updateDelayBeforeValue()
{
	double setpoint = delayTime_;
	double units = Seconds;

	// Convert setpoint and units to the current units.

	if (units != timeUnitsControl_->value()) {
		double newUnits = timeUnitsControl_->value();
		double newSetpoint = convertTimeValue(setpoint, units, newUnits);

		if (validTimeValue(newSetpoint)) {
			setpoint = newSetpoint;
			units = newUnits;
		}
	}

	// Check that the time value and units combo is valid.
	// Convert the time value to the correct time units, if
	// necessary.

	if (!validTimeValue(setpoint)) {
		double newUnits = getValidTimeUnits(setpoint, units);

		if (newUnits != -1) {
			setpoint = convertTimeValue(setpoint, units, newUnits);
			units = newUnits;
		}
	}

	// If the setpoint and units are valid, apply setpoint to the delay
	// before control and the units to the time units control.

	if (validTimeValue(setpoint)) {
		setTimeUnitsValue(int(units)); // Units should be changed first, to prevent errors related to setting an invalid pulse width for the current units.
		setDelayBeforeValue(setpoint);
	}
}

void BioXASZebraPulseControl::updatePulseWidthValue()
{
	double setpoint = pulseTime_;
	double units = Seconds;

	// Convert setpoint and units to the current units.

	if (units != timeUnitsControl_->value()) {
		double newUnits = timeUnitsControl_->value();
		double newSetpoint = convertTimeValue(setpoint, units, newUnits);

		if (validTimeValue(newSetpoint)) {
			setpoint = newSetpoint;
			units = newUnits;
		}
	}

	// Check that the time value and units combo is valid.
	// Convert the time value to the correct time units, if
	// necessary.

	if (!validTimeValue(setpoint)) {
		double newUnits = getValidTimeUnits(setpoint, units);

		if (newUnits != -1) {
			setpoint = convertTimeValue(setpoint, units, newUnits);
			units = newUnits;
		}
	}

	// If the setpoint and units are valid, apply setpoint to the delay
	// before control and the units to the time units control.

	if (validTimeValue(setpoint)) {
		setTimeUnitsValue(int(units)); // Units should be changed first, to prevent errors related to setting an invalid pulse width for the current units.
		setPulseWidthValue(setpoint);
	}
}

void BioXASZebraPulseControl::updateDelayTime()
{
	double timeValue = delayBeforeControl_->value();
	double timeUnits = timeUnitsControl_->value();

	setDelayTime( convertTimeValue(timeValue, timeUnits, Seconds) );
}

void BioXASZebraPulseControl::updatePulseTime()
{
	double timeValue = pulseWidthControl_->value();
	double timeUnits = timeUnitsControl_->value();

	setPulseTime( convertTimeValue(timeValue, timeUnits, Seconds) );
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

double BioXASZebraPulseControl::getValidTimeUnits(double timeValue, double timeUnits)
{
	double result = -1;

	// Check to see if the time value setpoint/units combo is valid.
	// If not, attempt to convert them.

	bool timeValid = validTimeValue(timeValue);

	if (timeValid) {
		result = timeUnits;

	} else {

		double newUnits;

		// Iterate through available time units, checking to see
		// if different units will make the time value valid.

		for (int i = MSeconds, max = DSeconds; i <= max && !timeValid; i++) {
			newUnits = i;
			timeValid = validTimeValue( convertTimeValue(timeValue, timeUnits, newUnits) );
		}

		if (timeValid)
			result = newUnits;
	}

	return result;
}
