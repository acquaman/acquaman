/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSBeamSelectorView.h"

#include "actions3/AMAction3.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

 VESPERSBeamSelectorView::~VESPERSBeamSelectorView(){}
VESPERSBeamSelectorView::VESPERSBeamSelectorView(QWidget *parent)
	: QWidget(parent)
{
	currentBeam_ = 0;

	beams_ = new QButtonGroup;

	QToolButton *temp = new QToolButton;
	temp->setText("Pink");
	temp->setFixedSize(35, 22);
	temp->setCheckable(true);
	beams_->addButton(temp, 0);

	temp = new QToolButton;
	temp->setText("10%");
	temp->setFixedSize(35, 22);
	temp->setCheckable(true);
	beams_->addButton(temp, 1);

	temp = new QToolButton;
	temp->setText("1.6%");
	temp->setFixedSize(35, 22);
	temp->setCheckable(true);
	beams_->addButton(temp, 2);

	temp = new QToolButton;
	temp->setText("Si");
	temp->setFixedSize(35, 22);
	temp->setCheckable(true);
	beams_->addButton(temp, 3);

	connect(beams_, SIGNAL(buttonClicked(int)), this, SLOT(changeBeam(int)));
	connect(VESPERSBeamline::vespers(), SIGNAL(currentBeamChanged(VESPERS::Beam)), this, SLOT(onCurrentBeamChanged(VESPERS::Beam)));

	progressBar_ = new QProgressBar;
	progressBar_->hide();
	progressBar_->setRange(0, 100);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(beams_->button(0));
	buttonsLayout->addWidget(beams_->button(1));
	buttonsLayout->addWidget(beams_->button(2));
	buttonsLayout->addWidget(beams_->button(3));

	QVBoxLayout *beamSelectorLayout = new QVBoxLayout;
	beamSelectorLayout->addLayout(buttonsLayout);
	beamSelectorLayout->addWidget(progressBar_);

	setLayout(beamSelectorLayout);
}

void VESPERSBeamSelectorView::changeBeam(int id)
{
	AMAction3 *action = 0;

	switch(id){

	case 0:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERS::Pink);
		break;

	case 1:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERS::TenPercent);
		break;

	case 2:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERS::OnePointSixPercent);
		break;

	case 3:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERS::Si);
		break;
	}

	if (!action)
		return;

	progressBar_->setRange(0, 0);
	progressBar_->show();
	connect(action, SIGNAL(cancelled()), this, SLOT(onBeamChangeCompleted()));
	connect(action, SIGNAL(failed()), this, SLOT(onBeamChangeCompleted()));
	connect(action, SIGNAL(succeeded()), this, SLOT(onBeamChangeCompleted()));
	connect(action, SIGNAL(cancelled()), action, SLOT(deleteLater()));
	connect(action, SIGNAL(failed()), action, SLOT(deleteLater()));
	connect(action, SIGNAL(succeeded()), action, SLOT(deleteLater()));
	action->start();
}

void VESPERSBeamSelectorView::onCurrentBeamChanged(VESPERS::Beam beam)
{
	if (currentBeam_)
		currentBeam_->setPalette(this->palette());

	switch(beam){

	case VESPERS::Pink:
		currentBeam_ = beams_->button(0);
		break;

	case VESPERS::TenPercent:
		currentBeam_ = beams_->button(1);
		break;

	case VESPERS::OnePointSixPercent:
		currentBeam_ = beams_->button(2);
		break;

	case VESPERS::Si:
		currentBeam_ = beams_->button(3);
		break;

	case VESPERS::NoBeam:
		currentBeam_ = 0;
		break;
	}

	if (currentBeam_)
		currentBeam_->setPalette(Qt::green);
}
