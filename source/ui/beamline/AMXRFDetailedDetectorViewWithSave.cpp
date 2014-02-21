#include "AMXRFDetailedDetectorViewWithSave.h"
#include "actions3/actions/AMScanAction.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "beamline/VESPERS/XRFDetector.h"



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

    buildScanSaveViews();

}

void AMXRFDetailedDetectorViewWithSave::buildScanSaveViews()
{
    saveScanButton_ = new QPushButton("Save Scan");

    rightLayout_->addWidget(saveScanButton_);

    connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));
}

void AMXRFDetailedDetectorViewWithSave::onSaveScanButtonClicked()
{
    config_ = new IDEASXRFScanConfiguration(IDEASBeamline::ideas()->ketek()->toInfo());
    //config_.setName("This is the set name");
    AMAction3* action = new AMScanAction(new AMScanActionInfo(config_->createCopy()));
    action->start();
}


/*

  AMAction3* action = new AMScanAction(new AMScanActionInfo(view_->configuration()->createCopy()))
  action()->start;

  */
