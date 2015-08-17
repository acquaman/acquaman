#ifndef BIOXASMAINDBHRMIRRORS_H
#define BIOXASMAINDBHRMIRRORS_H

#include "beamline/BioXAS/BioXASDBHRMirrors.h"

class BioXASMainDBHRMirrors : public BioXASDBHRMirrors
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMainDBHRMirrors(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainDBHRMirrors();
};

#endif // BIOXASMAINDBHRMIRRORS_H
