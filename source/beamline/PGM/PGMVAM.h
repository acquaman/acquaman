#ifndef PGMVAM_H
#define PGMVAM_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMPVControl.h"

class PGMVAM : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	PGMVAM(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~PGMVAM();

	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }
	/// Returns the outboard blade control.
	AMControl* outboardBlade() const { return outboardBlade_; }
	/// Returns the inboard blade control.
	AMControl* inboardBlade() const { return inboardBlade_; }

protected:
	/// The upper blade control.
	AMControl *upperBlade_;
	/// The lower blade control.
	AMControl *lowerBlade_;
	/// The outboard blade control.
	AMControl *outboardBlade_;
	/// The inboard blade control.
	AMControl *inboardBlade_;
};

#endif // PGMVAM_H
