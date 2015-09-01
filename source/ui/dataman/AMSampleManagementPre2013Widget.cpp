/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMSampleManagementPre2013Widget.h"

#include <QGridLayout>
#include <QUrl>
#include <QGroupBox>

#include "ui/AMTopFrame.h"

#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"

AMSampleManagementPre2013Widget::AMSampleManagementPre2013Widget(QWidget *manipulatorWidget, const QUrl& sampleCameraUrl, const QString& sampleCameraDescription, AMSamplePlatePre2013* samplePlate, AMSampleManipulator* manipulator, QWidget *parent) :
	QWidget(parent)
{
#ifdef AM_MOBILITY_VIDEO_ENABLED
	cameraWidget_ = new AMBeamlineCameraWidgetWithSourceTabs(sampleCameraUrl, sampleCameraDescription);
	cameraWidget_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
#else
	Q_UNUSED(sampleCameraUrl);
	Q_UNUSED(sampleCameraDescription);
#endif

	topFrame_ = new AMTopFrame("Sample Management & Positioning", QIcon(":/system-software-update.png"));

	plateView_ = new AMSamplePlatePre2013View(samplePlate);
	plateView_->setManipulator(manipulator);

	manipulatorWidget_ = manipulatorWidget;

	connect(plateView_, SIGNAL(newSamplePlateSelected()), this, SLOT(onNewSamplePlateSelected()));

	QVBoxLayout *vl = new QVBoxLayout();
	QGridLayout* gl = new QGridLayout();
	vl->addWidget(topFrame_);
	vl->addLayout(gl);

#ifdef AM_MOBILITY_VIDEO_ENABLED
	gl->addWidget(cameraWidget_, 0, 0, 1, 1);
#endif
	gl->addWidget(plateView_, 0, 1, 2, 1);
	gl->addWidget(manipulatorWidget_, 1, 0, 1, 1);
	gl->setColumnStretch(0,1);
	gl->setColumnStretch(1,0);

	vl->setContentsMargins(0,0,0,0);
	gl->setContentsMargins(10, 0, 10, 0);

	setLayout(vl);
}

AMSampleManagementPre2013Widget::~AMSampleManagementPre2013Widget(){}

void AMSampleManagementPre2013Widget::onNewSamplePlateSelected() {
	emit newSamplePlateSelected(plateView_->samplePlate());
}
