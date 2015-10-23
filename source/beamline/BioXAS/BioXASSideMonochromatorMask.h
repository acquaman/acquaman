#ifndef BIOXASSIDEMONOCHROMATORMASK_H
#define BIOXASSIDEMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h"

class BioXASSideMonochromatorMask : public BioXASSSRLMonochromatorMaskControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideMonochromatorMask(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromatorMask();
};

#endif // BIOXASSIDEMONOCHROMATORMASK_H
