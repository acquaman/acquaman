#ifndef BIOXASSIDEZEBRA_H
#define BIOXASSIDEZEBRA_H

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASZebra.h"

class BioXASSideZebra : public BioXASZebra
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideZebra(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideZebra();

	/// Returns the scaler dwell time.
	AMControl* scalerDwellTime() const { return scalerDwellTime_; }
	/// Returns the fast shutter pulse.
	BioXASZebraPulseControl* fastShutterPulse() const { return fastShutterPulse_; }
	/// Returns the Ge detector pulse.
	BioXASZebraPulseControl* geDetectorPulse() const { return geDetectorPulse_; }

signals:
	/// Notifier that the scaler has changed.
	void scalerDwellTimeChanged(AMControl *newControl);
	/// Notifier that the fast shutter pulse has changed.
	void fastShutterPulseChanged(BioXASZebraPulseControl *newControl);
	/// Notifier that the Ge detector pulse has changed.
	void geDetectorPulseChanged(BioXASZebraPulseControl *newControl);

public slots:
	/// Sets the scaler.
	void setScalerDwellTime(AMControl *newControl);
	/// Sets the fast shutter pulse.
	void setFastShutterPulse(BioXASZebraPulseControl *newControl);
	/// Sets the Ge detector pulse.
	void setGeDetectorPulse(BioXASZebraPulseControl *newControl);

	/// Synchronizes the fast shutter to the Ge detector.
	void synchronizeFastShutterToGeDetector();
	/// Synchronizes the fast shutter to the scaler.
	void synchronizeFastShutterToScaler();

protected slots:
	/// Sets the fast shutter synchronization source.
	void setFastShutterPulseSyncSource(AMControl *newControl);

protected:
	/// Connects the fast shutter pulse to its synchronization source.
	void connectFastShutterPulse();
	/// Disconnects the fast shutter pulse from its synchronization source.
	void disconnectFastShutterPulse();

protected:
	/// The scaler dwell time.
	AMControl *scalerDwellTime_;
	/// The fast shutter pulse.
	BioXASZebraPulseControl *fastShutterPulse_;
	/// The Ge detector pulse.
	BioXASZebraPulseControl *geDetectorPulse_;

	/// The fast shutter pulse synchronization source.
	AMControl *fastShutterSyncSource_;
};

#endif // BIOXASSIDEZEBRA_H
