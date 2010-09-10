#ifndef ACQMAN_SamplePositions_H
#define ACQMAN_SamplePositions_H

#include "ui_SamplePositions.h"
#include "ui/AMBeamlineCameraWidget.h"
#include <QUrl>


// TODO: move to global settings section...

//#define SAMPLEPOS_CAM_SOURCE1 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/1/video.mjpg"

#define SAMPLEPOS_CAM_SOURCE1 "http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"

#define SAMPLEPOS_CAM_SOURCE2 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/2/video.mjpg"
#define SAMPLEPOS_CAM_SOURCE3 "http://reixs:Welcome1@v2e1610-101.cs.clsi.ca/mjpg/quad/video.mjpg"


// temporary: prevent waiting a long time trying to connect when Mark's at home:
#define SAMPLEPOS_CAM_SOURCE1 "notfound"



class SamplePositions : public QWidget, private Ui::SamplePositions {

	Q_OBJECT

public:
	SamplePositions(QWidget* parent = 0);


protected:
	AMBeamlineCameraWidget* camWidget_;
};

#endif
