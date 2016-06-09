#ifndef BIOXASMONOCHROMATOR_H
#define BIOXASMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class CLSMAXvMotor;
class BioXASMonochromatorEnergyControl;

class BioXASMonochromator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromator();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the energy control.
	BioXASMonochromatorEnergyControl* energy() const { return energy_; }

signals:
	/// Notifier that the energy control has changed.
	void energyChanged(BioXASMonochromatorEnergyControl *newControl);


protected slots:
	/// Sets the energy control.
	virtual void setEnergy(BioXASMonochromatorEnergyControl *newControl);

protected:
	/// The energy control.
	BioXASMonochromatorEnergyControl *energy_;
};

#endif // BIOXASMONOCHROMATOR_H
