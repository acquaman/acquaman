#ifndef BIOXASSIDEDBHRMIRRORS_H
#define BIOXASSIDEDBHRMIRRORS_H

#include "beamline/BioXAS/BioXASDBHRMirrors.h"

class BioXASSideDBHRMirrors : public BioXASDBHRMirrors
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideDBHRMirrors(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideDBHRMirrors();

};

#endif // BIOXASSIDEDBHRMIRRORS_H
