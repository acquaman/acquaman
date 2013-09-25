#ifndef AMXRFDETAILEDDETECTORVIEW_H
#define AMXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFBaseDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

class AMXRFDetailedDetectorView : public AMXRFBaseDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for AMXRFDetectors.
	AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
