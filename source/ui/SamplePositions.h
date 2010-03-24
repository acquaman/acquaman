#ifndef SamplePositions_H
#define SamplePositions_H

#include "ui_SamplePositions.h"
#include "CamWidget.h"
#include <QUrl>


#define SAMPLEPOS_CAM_SOURCE1 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/1/video.mjpg"
//#define SAMPLEPOS_CAM_SOURCE1 "http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"
#define SAMPLEPOS_CAM_SOURCE2 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/2/video.mjpg"
#define SAMPLEPOS_CAM_SOURCE3 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/quad/video.mjpg"


class SamplePositions : public QWidget, private Ui::SamplePositions {
	
	Q_OBJECT
	
public:
	SamplePositions(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
		
		QUrl source1(SAMPLEPOS_CAM_SOURCE1);
		
		camWidget_ = new CamWidget("Camera 1", source1);
		
		QVBoxLayout* vl = new QVBoxLayout(videoFrameWidget);
		vl->addWidget(camWidget_);
		
		
		// add camera 2:
		source1.setUrl(SAMPLEPOS_CAM_SOURCE2);
		camWidget_->addSource("Camera 2", source1);
		
		// add camera 3:
		source1.setUrl(SAMPLEPOS_CAM_SOURCE3);
		camWidget_->addSource("Camera 3", source1);
		
		
	}
	
	
protected:
	CamWidget* camWidget_;
};

#endif
