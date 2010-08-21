#include "AMSamplePositionView.h"

AMSamplePositionView::AMSamplePositionView(QWidget *parent, AMSampleManipulatorView *manipulatorView) :
	QWidget(parent)
{
	QUrl source("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg");
	cam_ = new CamWidget("Camera 1", source);

	plateView_ = new AMSamplePlateView();
	plateView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	if(!manipulatorView)
		manipulatorView_ = new AMSampleManipulatorView();
	else
		manipulatorView_ = manipulatorView;
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
