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

	/// Returns the scaler.
	CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the fast shutter pulse.
	BioXASZebraPulseControl* fastShutterPulse() const { return fastShutterPulse_; }
	/// Returns the Ge detector pulse.
	BioXASZebraPulseControl* geDetectorPulse() const { return geDetectorPulse_; }

signals:
	/// Notifier that the scaler has changed.
	void scalerChanged(CLSSIS3820Scaler *newScaler);
	/// Notifier that the fast shutter pulse has changed.
	void fastShutterPulseChanged(BioXASZebraPulseControl *newPulse);
	/// Notifier that the Ge detector pulse has changed.
	void geDetectorPulseChanged(BioXASZebraPulseControl *newPulse);

public slots:
	/// Sets the scaler.
	void setScaler(CLSSIS3820Scaler *newScaler);
	/// Sets the fast shutter pulse.
	void setFastShutterPulse(BioXASZebraPulseControl *newPulse);
	/// Sets the Ge detector pulse.
	void setGeDetectorPulse(BioXASZebraPulseControl *newPulse);

	/// Synchronizes the fast shutter to the Ge detector.
	void synchronizeFastShutterToGeDetector();
	/// Synchronizes the fast shutter to the scaler.
	void synchronizeFastShutterToScaler();

protected slots:
	/// Sets the fast shutter synchronization source.
	void setFastShutterPulseSyncSource(AMControl *newControl);

protected:
	/// Connects the fast shutter pulse to it's synchronization source.
	void connectFastShutterPulse();
	/// Disconnects the fast shutter pulse from it's synchronization source.
	void disconnectFastShutterPulse();

protected:
	/// The scaler.
	CLSSIS3820Scaler *scaler_;
	/// The fast shutter pulse.
	BioXASZebraPulseControl *fastShutterPulse_;
	/// The Ge detector pulse.
	BioXASZebraPulseControl *geDetectorPulse_;

	/// The fast shutter pulse synchronization source.
	AMControl *fastShutterSyncSource_;
};

#endif // BIOXASSIDEZEBRA_H
