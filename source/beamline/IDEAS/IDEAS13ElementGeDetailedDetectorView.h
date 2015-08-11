#ifndef IDEAS13ELEMENTGEDETAILEDDETECTORVIEW_H
#define IDEAS13ELEMENTGEDETAILEDDETECTORVIEW_H

#include "ui/IDEAS/IDEASXRFDetailedDetectorView.h"
#include "beamline/IDEAS/IDEAS13ElementGeDetector.h"

class IDEAS13ElementGeDetailedDetectorView : public IDEASXRFDetailedDetectorView
{
	Q_OBJECT
public:
	/// Constructor
	IDEAS13ElementGeDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent = 0);

	/// Destructor
	virtual ~IDEAS13ElementGeDetailedDetectorView();

	/// Re-implementing to add peaking time spin box
	virtual void buildDetectorView();

public slots:


protected:
	/// Pointer to actual ketek detector for setting peaking time
	IDEAS13ElementGeDetector *ge13ElementDetector_;


};

#endif // G13ELEMENTDETAILEDDETECTORVIEW_H
