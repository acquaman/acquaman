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

public slots:
	/// Adds a detector.
	virtual bool addDetector(AMDetector *newDetector);
	/// Removes a detector.
	virtual bool removeDetector(AMDetector *detector);
	/// Clears all detectors.
	virtual bool clearDetectors();

	/// Adds the fast shutter.
	void addFastShutter();
	/// Removes the fast shutter.
	void removeFastShutter();

	/// Adds a scaler channel detector.
	void addScalerChannelDetector(CLSBasicScalerChannelDetector *detector);
	/// Removes a scaler channel detector.
	void removeScalerChannelDetector(CLSBasicScalerChannelDetector *detector);

	/// Adds the scaler.
	void addScaler(CLSSIS3820Scaler *scaler);
	/// Removes the scaler.
	void removeScaler(CLSSIS3820Scaler *scaler);

	/// Adds a Ge-32 detector.
	void addGeDetector(BioXAS32ElementGeDetector *detector);
	/// Removes a Ge-32 detector.
	void removeGeDetector(BioXAS32ElementGeDetector *detector);

protected slots:
	/// Testing.
	void onInputControlValueChanged(int newValue);

protected:
	/// The list of scaler channel detectors.
	QList<AMDetector*> scalerChannelDetectors_;
	/// The list of Ge detectors.
	QList<BioXAS32ElementGeDetector*> geDetectors_;
};

#endif // BIOXASSIDEZEBRA_H
