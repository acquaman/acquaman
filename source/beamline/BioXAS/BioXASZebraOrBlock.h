#ifndef BIOXASZEBRAORBLOCK_H
#define BIOXASZEBRAORBLOCK_H

#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

#define BIOXASZEBRAORBLOCK_INPUT_NUM 4

class BioXASZebraOrBlock : public BioXASZebraLogicBlock
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraOrBlock(const QString &name, const QString &baseName, int blockIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraOrBlock();
};

#endif // BIOXASZEBRAORBLOCK_H
