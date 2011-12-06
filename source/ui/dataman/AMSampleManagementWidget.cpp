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


#include "AMSampleManagementWidget.h"

#include <QGridLayout>
#include <QUrl>
#include <QGroupBox>

#include "ui/AMTopFrame.h"

AMSampleManagementWidget::AMSampleManagementWidget(QWidget *manipulatorWidget, const QUrl& sampleCameraUrl, AMSamplePlate* samplePlate, AMSampleManipulator* manipulator, QWidget *parent) :
	QWidget(parent)
{
	Q_UNUSED(sampleCameraUrl);
	// cam_ = new AMBeamlineCameraWidget("Sample Camera", sampleCameraUrl);
	// cam_->addSource("Camera 2", source2);

	topFrame_ = new AMTopFrame("Sample Management & Positioning");
	topFrame_->setIcon(QIcon(":/system-software-update.png"));

	plateView_ = new AMSamplePlateView(samplePlate);
	plateView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	plateView_->setManipulator(manipulator);

	manipulatorWidget_ = manipulatorWidget;
	manipulatorWidget_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	connect(plateView_, SIGNAL(newSamplePlateSelected()), this, SLOT(onNewSamplePlateSelected()));

	gl_ = new QGridLayout();
	//gl_->addWidget(cam_, 0, 0, 3, 1, Qt::AlignLeft);
	gl_->addWidget(plateView_, 0, 1, 5, 1, Qt::AlignLeft);
	gl_->addWidget(manipulatorWidget_, 3, 0, 2, 1, Qt::AlignLeft);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(topFrame_);
	vl->addLayout(gl_);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(1);
	gl_->setContentsMargins(10, 0, 10, 0);

	setLayout(vl);
}

void AMSampleManagementWidget::onNewSamplePlateSelected() {
	// qDebug() << "I heard that the current sample plate changed";
	emit newSamplePlateSelected(plateView_->samplePlate());
}
