#include "AMSamplePositionView.h"

AMSamplePositionView::AMSamplePositionView(QWidget *parent, AMSampleManipulatorView *manipulatorView) :
	QWidget(parent)
{
	/// \todo temporary. (find a place to get this location from)
	QUrl source("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg");
	QUrl source2("/Users/mboots/Desktop/japan2010/devin/P8100682.MP4");
	cam_ = new AMBeamlineCameraWidget("Camera 1", source);
	cam_->addSource("Camera 2", source2);

	manipulator_ = NULL;
	/// \todo move this out of AM framework level... (or move currentSamplePlate() up to AM framework level)
	plateView_ = new AMSamplePlateView(SGMBeamline::sgm()->currentSamplePlate());
	plateView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	if(!manipulatorView)
		manipulatorView_ = new AMSampleManipulatorView();
	else
		manipulatorView_ = manipulatorView;
	if(manipulatorView_->manipulator()){
		manipulator_ = manipulatorView_->manipulator();
		plateView_->setManipulator(manipulator_);
	}

	manipulatorView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	gl_ = new QGridLayout();
	gl_->addWidget(cam_, 0, 0, 3, 1, Qt::AlignLeft);
	gl_->addWidget(plateView_, 0, 1, 5, 1, Qt::AlignLeft);
	gl_->addWidget(manipulatorView_, 3, 0, 2, 1, Qt::AlignLeft);

	setLayout(gl_);
}

SGMSamplePositionView::SGMSamplePositionView(QWidget *parent) :
		AMSamplePositionView(parent, new SGMSampleManipulatorView() )
{

}
