#ifndef AMXRFDETAILEDDETECTORVIEWWITHSAVE_H
#define AMXRFDETAILEDDETECTORVIEWWITHSAVE_H

#include "ui/beamline/AMXRFBaseDetectorView.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"
#include "ui/util/AMSelectableElementView.h"
#include "util/AMNameAndRangeValidator.h"

#include <QSignalMapper>

class AMXRFDetailedDetectorViewWithSave : public AMXRFDetailedDetectorView
{
    Q_OBJECT
public:
    explicit AMXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent = 0);

    /// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
    virtual void buildDetectorView();


signals:

public slots:

};

#endif // AMXRFDETAILEDDETECTORVIEWWITHSAVE_H
