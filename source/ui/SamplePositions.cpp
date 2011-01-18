/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "SamplePositions.h"

SamplePositions::SamplePositions(QWidget* parent) : QWidget(parent) {

	setupUi(this);

	QUrl source1(SAMPLEPOS_CAM_SOURCE1);

	camWidget_ = new AMBeamlineCameraWidget("Camera 1", source1);

	QVBoxLayout* vl = new QVBoxLayout(videoFrameWidget);
	vl->addWidget(camWidget_);


	// add camera 2:
	QUrl source2(SAMPLEPOS_CAM_SOURCE2);
	camWidget_->addSource("Camera Two", source2);

	// add camera 3:
	QUrl source3(SAMPLEPOS_CAM_SOURCE3);
	camWidget_->addSource("Camera Tree", source3);


}
