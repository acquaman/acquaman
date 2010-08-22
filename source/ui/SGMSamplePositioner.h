#ifndef SGMSAMPLEPOSITIONER_H
#define SGMSAMPLEPOSITIONER_H

#include <QWidget>
#include "ui/AMCameraWidget.h"
#include <QGridLayout>
#include <QUrl>

class SGMSamplePositioner : public QWidget
{
Q_OBJECT
public:
	explicit SGMSamplePositioner(QWidget *parent = 0) : QWidget(parent){
		mainLayout_ = new QGridLayout();
		QUrl source("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg");
		camera_ = new AMCameraWidget("Camera 1", source);
		mainLayout_->addWidget(camera_, 0, 0, 1, 1, Qt::AlignCenter);
		setLayout(mainLayout_);
	}

signals:

public slots:

protected:
	AMCameraWidget* camera_;
	QGridLayout* mainLayout_;
};

#endif // SGMSAMPLEPOSITIONER_H
