#ifndef BIOXASMAINXIAFILTERS_H
#define BIOXASMAINXIAFILTERS_H

#include "beamline/BioXAS/BioXASXIAFilters.h"

class BioXASMainXIAFilters : public BioXASXIAFilters
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainXIAFilters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainXIAFilters();
};

#endif // BIOXASMAINXIAFILTERS_H
