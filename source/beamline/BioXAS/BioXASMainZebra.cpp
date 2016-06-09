#include "BioXASMainZebra.h"

BioXASMainZebra::BioXASMainZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	// Add the Main-specific output controls.

	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 TTL"), QString("%1:OUT1_TTL").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 NIM"), QString("%1:OUT1_NIM").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT2 TTL"), QString("%1:OUT2_TTL").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT3 TTL"), QString("%1:OUT3_TTL").arg(baseName), this);

	foreach (AMControl *outputControl, outputControls_)
		connect( outputControl, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	onConnectedChanged();

	// Value preferences.

	BioXASZebraPulseControl *pulse1 = pulseControlAt(0);
	if (pulse1)
		pulse1->setEdgeTriggerPreference(0);

	BioXASZebraPulseControl *pulse3 = pulseControlAt(2);
	if (pulse3)
		pulse3->setEdgeTriggerPreference(0);

	BioXASZebraSoftInputControl *softIn1 = softInputControlAt(0);
	if (softIn1)
		softIn1->setTimeBeforeResetPreference(0.1);

	BioXASZebraSoftInputControl *softIn3 = softInputControlAt(2);
	if (softIn3)
		softIn3->setTimeBeforeResetPreference(0.1);

	BioXASZebraLogicBlock *scalerBlock = orBlockAt(1);
	if (scalerBlock) {
		scalerBlock->setInputValuePreference(0, 52); // The scaler can be triggered using pulse 1 (#52).
		scalerBlock->setInputValuePreference(1, 62); // The scaler can be triggered using soft input 3 (#62).
	}

	BioXASZebraPulseControl *scanPulse = pulseControlAt(0);
	if (scanPulse)
		scanPulse->setInputValuePreference(60); // The 'scan pulse' (pulse 1) can be triggered using soft input 1 (#60).

	BioXASZebraPulseControl *xspress3Pulse = pulseControlAt(2);
	if (xspress3Pulse)
		xspress3Pulse->setInputValuePreference(52); // The Xspress3 detector can be triggered using pulse 1 (#52).

	BioXASZebraOutputControl *out1TTL = outputControlAt(0);
	if (out1TTL)
		out1TTL->setOutputValuePreference(54); // The OUT1TTL output should connect to PULSE3, or the Ge detector.

	BioXASZebraOutputControl *out1NIM = outputControlAt(1);
	if (out1NIM)
		out1NIM->setOutputValuePreference(37); // The OUT1NIM output should connect to OR2, or the scaler.

	BioXASZebraOutputControl *out2TTL = outputControlAt(2);
	if (out2TTL)
		out2TTL->setOutputValuePreference(54); // The OUT2TTL output should connect to PULSE3, or the Ge detector.

	BioXASZebraOutputControl *out3TTL = outputControlAt(3);
	if (out3TTL)
		out3TTL->setOutputValuePreference(32); // The OUT3TTL output should connect to AND1, or the fast shutter.
}

BioXASMainZebra::~BioXASMainZebra()
{

}

