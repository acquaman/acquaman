#ifndef AMSLITS_H
#define AMSLITS_H

#include "beamline/AMControl.h"
#include "beamline/AMSlit.h"

class AMSlits : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlits(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlits();


};

#endif // AMSLITS_H
