#ifndef BIOXAS32ELEMENTGEDETECTORVIEW_H
#define BIOXAS32ELEMENTGEDETECTORVIEW_H

#include "ui/BioXAS/BioXASXRFDetectorView.h"

#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

class BioXAS32ElementGeDetectorView : public BioXASXRFDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetectorView(){}
};

#endif // BIOXAS32ELEMENTGEDETECTORVIEW_H
