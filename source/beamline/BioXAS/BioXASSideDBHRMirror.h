#ifndef BIOXASSIDEDBHRMIRROR_H
#define BIOXASSIDEDBHRMIRROR_H

#include "beamline/BioXAS/BioXASDBHRMirror.h"

class BioXASSideDBHRMirror : public BioXASDBHRMirror
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideDBHRMirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideDBHRMirror();

};

#endif // BIOXASSIDEDBHRMIRROR_H
