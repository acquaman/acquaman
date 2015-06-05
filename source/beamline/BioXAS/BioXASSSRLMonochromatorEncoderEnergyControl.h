#ifndef BIOXASSSRLMONOCHROMATORENCODERENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORENCODERENERGYCONTROL_H


#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"

class BioXASSSRLMonochromatorEncoderEnergyControl : public BioXASSSRLMonochromatorEnergyControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEncoderEnergyControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEncoderEnergyControl();

protected slots:
	/// Updates the current value.
	void updateValue();
};

#endif // BIOXASSSRLMONOCHROMATORENCODERENERGYCONTROL_H
