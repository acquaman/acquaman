#ifndef CLSJJSLITS_H
#define CLSJJSLITS_H

#include "beamline/AMSlits.h"

class CLSJJSlits : public AMSlits
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0);
	/// Destructor
	virtual ~CLSJJSlits();
};

#endif // CLSJJSLITS_H
