#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	// Basic setup. These settings should always be true for this Zebra.

	BioXASZebraSoftInputControl *softIn1 = softInputControlAt(0);
	if (softIn1)
		softIn1->setTimeBeforeResetPreference(0.1); // The zebra has a good chance of missing triggers if this value is lower (definitely at 0.01).

	BioXASZebraSoftInputControl *softIn2 = softInputControlAt(1);
	if (softIn2)
		softIn2->setTimeBeforeResetPreference(0);  // The fast shutter control should toggle high or toggle low when triggered.

	BioXASZebraSoftInputControl *softIn3 = softInputControlAt(2);
	if (softIn3)
		softIn3->setTimeBeforeResetPreference(0.1); // The zebra has a good chance of missing triggers if this value is lower (definitely at 0.01).

	BioXASZebraPulseControl *scanPulse = pulseControlAt(0);
	if (scanPulse) {
		scanPulse->setEdgeTriggerPreference(0); // The 'scan' pulse is edge triggered.
		scanPulse->setInputValuePreference(60); // The 'scan' pulse can always be triggered using soft input 1.
	}

	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);
	if (fastShutterBlock) {
		fastShutterBlock->setInputValuePreference(0, 61); // The fast shutter can always be triggered using soft input 2.
//		fastShutterBlock->setInputValuePreference(1, 0);

		connect( fastShutterBlock->inputControlAt(1), SIGNAL(valueChanged(double)), this, SLOT(onFastShutterBlockInputValueChanged()) );
	}

	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);
	if (scalerBlock) {
		scalerBlock->setInputValuePreference(0, 62); // The scaler can always be triggered using soft input 3.
//		scalerBlock->setInputValuePreference(1, 0);

		connect( scalerBlock->inputControlAt(1), SIGNAL(valueChanged(double)), this, SLOT(onScalerBlockInputValueChanged()) );
	}

	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);
	if (xspress3Pulse) {
		xspress3Pulse->setEdgeTriggerPreference(0); // The xspress3 pulse is edge triggered.
//		xspress3Pulse->setInputValuePreference(0);

		connect( xspress3Pulse->inputControl(), SIGNAL(valueChanged(double)), this, SLOT(onXspress3BlockInputValueChanged()) );
	}
}

BioXASSideZebra::~BioXASSideZebra()
{

}

bool BioXASSideZebra::fastShutterBlockActive() const
{
	bool result = false;

	BioXASZebraLogicBlock *block = fastShutterBlock();
	if (block && block->isConnected())
		result = (block->inputControlAt(1)->inputValue() == 52);

	return result;
}

bool BioXASSideZebra::scalerBlockActive() const
{
	bool result = false;

	BioXASZebraLogicBlock *block = scalerBlock();
	if (block && block->isConnected())
		result = (block->inputControlAt(1)->inputValue() == 52);

	return result;
}

bool BioXASSideZebra::xspress3BlockActive() const
{
	bool result = false;

	BioXASZebraPulseControl *pulse = geDetectorBlock();
	if (pulse && pulse->isConnected())
		result = (pulse->inputValue() == 52);

	return result;
}

void BioXASSideZebra::setFastShutterBlockActive()
{
	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);

	if (fastShutterBlock)
		fastShutterBlock->setInputValue(1, 52); // The fast shutter is triggered by the 'scan' pulse.
}

void BioXASSideZebra::setFastShutterBlockInactive()
{
	BioXASZebraLogicBlock *fastShutterBlock = andBlockAt(0);

	if (fastShutterBlock)
		fastShutterBlock->setInputValue(1, 0);
}

void BioXASSideZebra::setScalerBlockActive()
{
	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

	if (scalerBlock)
		scalerBlock->setInputValue(1, 52); // The scaler can be triggered using pulse 1 (#52).
}

void BioXASSideZebra::setScalerBlockInactive()
{
	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);

	if (scalerBlock)
		scalerBlock->setInputValue(1, 0);
}

void BioXASSideZebra::setXspress3BlockActive()
{
	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);

	if (xspress3Pulse)
		xspress3Pulse->setInputValue(52);
}

void BioXASSideZebra::setXspress3BlockInactive()
{
	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);

	if (xspress3Pulse)
		xspress3Pulse->setInputValue(0);
}

void BioXASSideZebra::onFastShutterBlockInputValueChanged()
{
	emit fastShutterBlockActivityChanged(fastShutterBlockActive());
}

void BioXASSideZebra::onScalerBlockInputValueChanged()
{
	emit scalerBlockActivityChanged(scalerBlockActive());
}

void BioXASSideZebra::onXspress3BlockInputValueChanged()
{
	emit xspress3BlockActivityChanged(xspress3BlockActive());
}

void BioXASSideZebra::updateBlockActivity()
{
	if (hasScaler())
		setScalerBlockActive();
	else
		setScalerBlockInactive();

	if (hasGeDetector())
		setXspress3BlockActive();
	else
		setXspress3BlockInactive();
}
