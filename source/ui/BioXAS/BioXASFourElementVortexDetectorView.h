#ifndef BIOXASFOURELEMENTVORTEXDETECTORVIEW_H
#define BIOXASFOURELEMENTVORTEXDETECTORVIEW_H

#include "ui/BioXAS/BioXASXRFDetectorView.h"

#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"

class BioXASFourElementVortexDetectorView : public BioXASXRFDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASFourElementVortexDetectorView(BioXASFourElementVortexDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFourElementVortexDetectorView(){}
};

#endif // BIOXASFOURELEMENTVORTEXDETECTORVIEW_H
