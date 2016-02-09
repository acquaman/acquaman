#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &name, const QString &baseName, QObject *parent) :
	BioXASZebra(name, baseName, parent)
{
	// Initialize class variables.

	scalerDwellTime_ = 0;
	fastShutterPulse_ = 0;
	geDetectorPulse_ = 0;

	fastShutterSyncSource_ = 0;

	// Current settings.

	setFastShutterPulse(pulseControlAt(0));
	setGeDetectorPulse(pulseControlAt(2));

	setFastShutterPulseSyncSource(pulseControlAt(2)); // Initially, the fast shutter is synchronized to the Ge detector.
}

BioXASSideZebra::~BioXASSideZebra()
{

}

void BioXASSideZebra::setScalerDwellTime(AMControl *newControl)
{
	if (scalerDwellTime_ != newControl) {
		scalerDwellTime_ = newControl;
		emit scalerDwellTimeChanged(scalerDwellTime_);
	}
}

void BioXASSideZebra::setFastShutterPulse(BioXASZebraPulseControl *newControl)
{
	if (fastShutterPulse_ != newControl) {
		disconnectFastShutterPulse();
		fastShutterPulse_ = newControl;
		connectFastShutterPulse();

		emit fastShutterPulseChanged(fastShutterPulse_);
	}
}

void BioXASSideZebra::setGeDetectorPulse(BioXASZebraPulseControl *newControl)
{
	if (geDetectorPulse_ != newControl) {
		geDetectorPulse_ = newControl;
		emit geDetectorPulseChanged(geDetectorPulse_);
	}
}

void BioXASSideZebra::synchronizeFastShutterToGeDetector()
{
	setFastShutterPulseSyncSource(geDetectorPulse_);
}

void BioXASSideZebra::synchronizeFastShutterToScaler()
{
	setFastShutterPulseSyncSource(scalerDwellTime_);
}

void BioXASSideZebra::setFastShutterPulseSyncSource(AMControl *newControl)
{
	if (fastShutterSyncSource_ != newControl) {
		disconnectFastShutterPulse();
		fastShutterSyncSource_ = newControl;
		connectFastShutterPulse();
	}
}

void BioXASSideZebra::connectFastShutterPulse()
{
	if (fastShutterPulse_ && fastShutterSyncSource_)
		connect( fastShutterSyncSource_, SIGNAL(valueChanged(double)), fastShutterPulse_, SLOT(setPulseWidthValueSeconds(double)) );
}

void BioXASSideZebra::disconnectFastShutterPulse()
{
	if (fastShutterPulse_ && fastShutterSyncSource_)
		disconnect( fastShutterSyncSource_, 0, fastShutterPulse_, 0 );
}
