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

	delayBeforeSecondsControl_ = new BioXASZebraTimeSeconds(QString("PulseControl%1DelayBeforeSeconds").arg(pulseIndex), this);
	delayBeforeSecondsControl_->setTimeValueControl(delayBeforeControl_);
	delayBeforeSecondsControl_->setTimeUnitsControl(timeUnitsControl_);

	pulseWidthSecondsControl_ = new BioXASZebraTimeSeconds(QString("PulseControl%1WidthSeconds").arg(pulseIndex), this);
	pulseWidthSecondsControl_->setTimeValueControl(pulseWidthControl_);
	pulseWidthSecondsControl_->setTimeUnitsControl(timeUnitsControl_);

	connected_ = false;

	edgeTriggerPreferenceSet_ = false;
	edgeTriggerPreference_ = 0;

	inputValuePreferenceSet_ = false;
	inputValuePreference_ = 0;

	allControls_ = new AMControlSet(this);
	allControls_->addControl(inputControl_);
	allControls_->addControl(inputStatusControl_);
	allControls_->addControl(edgeTriggerControl_);
	allControls_->addControl(delayBeforeControl_);
	allControls_->addControl(pulseWidthControl_);
	allControls_->addControl(timeUnitsControl_);
	allControls_->addControl(triggerWhileActiveControl_);
	allControls_->addControl(outputPulseControl_);

	allControls_->addControl(delayBeforeSecondsControl_);
	allControls_->addControl(pulseWidthSecondsControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectedChanged(bool)));
	connect(inputControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueChanged()));
	connect(inputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueStatusChanged()));
	connect(edgeTriggerControl_, SIGNAL(valueChanged(double)), this, SLOT(onEdgeTriggerValueChanged()));
	connect(delayBeforeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDelayBeforeValueChanged()));
	connect(pulseWidthControl_, SIGNAL(valueChanged(double)), this, SLOT(onPulseWidthValueChanged()));
	connect(timeUnitsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTimeUnitsValueChanged()));
	connect(triggerWhileActiveControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerWhileActiveValueChanged()));
	connect(outputPulseControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputValueStatusChanged()));

	connect(delayBeforeSecondsControl_, SIGNAL(valueChanged(double)), this, SLOT(onDelayBeforeValueSecondsChanged()));
	connect(pulseWidthSecondsControl_, SIGNAL(valueChanged(double)), this, SLOT(onPulseWidthValueSecondsChanged()));
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

bool BioXASZebraPulseControl::triggerWhileActiveValue() const
{
	return int(triggerWhileActiveControl_->value()) == 1;
}

bool BioXASZebraPulseControl::outputValue() const
{
	return int(outputPulseControl_->value()) == 1;
}

double BioXASZebraPulseControl::delayBeforeValueSeconds() const
{
	return delayBeforeSecondsControl_->value();
}

double BioXASZebraPulseControl::pulseWidthValueSeconds() const
{
	return pulseWidthSecondsControl_->value();
}

int BioXASZebraPulseControl::edgeTriggerPreference() const
{
	return edgeTriggerPreference_;
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

void BioXASZebraPulseControl::setDelayBeforeValueSeconds(double delayValue)
{
	if (!delayBeforeSecondsControl_->withinTolerance(delayValue))
		delayBeforeSecondsControl_->move(delayValue);
}

void BioXASZebraPulseControl::setPulseWidthValueSeconds(double pulseWidth)
{
	if (!pulseWidthSecondsControl_->withinTolerance(pulseWidth))
		pulseWidthSecondsControl_->move(pulseWidth);
}

void BioXASZebraPulseControl::setEdgeTriggerPreference(int value)
{
	if (!edgeTriggerPreferenceSet_ || (edgeTriggerPreference_ != value)) {
		edgeTriggerPreferenceSet_ = true;
		edgeTriggerPreference_ = value;
		updateEdgeTriggerControl();

		emit edgeTriggerPreferenceChanged(edgeTriggerPreference_);
	}
}

void BioXASZebraPulseControl::setInputValuePreference(int value)
{
	if (!inputValuePreferenceSet_ || (inputValuePreference_ != value)) {
		inputValuePreferenceSet_ = true;
		inputValuePreference_ = value;
		updateInputControl();

		emit inputValuePreferenceChanged(inputValuePreference_);
	}
}

void BioXASZebraPulseControl::onControlSetConnectedChanged(bool connected)
{
	if (connected_ != connected){
		connected_ = connected;
		emit connectedChanged(connected_);

		updateEdgeTriggerControl();
		updateInputControl();
	}
}

void BioXASZebraPulseControl::onInputValueChanged()
{
	emit inputValueChanged(inputValue());
	emit inputValueStringChanged(inputValueString());

	//updateInputControl();
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
	emit delayBeforeValueChanged(delayBeforeControl_->value());
}

void BioXASZebraPulseControl::onPulseWidthValueChanged()
{
	emit pulseWidthValueChanged(pulseWidthControl_->value());
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

void BioXASZebraPulseControl::onDelayBeforeValueSecondsChanged()
{
	emit delayBeforeValueSecondsChanged(delayBeforeValueSeconds());
}

void BioXASZebraPulseControl::onPulseWidthValueSecondsChanged()
{
	emit pulseWidthValueSecondsChanged(pulseWidthValueSeconds());
}

void BioXASZebraPulseControl::updateEdgeTriggerControl()
{
	if (edgeTriggerPreferenceSet_)
		setEdgeTriggerValue(edgeTriggerPreference_);
}

void BioXASZebraPulseControl::updateInputControl()
{
	if (inputValuePreferenceSet_)
		setInputValue(inputValuePreference_);
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
