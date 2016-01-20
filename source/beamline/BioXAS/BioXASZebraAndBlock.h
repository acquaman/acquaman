#ifndef BIOXASZEBRAANDBLOCK_H
#define BIOXASZEBRAANDBLOCK_H

#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

#define BIOXASZEBRAANDBLOCK_INPUT_NUM 4

class BioXASZebraAndBlock : public BioXASZebraLogicBlock
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraAndBlock(const QString &name, const QString &baseName, int blockIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraAndBlock();
};

#endif // BIOXASZEBRAANDBLOCK_H
