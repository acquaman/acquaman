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


#include "SGMSidebar.h"

SGMSidebar::SGMSidebar(QWidget *parent) :
    QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	//vl_ = new QVBoxLayout();
	gl_ = new QGridLayout();
	//mainBox_->setLayout(vl_);
	mainBox_->setLayout(gl_);

	energyNC_ = new AMControlEdit(SGMBeamline::sgm()->energy(), SGMBeamline::sgm()->energyMovingStatus());
	energyNC_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	energyNC_->setMinimumWidth(150);
	gratingNC_ = new AMControlEdit(SGMBeamline::sgm()->grating());
	gratingNC_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	gratingNC_->setMinimumWidth(150);
	exitSlitNC_ = new AMControlEdit(SGMBeamline::sgm()->exitSlitGap());
	exitSlitNC_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	exitSlitNC_->setMinimumWidth(150);
	entranceSlitNC_ = new AMControlEdit(SGMBeamline::sgm()->entranceSlitGap());
	entranceSlitNC_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	entranceSlitNC_->setMinimumWidth(150);
	readyLabel_ = new AMControlEdit(SGMBeamline::sgm()->beamlineReady());
	readyLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	readyLabel_->setMinimumWidth(150);
//	moveStatusLabel_ = new AMControlEdit(SGMBeamline::sgm()->energyMovingStatus());
	/*
	vl_->addWidget(energyNC_);
	vl_->addWidget(gratingNC_);
	vl_->addWidget(exitSlitNC_);
	vl_->addWidget(entranceSlitNC_);
	vl_->addWidget(readyLabel_);
	vl_->addWidget(moveStatusLabel_);
	*/
	gl_->addWidget(readyLabel_,		0, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(energyNC_,		1, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(gratingNC_,		2, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(entranceSlitNC_,		3, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(exitSlitNC_,		3, 1, 1, 1, Qt::AlignCenter);

	setLayout(mainLayout_);
}
