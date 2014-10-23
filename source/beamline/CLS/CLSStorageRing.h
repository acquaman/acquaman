#ifndef CLSSTORAGERING_H
#define CLSSTORAGERING_H

#include "beamline/AMStorageRing.h"

/// This class encapsulates the CLS Storage Ring.
/*!
	Currently, it manages the ring current PV and it's
	shutters enabled PV to determine the beam status.
  */
class CLSStorageRing : public AMStorageRing
{
	Q_OBJECT

public:
	/// Singleton accessor for the CLS Storage Ring.  If no pointer exists then it creates a new instance and then returns it.
	static CLSStorageRing *sr1();

	/// Destructor.
	~CLSStorageRing();

protected slots:
	/// Determines whether the beam availability has changed.
	void determineBeamAvailability();

protected:
	/// Specific constructor for the CLS storage ring.
	CLSStorageRing(QObject *parent = 0);

	/// The control for the SR status that is manually changed by the control room.
	AMControl *srStatus_;
};

#endif // CLSSTORAGERING_H
