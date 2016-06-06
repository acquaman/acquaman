#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

#define BIOXASSIDEMONOCHROMATOR_MASK_UPPER_BLADE_CW_DESTINATION -5
#define BIOXASSIDEMONOCHROMATOR_MASK_LOWER_BLADE_CW_DESTINATION 5
#define BIOXASSIDEMONOCHROMATOR_PADDLE_CCW_DESTINATION -1
#define BIOXASSIDEMONOCHROMATOR_BRAGG_CW_DESTINATION -1000
#define BIOXASSIDEMONOCHROMATOR_BRAGG_CCW_DESTINATION 1000
#define BIOXASSIDEMONOCHROMATOR_CRYSTAL_CHANGE_CW_DESTINATION 15000
#define BIOXASSIDEMONOCHROMATOR_CRYSTAL_CHANGE_CCW_DESTINATION -15000

class BioXASSideMonochromator : public BioXASSSRLMonochromator
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromator();
};

#endif // BIOXASSIDEMONOCHROMATOR_H
