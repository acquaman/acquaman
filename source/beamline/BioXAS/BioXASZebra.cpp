#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &baseName, QObject *parent)
	: QObject(parent)
{
	connected_ = false;

	pulseControls_ << new BioXASZebraPulseControl(baseName, 1, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 2, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 3, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 4, this);

	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn1", QString("%1:SOFT_IN:B0").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn2", QString("%1:SOFT_IN:B1").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn3", QString("%1:SOFT_IN:B2").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn4", QString("%1:SOFT_IN:B3").arg(baseName), this, 0.5);

	andBlocks_ << new BioXASZebraLogicBlock("AND1", QString("%1:AND1").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND2", QString("%1:AND2").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND3", QString("%1:AND3").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND4", QString("%1:AND4").arg(baseName), this);

	orBlocks_ << new BioXASZebraLogicBlock("OR1", QString("%1:OR1").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR2", QString("%1:OR2").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR3", QString("%1:OR3").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR4", QString("%1:OR4").arg(baseName), this);

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connect(pulseControl, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectedChanged()));

	foreach(AMControl *control, softInputControls_)
		connect(control, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));

	foreach (AMControl *andBlock, andBlocks_)
		connect( andBlock, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	foreach (AMControl *orBlock, orBlocks_)
		connect( orBlock, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	// Setup synchronization capabilities.

	synchronizedDelayBeforeMapper_ = new QSignalMapper(this);
	connect( synchronizedDelayBeforeMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSynchronizedDelayBeforeValueChanged(QObject*)) );

	synchronizedPulseWidthMapper_ = new QSignalMapper(this);
	connect( synchronizedPulseWidthMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSynchronizedPulseWidthValueChanged(QObject*)) );

	synchronizedTimeUnitsMapper_ = new QSignalMapper(this);
	connect( synchronizedTimeUnitsMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSynchronizedTimeUnitsValueChanged(QObject*)) );
}

BioXASZebra::~BioXASZebra()
{

}

bool BioXASZebra::isConnected() const
{
	return connected_;
}

QList<BioXASZebraPulseControl *> BioXASZebra::pulseControls() const
{
	return pulseControls_;
}

BioXASZebraPulseControl *BioXASZebra::pulseControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return pulseControls_.at(index);

	return 0;
}

QList<BioXASZebraSoftInputControl *> BioXASZebra::softInputControls() const
{
	return softInputControls_;
}

BioXASZebraSoftInputControl *BioXASZebra::softInputControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return softInputControls_.at(index);

	return 0;
}

QList<BioXASZebraLogicBlock*> BioXASZebra::andBlocks() const
{
	return andBlocks_;
}

BioXASZebraLogicBlock* BioXASZebra::andBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < andBlocks_.count())
		result = andBlocks_.at(index);

	return result;
}

QList<BioXASZebraLogicBlock*> BioXASZebra::orBlocks() const
{
	return orBlocks_;
}

BioXASZebraLogicBlock* BioXASZebra::orBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < orBlocks_.count())
		result = orBlocks_.at(index);

	return result;
}

QList<BioXASZebraOutputControl*> BioXASZebra::outputControls() const
{
	return outputControls_;
}

BioXASZebraOutputControl* BioXASZebra::outputControlAt(int index) const
{
	BioXASZebraOutputControl *result = 0;

	if (index >= 0 && index < outputControls_.count())
		result = outputControls_.at(index);

	return result;
}

void BioXASZebra::onConnectedChanged()
{
	bool connected = true;

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connected &= pulseControl->isConnected();

	foreach (AMControl *control, softInputControls_)
		connected &= control->isConnected();

	foreach (AMControl *andBlock, andBlocks_)
		connected &= andBlock->isConnected();

	foreach (AMControl *orBlock, orBlocks_)
		connected &= orBlock->isConnected();

	foreach (AMControl *outputControl, outputControls_)
		connected &= outputControl->isConnected();

	if (connected_ != connected){
		connected_ = connected;
		emit connectedChanged(connected_);
	}

	// Add the appropriate pulse controls to the list of synchronized
	// pulse controls. This must happen only after they are connected,
	// or they will be synchronized with not real values.

	if (connected) {
		addSynchronizedPulseControl(pulseControls_.at(0));
		addSynchronizedPulseControl(pulseControls_.at(2));
	}
}

bool BioXASZebra::addSynchronizedPulseControl(BioXASZebraPulseControl *newControl)
{
	bool result = false;

	if (newControl && !synchronizedPulseControls_.contains(newControl)) {

		// Add control to the list of synchronized pulse controls.

		synchronizedPulseControls_.append(newControl);

		// Setup mappings.

		synchronizedDelayBeforeMapper_->setMapping(newControl, newControl);
		connect( newControl, SIGNAL(delayBeforeValueChanged(double)), synchronizedDelayBeforeMapper_, SLOT(map()) );

		synchronizedPulseWidthMapper_->setMapping(newControl, newControl);
		connect( newControl, SIGNAL(pulseWidthValueChanged(double)), synchronizedPulseWidthMapper_, SLOT(map()) );

		synchronizedTimeUnitsMapper_->setMapping(newControl, newControl);
		connect( newControl, SIGNAL(timeUnitsValueChanged(int)), synchronizedTimeUnitsMapper_, SLOT(map()) );

		result = true;
	}

	return result;
}

bool BioXASZebra::removeSynchronizedPulseControl(BioXASZebraPulseControl *control)
{
	bool result = false;

	if (synchronizedPulseControls_.contains(control)) {

		// Remove mappings.

		synchronizedDelayBeforeMapper_->removeMappings(control);
		disconnect( control, 0, synchronizedDelayBeforeMapper_, 0 );

		synchronizedPulseWidthMapper_->removeMappings(control);
		disconnect( control, 0, synchronizedPulseWidthMapper_, 0 );

		synchronizedTimeUnitsMapper_->removeMappings(control);
		disconnect( control, 0, synchronizedTimeUnitsMapper_, 0 );

		// Remove control from list of synchronized pulse controls.

		synchronizedPulseControls_.removeOne(control);

		result = true;
	}

	return result;
}

void BioXASZebra::onSynchronizedDelayBeforeValueChanged(QObject *controlObject)
{
	BioXASZebraPulseControl *signalOrigin = qobject_cast<BioXASZebraPulseControl*>(controlObject);

	if (signalOrigin) {

		// Iterate through the list of synchronized pulse controls,
		// updating the 'delay before' values for each. We skip
		// the control that originally sent the value changed signal.

		foreach (BioXASZebraPulseControl *pulseControl, synchronizedPulseControls_) {
			if (pulseControl && pulseControl != signalOrigin)
				pulseControl->setDelayBeforeValue(signalOrigin->delayBeforeValue());
		}
	}
}

void BioXASZebra::onSynchronizedPulseWidthValueChanged(QObject *controlObject)
{
	BioXASZebraPulseControl *signalOrigin = qobject_cast<BioXASZebraPulseControl*>(controlObject);

	if (signalOrigin) {

		// Iterate through the list of synchronized pulse controls,
		// updating the 'pulse width' values for each. We skip
		// the control that originally sent the value changed signal.

		foreach (BioXASZebraPulseControl *pulseControl, synchronizedPulseControls_) {
			if (pulseControl && pulseControl != signalOrigin)
				pulseControl->setPulseWidthValue(signalOrigin->pulseWidthValue());
		}
	}
}

void BioXASZebra::onSynchronizedTimeUnitsValueChanged(QObject *controlObject)
{
	BioXASZebraPulseControl *signalOrigin = qobject_cast<BioXASZebraPulseControl*>(controlObject);

	if (signalOrigin) {

		// Iterate through the list of synchronized pulse controls,
		// updating the 'time units' values for each. We skip
		// the control that originally sent the value changed signal.

		foreach (BioXASZebraPulseControl *pulseControl, synchronizedPulseControls_) {
			if (pulseControl && pulseControl != signalOrigin)
				pulseControl->setTimeUnitsValue(signalOrigin->timeUnitsValue());
		}
	}
}
