#ifndef AMXRFDETAILEDDETECTORVIEW_H
#define AMXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFBaseDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/util/AMSelectableElementView.h"

class AMXRFDetailedDetectorView : public AMXRFBaseDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for AMXRFDetectors.
	AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

protected:
	/// The selectable periodic table model.
	AMSelectablePeriodicTable *periodicTable_;
	/// The selectable periodic table view.
	AMSelectablePeriodicTableView *periodicTableView_;
	/// The selectable element view.
	AMSelectableElementView *elementView_;
};

#endif // AMXRFDETAILEDDETECTORVIEW_H
