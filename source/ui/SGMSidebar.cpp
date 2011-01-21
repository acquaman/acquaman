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
	vl_ = new QVBoxLayout();
	mainBox_->setLayout(vl_);

	energyNC_ = new AMControlEdit(SGMBeamline::sgm()->energy());
	gratingNC_ = new AMControlEdit(SGMBeamline::sgm()->grating());
	readyLabel_ = new AMControlEdit(SGMBeamline::sgm()->beamlineReady());
	moveStatusLabel_ = new AMControlEdit(SGMBeamline::sgm()->energyMovingStatus());
	vl_->addWidget(energyNC_);
	vl_->addWidget(gratingNC_);
	vl_->addWidget(readyLabel_);
	vl_->addWidget(moveStatusLabel_);

	setLayout(mainLayout_);
}
