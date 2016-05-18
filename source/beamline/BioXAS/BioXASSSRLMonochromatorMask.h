#ifndef BIOXASSSRLMONOCHROMATORMASK_H
#define BIOXASSSRLMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASSSRLMonochromatorMaskState;

class BioXASSSRLMonochromatorMask : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enumeration of different state values.
	enum State { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMask();

	/// Returns true if the mask is connected, false otherwise.
	virtual bool isConnected() const;
};

#endif // BIOXASSSRLMONOCHROMATORMASK_H
