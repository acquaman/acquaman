#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	// Initialize class variables.

	scaler_ = 0;
	fastShutterPulse_ = 0;
	geDetectorPulse_ = 0;

	fastShutterSyncSource_ = 0;

	// Current settings.

	setFastShutterPulse(pulseControlAt(0));
	setGeDetectorPulse(pulseControlAt(2));
}

BioXASSideZebra::~BioXASSideZebra()
{

}

void BioXASSideZebra::setScaler(CLSSIS3820Scaler *newScaler)
{
	if (scaler_ != newScaler) {
		scaler_ = newScaler;
		emit scalerChanged(scaler_);
	}
}

void BioXASSideZebra::setFastShutterPulse(BioXASZebraPulseControl *newPulse)
{
	if (fastShutterPulse_ != newPulse) {
		disconnectFastShutterPulse();
		fastShutterPulse_ = newPulse;
		connectFastShutterPulse();

		emit fastShutterPulseChanged(fastShutterPulse_);
	}
}

void BioXASSideZebra::setGeDetectorPulse(BioXASZebraPulseControl *newPulse)
{
	if (geDetectorPulse_ != newPulse) {
		geDetectorPulse_ = newPulse;
		emit geDetectorPulseChanged(geDetectorPulse_);
	}
}

void BioXASSideZebra::synchronizeFastShutterToGeDetector()
{
	setFastShutterPulseSyncSource(geDetectorPulse_);
}

void BioXASSideZebra::synchronizeFastShutterToScaler()
{
	if (scaler_)
		setFastShutterPulseSyncSource(scaler_->dwellTimeControl());
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
		disconnect( fastShutterSyncSource_, 0, fastShutter_, 0 );
}
