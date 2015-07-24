#include "IDEAS13ElementGeDetailedDetectorView.h"

IDEAS13ElementGeDetailedDetectorView::IDEAS13ElementGeDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent)
    : IDEASXRFDetailedDetectorView(detector, parent)
{
	ge13ElementDetector_ = detector;
}

IDEAS13ElementGeDetailedDetectorView::~IDEAS13ElementGeDetailedDetectorView()
{

}

void IDEAS13ElementGeDetailedDetectorView::buildDetectorView()
{
	/// Call inherited parent classes buildDetectorView
    IDEASXRFDetailedDetectorView::buildDetectorView();
}
