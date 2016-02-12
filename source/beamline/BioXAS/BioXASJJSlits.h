#ifndef BIOXASJJSLITS_H
#define BIOXASJJSLITS_H

#include "beamline/AMSlits.h"

class BioXASJJSlits : public AMSlits
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0);
	/// Destructor.
	virtual ~BioXASJJSlits();
};

#endif // BIOXASJJSLITS_H
