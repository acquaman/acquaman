#ifndef BIOXASMAINMONOCHROMATORMASK_H
#define BIOXASMAINMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorMask.h"

class BioXASMainMonochromatorMask : public BioXASSSRLMonochromatorMask
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorMask(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorMask();
};

#endif // BIOXASMAINMONOCHROMATORMASK_H
