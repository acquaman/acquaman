/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
