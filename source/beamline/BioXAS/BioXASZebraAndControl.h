#ifndef BIOXASZEBRAANDCONTROL_H
#define BIOXASZEBRAANDCONTROL_H

#include "beamline/BioXAS/BioXASZebraLogicBlockControl.h"

#define BIOXASZEBRAANDCONTROL_INPUT_CONTROL_NUM 4

class BioXASZebraAndControl : public BioXASZebraLogicBlockControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraAndControl(const QString &name, const QString &baseName, int blockIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraAndControl();
};

#endif // BIOXASZEBRAANDCONTROL_H
