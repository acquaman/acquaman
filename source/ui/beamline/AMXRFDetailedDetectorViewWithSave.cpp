#include "AMXRFDetailedDetectorViewWithSave.h"

AMXRFDetailedDetectorViewWithSave::AMXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent) :
    AMXRFDetailedDetectorView(detector, parent)
{
}

void AMXRFDetailedDetectorViewWithSave::buildDetectorView()
{
    // This is going to extend the base view, so we will simply call it's build method too.
    AMXRFBaseDetectorView::buildDetectorView();

    AMXRFDetailedDetectorView::buildPeriodicTableViewAndElementView();
    AMXRFDetailedDetectorView::buildShowSpectraButtons();
    AMXRFDetailedDetectorView::buildPileUpPeakButtons();
    AMXRFDetailedDetectorView::buildDeadTimeView();
    AMXRFDetailedDetectorView::buildRegionOfInterestViews();

}
