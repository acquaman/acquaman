#ifndef BIOXASFOURELEMENTVORTEXDETECTORVIEW_H
#define BIOXASFOURELEMENTVORTEXDETECTORVIEW_H

#include "ui/beamline/AMXspress3XRFDetectorView.h"

#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"

class BioXASFourElementVortexDetectorView : public AMXspress3XRFDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASFourElementVortexDetectorView(BioXASFourElementVortexDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFourElementVortexDetectorView(){}
};

#endif // BIOXASFOURELEMENTVORTEXDETECTORVIEW_H
