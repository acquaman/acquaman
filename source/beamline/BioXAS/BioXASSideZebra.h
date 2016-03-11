#ifndef BIOXASSIDEZEBRA_H
#define BIOXASSIDEZEBRA_H

#include <QObject>
#include "beamline/BioXAS/BioXASZebra.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"

class BioXASSideZebra : public BioXASZebra
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSideZebra(const QString &baseName, QObject *parent = 0);
	/// Destuctor.
	virtual ~BioXASSideZebra();

	/// Returns true if the fast shutter is added, false otherwise.
	bool hasFastShutter() const;
	/// Returns true if the scaler is added, false otherwise.
	bool hasScaler() const;
	/// Returns true if the Ge detector is added, false otherwise.
	bool hasGeDetector() const;

	/// Returns the fast shutter AND block.
	BioXASZebraLogicBlock* fastShutterBlock() const { return andBlockAt(0); }
	/// Returns the scaler OR block.
	BioXASZebraLogicBlock* scalerBlock() const { return orBlockAt(1); }
	/// Returns the Ge detector pulse block.
	BioXASZebraPulseControl* geDetectorPulse() const { return pulseControlAt(2); }

signals:
	/// Notifier that the detectors have changed.
	void detectorsChanged();
	/// Notifier that the detector managers have changed.
	void detectorManagersChanged();

	/// Notifier that the fast shutter has been added or removed.
	void fastShutterChanged(bool isAdded);
	/// Notifier that a scaler channel detector has been added or removed.
	void scalerChannelDetectorAdded(bool isAdded);
	/// Notifier that the scaler has been added or removed.
	void scalerChanged(bool isAdded);
	/// Notifier that the Ge detector has been added or removed.
	void geDetectorChanged(bool isAdded);

public slots:
	/// Adds a detector.
	virtual bool addDetector(AMDetector *newDetector);
	/// Removes a detector.
	virtual bool removeDetector(AMDetector *detector);
	/// Clears all detectors.
	virtual bool clearDetectors();

protected slots:
	/// Adds the fast shutter.
	void addFastShutter();
	/// Removes the fast shutter.
	void removeFastShutter();
	/// Handles emitting fast shutter changed signal, when the fast shutter AND block reports an input value change.
	void onFastShutterBlockInputValueChanged();

	/// Adds a scaler channel detector.
	void addScalerChannelDetector(CLSBasicScalerChannelDetector *detector);
	/// Removes a scaler channel detector.
	void removeScalerChannelDetector(CLSBasicScalerChannelDetector *detector);

	/// Adds the scaler.
	void addScaler(CLSSIS3820Scaler *scaler);
	/// Removes the scaler.
	void removeScaler(CLSSIS3820Scaler *scaler);
	/// Handles emitting scaler changed signal, when the scaler OR block reports an input value change.
	void onScalerBlockInputValueChanged();

	/// Adds a Ge-32 detector.
	void addGeDetector(BioXAS32ElementGeDetector *detector);
	/// Removes a Ge-32 detector.
	void removeGeDetector(BioXAS32ElementGeDetector *detector);
	/// Handles emitting Ge detector changed signal, when the Ge detector pulse block reports an input value change.
	void onGeDetectorBlockInputValueChanged();

protected:
	/// The list of scaler channel detectors.
	QList<AMDetector*> scalerChannelDetectors_;
	/// The list of Ge detectors.
	QList<BioXAS32ElementGeDetector*> geDetectors_;
};

#endif // BIOXASSIDEZEBRA_H
