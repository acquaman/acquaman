#ifndef BIOXASSIDEFILTERFLIPPER_H
#define BIOXASSIDEFILTERFLIPPER_H

#include "beamline/BioXAS/BioXASFilterFlipper.h"

class BioXASSideFilterFlipper : public BioXASFilterFlipper
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideFilterFlipper(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideFilterFlipper();
};

#endif // BIOXASSIDEFILTERFLIPPER_H
