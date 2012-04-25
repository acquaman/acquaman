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


#include "SGMAddOnsCoordinator.h"
#include "beamline/AMPVControl.h"

SGMAddOnsCoordinator::SGMAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	SGMGratingControl_ = new AMPVControl("SGMOldGrating", "SG16114I1001:choice", "SG16114I1001:choice", QString(), this, 0.1);
	AddOnsGratingControl_ = new AMPVControl("SGMAddOnsGrating", "BL1611-ID-1:AddOns:grating", "BL1611-ID-1:AddOns:grating", QString(), this, 0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(SGMGratingControl_);
	allControls_->addControl(AddOnsGratingControl_);

	connect(SGMGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlChanged(double)));
	connect(AddOnsGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsGratingControlChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
}

void SGMAddOnsCoordinator::onSGMGratingControlChanged(double sgmGrating){
	if(!connectedOnce_)
		return;
	int gratingChoice = (int)sgmGrating;
	switch(gratingChoice){
	case 0:
		qDebug() << "Detected OLD grating UNKNOWN move to Automatic (0), failing in case 0?";
		break;
	case 1:
		if(!AddOnsGratingControl_->withinTolerance(2)){
			qDebug() << "Detected OLD grating move to High (1), must move AddOns to High (2)";
			AddOnsGratingControl_->move(2);
		}
		break;
	case 2:
		if(!AddOnsGratingControl_->withinTolerance(1)){
			qDebug() << "Detected OLD grating move to Medium (2), must move AddOns to Medium (1)";
			AddOnsGratingControl_->move(1);
		}
		break;
	case 3:
		if(!AddOnsGratingControl_->withinTolerance(0)){
			qDebug() << "Detected OLD grating move to Low (3), must move AddOns to Low (0)";
			AddOnsGratingControl_->move(0);
		}
		break;
	default:
		qDebug() << "Detected OLD grating UNKNOWN move, failing in default?";
		break;
	}
}

void SGMAddOnsCoordinator::onAddOnsGratingControlChanged(double addOnsGrating){
	if(!connectedOnce_)
		return;
	int gratingChoice = (int)addOnsGrating;
	switch(gratingChoice){
	case 0:
		if(!SGMGratingControl_->withinTolerance(3)){
			qDebug() << "Detected AddOns grating move to Low (0), must move OLD to Low (3)";
			SGMGratingControl_->move(3);
		}
		break;
	case 1:
		if(!SGMGratingControl_->withinTolerance(2)){
			qDebug() << "Detected AddOns grating move to Medium (1), must move OLD to Medium (2)";
			SGMGratingControl_->move(2);
		}
		break;
	case 2:
		if(!SGMGratingControl_->withinTolerance(1)){
			qDebug() << "Detected AddOns grating move to High (2), must move OLD to High (1)";
			SGMGratingControl_->move(1);
		}
		break;
	default:
		qDebug() << "Detected AddOns grating UNKNOWN move, failing in default?";
		break;
	}
}

void SGMAddOnsCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;
		qDebug() << "Checking start up value from the OLD grating as " << SGMGratingControl_->value();
		onSGMGratingControlChanged(SGMGratingControl_->value());
	}
}
