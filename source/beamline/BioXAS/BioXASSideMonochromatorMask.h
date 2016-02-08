#ifndef BIOXASSIDEMONOCHROMATORMASK_H
#define BIOXASSIDEMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorMask.h"

class BioXASSideMonochromatorMask : public BioXASSSRLMonochromatorMask
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromatorMask(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromatorMask();
};

#endif // BIOXASSIDEMONOCHROMATORMASK_H
