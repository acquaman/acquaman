#ifndef BIOXASSIDEXIAFILTERS_H
#define BIOXASSIDEXIAFILTERS_H

#include "beamline/BioXAS/BioXASXIAFilters.h"

class BioXASSideXIAFilters : public BioXASXIAFilters
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideXIAFilters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideXIAFilters();
};

#endif // BIOXASSIDEXIAFILTERS_H
