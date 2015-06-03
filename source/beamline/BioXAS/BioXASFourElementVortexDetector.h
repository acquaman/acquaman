#ifndef BIOXASFOURELEMENTVORTEXDETECTOR_H
#define BIOXASFOURELEMENTVORTEXDETECTOR_H

#include "beamline/AMXspress3XRFDetector.h"

/// Implementation of AMXRFDetector for the 4-element Vortex detector using the XPress3 electronics.
class BioXASFourElementVortexDetector : public AMXspress3XRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.
	BioXASFourElementVortexDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFourElementVortexDetector();
};

#endif // BIOXASFOURELEMENTVORTEXDETECTOR_H
