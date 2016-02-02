#ifndef BIOXASMAINSTANDARDSWHEEL_H
#define BIOXASMAINSTANDARDSWHEEL_H

#include "beamline/CLS/CLSStandardsWheel.h"

class BioXASMainStandardsWheel : public CLSStandardsWheel
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainStandardsWheel(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainStandardsWheel();
};

#endif // BIOXASMAINSTANDARDSWHEEL_H
