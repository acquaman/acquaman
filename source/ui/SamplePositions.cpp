#include "SamplePositions.h"

SamplePositions::SamplePositions(QWidget* parent) : QWidget(parent) {

	setupUi(this);

	QUrl source1(SAMPLEPOS_CAM_SOURCE1);

	camWidget_ = new CamWidget("Camera 1", source1);

	QVBoxLayout* vl = new QVBoxLayout(videoFrameWidget);
	vl->addWidget(camWidget_);


	// add camera 2:
	QUrl source2(SAMPLEPOS_CAM_SOURCE2);
	camWidget_->addSource("Camera Two", source2);

	// add camera 3:
	QUrl source3(SAMPLEPOS_CAM_SOURCE3);
	camWidget_->addSource("Camera Tree", source3);


}
