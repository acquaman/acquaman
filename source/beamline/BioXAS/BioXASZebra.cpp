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

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connect(pulseControl, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectedChanged()));

	foreach(AMControl *control, softInputControls_)
		connect(control, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));

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

void BioXASZebra::onConnectedChanged()
{
	bool connected = true;

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connected &= pulseControl->isConnected();

	foreach (AMControl *control, softInputControls_)
		connected &= control->isConnected();

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
				pulseControl->setDelayTime(signalOrigin->delayTime());
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
				pulseControl->setPulseTime(signalOrigin->pulseTime());
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
