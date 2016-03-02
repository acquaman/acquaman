#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &name, const QString &baseName, QObject *parent)
	: AMTriggerManager(name, parent)
{
	connected_ = false;

	pulseControls_ = new AMControlSet(this);
	connect( pulseControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	pulseControls_->addControl(new BioXASZebraPulseControl(baseName, 1, this));
	pulseControls_->addControl(new BioXASZebraPulseControl(baseName, 2, this));
	pulseControls_->addControl(new BioXASZebraPulseControl(baseName, 3, this));
	pulseControls_->addControl(new BioXASZebraPulseControl(baseName, 4, this));

	softInputControls_ = new AMControlSet(this);
	connect( softInputControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	softInputControls_->addControl(new BioXASZebraSoftInputControl("SoftIn1", QString("%1:SOFT_IN:B0").arg(baseName), this, 0.5));
	softInputControls_->addControl(new BioXASZebraSoftInputControl("SoftIn2", QString("%1:SOFT_IN:B1").arg(baseName), this, 0.5));
	softInputControls_->addControl(new BioXASZebraSoftInputControl("SoftIn3", QString("%1:SOFT_IN:B2").arg(baseName), this, 0.5));
	softInputControls_->addControl(new BioXASZebraSoftInputControl("SoftIn4", QString("%1:SOFT_IN:B3").arg(baseName), this, 0.5));

	andBlocks_ = new AMControlSet(this);
	connect( andBlocks_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	andBlocks_->addControl(new BioXASZebraLogicBlock("AND1", QString("%1:AND1").arg(baseName), this));
	andBlocks_->addControl(new BioXASZebraLogicBlock("AND2", QString("%1:AND2").arg(baseName), this));
	andBlocks_->addControl(new BioXASZebraLogicBlock("AND3", QString("%1:AND3").arg(baseName), this));
	andBlocks_->addControl(new BioXASZebraLogicBlock("AND4", QString("%1:AND4").arg(baseName), this));

	orBlocks_ = new AMControlSet(this);
	connect( orBlocks_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	orBlocks_->addControl(new BioXASZebraLogicBlock("OR1", QString("%1:OR1").arg(baseName), this));
	orBlocks_->addControl(new BioXASZebraLogicBlock("OR2", QString("%1:OR2").arg(baseName), this));
	orBlocks_->addControl(new BioXASZebraLogicBlock("OR3", QString("%1:OR3").arg(baseName), this));
	orBlocks_->addControl(new BioXASZebraLogicBlock("OR4", QString("%1:OR4").arg(baseName), this));

	// Set up trigger source.

	AMZebraDetectorTriggerSource *triggerSource = new AMZebraDetectorTriggerSource(QString("%1%2").arg(baseName).arg("TriggerSource"), this);
	triggerSource->setTriggerControl(softInputControlAt(0));

	setTriggerSource(triggerSource);

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

BioXASZebraPulseControl *BioXASZebra::pulseControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return qobject_cast<BioXASZebraPulseControl*>(pulseControls_->at(index));

	return 0;
}

BioXASZebraSoftInputControl *BioXASZebra::softInputControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return qobject_cast<BioXASZebraSoftInputControl*>(softInputControls_->at(index));

	return 0;
}

BioXASZebraLogicBlock* BioXASZebra::andBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < andBlocks_->count())
		result = qobject_cast<BioXASZebraLogicBlock*>(andBlocks_->at(index));

	return result;
}

BioXASZebraLogicBlock* BioXASZebra::orBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < orBlocks_->count())
		result = qobject_cast<BioXASZebraLogicBlock*>(orBlocks_->at(index));

	return result;
}

void BioXASZebra::onConnectedChanged()
{
	updateConnected();

	// Add the appropriate pulse controls to the list of synchronized
	// pulse controls. This must happen only after they are connected,
	// or they will be synchronized with not real values.

	if (isConnected()) {
		addSynchronizedPulseControl(pulseControlAt(0));
		addSynchronizedPulseControl(pulseControlAt(2));
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

bool BioXASZebra::managerConnected() const
{
	return (AMTriggerManager::managerConnected() && pulseControls_->isConnected() && softInputControls_->isConnected() && andBlocks_->isConnected() && orBlocks_->isConnected() );
}
