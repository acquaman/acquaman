#include "BioXASZebraPulseControl.h"

#include "actions3/AMActionSupport.h"
#include "beamline/BioXAS/BioXASZebraCommands.h"

BioXASZebraPulseControl::BioXASZebraPulseControl(const QString &name, const QString &baseName, int pulseIndex, QObject *parent)
	: AMConnectedControl(name, "", parent)
{
	// The input control.

	inputControl_ = new AMSinglePVControl(QString("PulseControl%1Input").arg(pulseIndex),
					      QString("%1:PULSE%2_INP").arg(baseName).arg(pulseIndex),
					      this,
					      1.0);

	connect(inputControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueChanged()));
	addChildControl(inputControl_);

	// The input status control.

	inputStatusControl_ = new AMReadOnlyPVControl(QString("PulseControl%1InputStatus").arg(pulseIndex),
						      QString("%1:PULSE%2_INP:STA").arg(baseName).arg(pulseIndex),
						      this);

	connect(inputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputValueStatusChanged()));
	addChildControl(inputStatusControl_);

	// The edge trigger control.

	edgeTriggerPreference_ = 0;

	edgeTriggerControl_ = new AMSinglePVControl(QString("PulseControl%1EdgeTrigger").arg(pulseIndex),
						    QString("%1:POLARITY:B%2").arg(baseName).arg(letterFromPulseIndex(pulseIndex)),
						    this,
						    0.5);

	connect(edgeTriggerControl_, SIGNAL(valueChanged(double)), this, SLOT(onEdgeTriggerValueChanged()));
	addChildControl(edgeTriggerControl_);

	// The delay before control.

	delayBeforeControl_ = new AMSinglePVControl(QString("PulseControl%1DelayBefore").arg(pulseIndex),
						    QString("%1:PULSE%2_DLY").arg(baseName).arg(pulseIndex),
						    this,
						    0.0001);

	connect(delayBeforeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDelayBeforeValueChanged()));
	addChildControl(delayBeforeControl_);

	// The pulse width control.

	pulseWidthControl_ = new AMSinglePVControl(QString("PulseControl%1Width").arg(pulseIndex),
						   QString("%1:PULSE%2_WID").arg(baseName).arg(pulseIndex),
						   this,
						   0.0001);

	connect(pulseWidthControl_, SIGNAL(valueChanged(double)), this, SLOT(onPulseWidthValueChanged()));
	addChildControl(pulseWidthControl_);

	// The time units control.

	timeUnitsControl_ = new AMSinglePVControl(QString("PulseControl%1TimeUnits").arg(pulseIndex),
						  QString("%1:PULSE%2_PRE").arg(baseName).arg(pulseIndex),
						  this,
						  0.5);

	connect(timeUnitsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTimeUnitsValueChanged()));
	addChildControl(timeUnitsControl_);

	// The trigger while active control.

	triggerWhileActiveControl_ = new AMReadOnlyPVControl(QString("PulseControl%1TriggerActive").arg(pulseIndex),
							     QString("%1:SYS_STATERR.B%2").arg(baseName).arg(pulseIndex-1),
							     this);

	connect(triggerWhileActiveControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerWhileActiveValueChanged()));
	addChildControl(triggerWhileActiveControl_);

	// The output pulse control

	outputPulseControl_ = new AMReadOnlyPVControl(QString("PulseControl%1OutputPulse").arg(pulseIndex),
						      QString("%1:PULSE%2_OUT").arg(baseName).arg(pulseIndex),
						      this);

	connect(outputPulseControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputValueStatusChanged()));
	addChildControl(outputPulseControl_);

	// The delay before control, in seconds.

	delayBeforeSecondsControl_ = new BioXASZebraTimeSeconds(QString("PulseControl%1DelayBeforeSeconds").arg(pulseIndex), this);
	delayBeforeSecondsControl_->setTimeValueControl(delayBeforeControl_);
	delayBeforeSecondsControl_->setTimeUnitsControl(timeUnitsControl_);

	connect(delayBeforeSecondsControl_, SIGNAL(valueChanged(double)), this, SLOT(onDelayBeforeValueSecondsChanged()));
	addChildControl(delayBeforeSecondsControl_);

	// The pulse width control, in seconds.

	pulseWidthSecondsControl_ = new BioXASZebraTimeSeconds(QString("PulseControl%1WidthSeconds").arg(pulseIndex), this);
	pulseWidthSecondsControl_->setTimeValueControl(pulseWidthControl_);
	pulseWidthSecondsControl_->setTimeUnitsControl(timeUnitsControl_);

	connect(pulseWidthSecondsControl_, SIGNAL(valueChanged(double)), this, SLOT(onPulseWidthValueSecondsChanged()));
	addChildControl(pulseWidthSecondsControl_);
}

BioXASZebraPulseControl::~BioXASZebraPulseControl()
{

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
	if (edgeTriggerPreference_ != value) {
		edgeTriggerPreference_ = value;
		updateEdgeTriggerControl();

		emit edgeTriggerPreferenceChanged(edgeTriggerPreference_);
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
	setEdgeTriggerValue(edgeTriggerPreference_);
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
