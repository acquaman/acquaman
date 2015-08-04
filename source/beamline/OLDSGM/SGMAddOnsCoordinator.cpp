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


#include "SGMAddOnsCoordinator.h"

#include "beamline/AMPVControl.h"

 SGMAddOnsCoordinator::~SGMAddOnsCoordinator(){}
SGMAddOnsCoordinator::SGMAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	movingAddOnsExitSlitGap_ = false;
	sgmInitiatedMovement_ = false;
	addonsInitiatedMovement_ = false;

	SGMGratingControl_ = new AMPVControl("SGMOldGrating", "SG16114I1001:choice", "SG16114I1001:choice", QString(), this, 0.1);
	AddOnsGratingControl_ = new AMPVControl("SGMAddOnsGrating", "BL1611-ID-1:AddOns:grating", "BL1611-ID-1:AddOns:grating", QString(), this, 0.1);

	SGMHarmonicControl_ = new AMPVControl("SGMOldHarmonic", "BL1611-ID-1:harmonic", "BL1611-ID-1:harmonic", QString(), this, 0.1);
	AddOnsHarmonicControl_ = new AMPVControl("SGMAddOnsHarmonic", "BL1611-ID-1:AddOns:harmonic", "BL1611-ID-1:AddOns:harmonic", QString(), this, 0.1);

	SGMExitSlitGapControl_ = new AMPVControl("SGMOldExitSlitGap", "PSL16114I1004:Y:mm:encsp", "PSL16114I1004:Y:mm:encsp", QString(), this, 0.5);
	AddOnsExitSlitGapControl_ = new AMPVControl("SGMAddOnsExitSlitGap", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", QString(), this, 0.5);
	SGMExitSlitGapFeedbackControl_ = new AMReadOnlyPVControl("SGMExitSlitGapFeedback", "PSL16114I1004:Y:mm:fbk", this);
	SGMExitSlitGapFeedbackControl_->setTolerance(0.5);
	AddOnsExitSlitGapAddOnsMoving_ = new AMPVControl("SGMAddOnsExitSlitGapAddOnsMoving", "BL1611-ID-1:AddOns:ExitSlitGap:AddOnsMovingGap", "BL1611-ID-1:AddOns:ExitSlitGap:AddOnsMovingGap", QString(), this, 0.5);

	SGMExitSlitGapStatusControl_ = new AMPVControl("SGMOldExitSlitGapStatus", "SMTR16114I1017:status", "SMTR16114I1017:status", QString(), this, 0.1);
	AddOnsExitSlitGapStatusControl_ = new AMPVControl("AddOnsExitSlitGapStatus", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", QString(), this, 0.1);

	AddOnsExitSlitGapFullyOpenValueControl_ = new AMPVControl("AddOnsExitSlitGapFullyOpenValue", "BL1611-ID-1:AddOns:ExitSlitGap:FullyOpenValue", "BL1611-ID-1:AddOns:ExitSlitGap:FullyOpenValue", QString(), this, 1.0);

	AddOnsErrorCheckControl_ = new AMPVControl("AddOnsErrorCheck", "BL1611-ID-1:AddOns:ErrorCheck", "BL1611-ID-1:AddOns:ErrorCheck", QString(), this, 0.5);
	AddOnErrorStatusControl_ = new AMPVControl("AddOnsErrorStatus", "BL1611-ID-1:AddOns:Error", "BL1611-ID-1:AddOns:Error", QString(), this, 0.5);

	AddOnErrorSGMExitSlitStatusMDELControl_ = new AMReadOnlyPVControl("AddOnsErrorSGMExitSlitMDEL", "PSL16114I1004:Y:mm:encsp.MDEL");
	AddOnErrorSGMExitSlitStatusMDELControl_->setTolerance(0.5);
	AddOnErrorAddOnsExitSlitStatusMDELControl_ = new AMReadOnlyPVControl("AddOnsErrorAddOnsExitSlitMDEL", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm.MDEL");
	AddOnErrorAddOnsExitSlitStatusMDELControl_->setTolerance(0.5);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(SGMGratingControl_);
	allControls_->addControl(AddOnsGratingControl_);
	allControls_->addControl(SGMHarmonicControl_);
	allControls_->addControl(AddOnsHarmonicControl_);
	allControls_->addControl(SGMExitSlitGapControl_);
	allControls_->addControl(AddOnsExitSlitGapControl_);
	allControls_->addControl(SGMExitSlitGapFeedbackControl_);
	allControls_->addControl(SGMExitSlitGapStatusControl_);
	allControls_->addControl(AddOnsExitSlitGapStatusControl_);
	allControls_->addControl(AddOnsExitSlitGapFullyOpenValueControl_);
	allControls_->addControl(AddOnsErrorCheckControl_);
	allControls_->addControl(AddOnErrorStatusControl_);
	allControls_->addControl(AddOnErrorSGMExitSlitStatusMDELControl_);
	allControls_->addControl(AddOnErrorAddOnsExitSlitStatusMDELControl_);

	connect(SGMGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlChanged(double)));
	connect(AddOnsGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsGratingControlChanged(double)));

	connect(SGMHarmonicControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMHarmonicControlChanged(double)));
	connect(AddOnsHarmonicControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsHarmonicControlChanged(double)));

	connect(SGMExitSlitGapControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMExitSlitGapControlChanged(double)));
	connect(AddOnsExitSlitGapControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsExitSlitGapControlChanged(double)));
	connect(SGMExitSlitGapStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMExitSlitGapStatusControlChanged(double)));
	connect(AddOnsExitSlitGapAddOnsMoving_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsExitSlitGapAddOnsMoving(double)));

	connect(AddOnsErrorCheckControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsErrorCheckValueChanged(double)));

	connect(AddOnErrorSGMExitSlitStatusMDELControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnErrorSGMExitSlitStatusMDELValueChanged()));
	connect(AddOnErrorAddOnsExitSlitStatusMDELControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnErrorAddOnsExitSlitStatusMDELValueChanged()));

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

	checkSGMGratingErrors();
	checkErrors();
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

	checkAddOnsGratingErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::onSGMHarmonicControlChanged(double sgmHarmonic){
	if(!connectedOnce_)
		return;
	int harmonicChoice = (int)sgmHarmonic;
	switch(harmonicChoice){
	case 0:
		qDebug() << "Detected OLD harmonic UNKNOWN move to 0, failing in case 0?";
		break;
	case 1:
		if(!AddOnsHarmonicControl_->withinTolerance(0)){
			qDebug() << "Detected OLD harmonic move to First (1), must move AddOns to First (0)";
			AddOnsHarmonicControl_->move(0);
		}
		break;
	case 2:
		qDebug() << "Detected OLD harmonic UNKNOWN move to 2, failing in case 2?";
		break;
	case 3:
		if(!AddOnsHarmonicControl_->withinTolerance(1)){
			qDebug() << "Detected OLD harmonic move to Third (3), must move AddOns to Third (1)";
			AddOnsHarmonicControl_->move(1);
		}
		break;
	default:
		qDebug() << "Detected OLD harmonic UNKNOWN move, failing in default?";
		break;
	}

	checkSGMHarmonicErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::onAddOnsHarmonicControlChanged(double addOnsHarmonic){
	if(!connectedOnce_)
		return;
	int harmonicChoice = (int)addOnsHarmonic;
	switch(harmonicChoice){
	case 0:
		if(!SGMHarmonicControl_->withinTolerance(1)){
			qDebug() << "Detected AddOns harmonic move to First (0), must move OLD to First (1)";
			SGMHarmonicControl_->move(1);
		}
		break;
	case 1:
		if(!SGMHarmonicControl_->withinTolerance(3)){
			qDebug() << "Detected AddOns harmonic move to Third (1), must move OLD to Third (3)";
			SGMHarmonicControl_->move(3);
		}
		break;
	default:
		qDebug() << "Detected AddOns harmonic UNKNOWN move, failing in default?";
		break;
	}

	checkAddOnsHarmonicErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::onSGMExitSlitGapControlChanged(double sgmExitSlitGap){
	Q_UNUSED(sgmExitSlitGap)
	if(!connectedOnce_)
		return;
	qDebug() << "Detected OLD exit slit gap move requested for " << SGMExitSlitGapControl_->value() << "versus " << AddOnsExitSlitGapControl_->value() << " Initiated SGM/AddOns " << sgmInitiatedMovement_ << addonsInitiatedMovement_;

	if(movingAddOnsExitSlitGap_)
		return;

	if(!addonsInitiatedMovement_){
		sgmInitiatedMovement_ = true;
		AddOnsExitSlitGapControl_->move(SGMExitSlitGapControl_->value());
	}
	else
		addonsInitiatedMovement_ = false;
}

void SGMAddOnsCoordinator::onAddOnsExitSlitGapControlChanged(double addOnsExitSlitGap){
	Q_UNUSED(addOnsExitSlitGap)
	if(!connectedOnce_)
		return;
	if(AddOnsExitSlitGapControl_->withinTolerance(-1)){
		qDebug() << "Ignoring startup value change for AddOns exit slit gap";
		return;
	}
	qDebug() << "Detected AddOns exit slit gap move requested for " << AddOnsExitSlitGapControl_->value() << " Initiated SGM/AddOns " << sgmInitiatedMovement_ << addonsInitiatedMovement_;

	if(!sgmInitiatedMovement_){
		addonsInitiatedMovement_ = true;
		if(AddOnsExitSlitGapControl_->value() < (SGMExitSlitGapControl_->value()+SGMExitSlitGapControl_->tolerance()) )
			SGMExitSlitGapControl_->move(AddOnsExitSlitGapControl_->value());
		else{
			qDebug() << "Needs two step exit slit gap process";
			movingAddOnsExitSlitGap_ = true;
			AddOnsExitSlitGapAddOnsMoving_->move(1);
			connect(SGMExitSlitGapFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlFullyOpened(double)));
			SGMExitSlitGapControl_->move(AddOnsExitSlitGapFullyOpenValueControl_->value());
		}
	}
	else
		sgmInitiatedMovement_ = false;
}

void SGMAddOnsCoordinator::onSGMExitSlitGapStatusControlChanged(double sgmExitSlitGapStatus){
	if(!connectedOnce_)
		return;
	int statusChoice = (int)sgmExitSlitGapStatus;
	switch(statusChoice){
	case 0:
		qDebug() << "Detected OLD exit slit gap status change to MOVE DONE (0), might change AddOns to MOVE DONE (0)";
		if(!movingAddOnsExitSlitGap_)
			AddOnsExitSlitGapStatusControl_->move(0);
		else{
			qDebug() << "Found a situation where we might want to double check the two step process";
			onSGMGratingControlFullyOpened(SGMExitSlitGapFeedbackControl_->value());
		}
		break;
	case 1:
		if(!AddOnsExitSlitGapStatusControl_->withinTolerance(1)){
			qDebug() << "Detected OLD exit slit gap status change to MOVE ACTIVE (1), must move AddOns to MOVE ACTIVE (1)";
			AddOnsExitSlitGapStatusControl_->move(1);
		}
		break;
	case 2:
		if(!AddOnsExitSlitGapStatusControl_->withinTolerance(2)){
			qDebug() << "Detected OLD exit slit gap status change to AT LIMIT (2), must move AddOns to AT LIMIT (2)";
			AddOnsExitSlitGapStatusControl_->move(2);
		}
		break;
	case 3:
		if(!AddOnsExitSlitGapStatusControl_->withinTolerance(3)){
			qDebug() << "Detected OLD exit slit gap status change to FORCED STOP (3), must move AddOns to FORCED STOP (3)";
			AddOnsExitSlitGapStatusControl_->move(3);
		}                
		break;
	case 4:
		if(!AddOnsExitSlitGapStatusControl_->withinTolerance(4)){
			qDebug() << "Detected OLD exit slit gap status change to ERROR (4), must move AddOns to ERROR (4)";
			AddOnsExitSlitGapStatusControl_->move(4);
		}                
		break;
	default:
		qDebug() << "Detected OLD exit slit gap status UNKNOWN move, failing in default?";
		break;
	}

	checkSGMExitSlitGapStatusErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::onAddOnsExitSlitGapAddOnsMoving(double addOnsExitSlotGapAddOnsMoving){
	Q_UNUSED(addOnsExitSlotGapAddOnsMoving)
	if(!connectedOnce_)
		return;

	if(AddOnsExitSlitGapAddOnsMoving_->withinTolerance(0) && movingAddOnsExitSlitGap_)
		movingAddOnsExitSlitGap_ = false;
	else if(AddOnsExitSlitGapAddOnsMoving_->withinTolerance(1) && !movingAddOnsExitSlitGap_)
		movingAddOnsExitSlitGap_ = true;
}

void SGMAddOnsCoordinator::onSGMGratingControlFullyOpened(double sgmExitSlitGap){
	qDebug() << "Listening to the exit slit gap feedback as " << sgmExitSlitGap << movingAddOnsExitSlitGap_ << SGMExitSlitGapStatusControl_->withinTolerance(0) << SGMExitSlitGapFeedbackControl_->withinTolerance(AddOnsExitSlitGapFullyOpenValueControl_->value());
	if(movingAddOnsExitSlitGap_ && SGMExitSlitGapStatusControl_->withinTolerance(0) && SGMExitSlitGapFeedbackControl_->withinTolerance(AddOnsExitSlitGapFullyOpenValueControl_->value())){
		qDebug() << "Got into second stage move";
		disconnect(SGMExitSlitGapFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlFullyOpened(double)));
		SGMExitSlitGapControl_->move(AddOnsExitSlitGapControl_->value());
		movingAddOnsExitSlitGap_ = false;
		AddOnsExitSlitGapAddOnsMoving_->move(0);
	}
}

void SGMAddOnsCoordinator::onAddOnsErrorCheckValueChanged(double errorCheck){
	Q_UNUSED(errorCheck)

	if(!connectedOnce_)
		return;

	if(AddOnsErrorCheckControl_->withinTolerance(1.0)){
		qDebug() << "Forcing a check for errors";

		checkSGMGratingErrors();
		checkAddOnsGratingErrors();
		checkSGMExitSlitGapStatusErrors();

		checkSGMExitSlitGapMDELErrors();
		checkAddOnsExitSlitGapMDELErrors();

		checkErrors(true);
		AddOnsErrorCheckControl_->move(0.0);
	}
}

void SGMAddOnsCoordinator::onAddOnErrorSGMExitSlitStatusMDELValueChanged(){
	if(!connectedOnce_)
		return;

	checkSGMExitSlitGapMDELErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::onAddOnErrorAddOnsExitSlitStatusMDELValueChanged(){
	if(!connectedOnce_)
		return;

	checkAddOnsExitSlitGapMDELErrors();
	checkErrors();
}

void SGMAddOnsCoordinator::checkErrors(bool forceCheck){
	bool errorStatusChanged = false;
	if(AddOnErrorStatusControl_->withinTolerance(0.0) && errorsList_.count() != 0){
		AddOnErrorStatusControl_->move(1.0);
		errorStatusChanged = true;
	}
	else if(AddOnErrorStatusControl_->withinTolerance(1.0) && errorsList_.count() == 0){
		AddOnErrorStatusControl_->move(0.0);
		errorStatusChanged = true;
	}

	if(errorStatusChanged || forceCheck){
		if(errorsList_.count() != 0){
			qDebug() << "\n\nErrors detected: ";
			for(int x = 0; x < errorsList_.count(); x++)
				qDebug() << errorsList_.at(x);
			qDebug() << "\n\n";
		}
		else{
			qDebug() << "\n\nNo errors detected\n\n";
		}
	}
}

void SGMAddOnsCoordinator::checkSGMGratingErrors(){
	int gratingChoice = (int)(SGMGratingControl_->value());

	if((gratingChoice == 1) || (gratingChoice == 2) || (gratingChoice == 3)){
		if(errorsList_.contains("Old grating to Automatic (0) move ENUM error"))
			errorsList_.removeAll("Old grating to Automatic (0) move ENUM error");
		if(errorsList_.contains("Old grating unknown move ENUM error"))
			errorsList_.removeAll("Old grating unknown move ENUM error");
	}
	else if(gratingChoice == 0){
		if(!errorsList_.contains("Old grating to Automatic (0) move ENUM error"))
			errorsList_.append("Old grating to Automatic (0) move ENUM error");
	}
	else if(!errorsList_.contains("Old grating unknown move ENUM error")){
		errorsList_.append("Old grating unknown move ENUM error");
	}
}

void SGMAddOnsCoordinator::checkAddOnsGratingErrors(){
	int gratingChoice = (int)(AddOnsGratingControl_->value());

	if((gratingChoice == 0) || (gratingChoice == 1) || (gratingChoice == 2)){
		if(errorsList_.contains("AddOns grating unknown move ENUM error"))
			errorsList_.removeAll("AddOns grating unknown move ENUM error");
	}
	else if(!errorsList_.contains("AddOns grating unknown move ENUM error")){
		errorsList_.append("AddOns grating unknown move ENUM error");
	}
}

void SGMAddOnsCoordinator::checkSGMHarmonicErrors(){
	int harmonicChoice = (int)(SGMHarmonicControl_->value());

	if((harmonicChoice == 1) || (harmonicChoice == 3)){
		if(errorsList_.contains("Old harmonic unknown move ENUM error"))
			errorsList_.removeAll("Old harmonic unknown move ENUM error");
	}
	else if(!errorsList_.contains("Old harmonic unknown move ENUM error")){
		errorsList_.append("Old harmonic unknown move ENUM error");
	}
}

void SGMAddOnsCoordinator::checkAddOnsHarmonicErrors(){
	int harmonicChoice = (int)(AddOnsHarmonicControl_->value());

	if((harmonicChoice == 0) || (harmonicChoice == 1)){
		if(errorsList_.contains("AddOns harmonic unknown move ENUM error"))
			errorsList_.removeAll("AddOns harmonic unknown move ENUM error");
	}
	else if(!errorsList_.contains("AddOns harmonic unknown move ENUM error")){
		errorsList_.append("AddOns harmonic unknown move ENUM error");
	}
}

void SGMAddOnsCoordinator::checkSGMExitSlitGapStatusErrors(){
	int statusChoice = (int)(AddOnsExitSlitGapStatusControl_->value());

	if((statusChoice == 0) || (statusChoice == 1) || (statusChoice == 2) || (statusChoice == 3) || (statusChoice == 4)){
		if(errorsList_.contains("Old exit slit gap unknown move ENUM error"))
			errorsList_.removeAll("Old exit slit gap unknown move ENUM error");
	}
	else if(!errorsList_.contains("Old exit slit gap unknown move ENUM error")){
		errorsList_.append("Old exit slit gap unknown move ENUM error");
	}
}

void SGMAddOnsCoordinator::checkSGMExitSlitGapMDELErrors(){
	if(!AddOnErrorSGMExitSlitStatusMDELControl_->withinTolerance(-1.0) && !errorsList_.contains("Bad MDEL value for PSL16114I1004:y:mm:encsp"))
		errorsList_.append("Bad MDEL value for PSL16114I1004:y:mm:encsp");
	else if(AddOnErrorSGMExitSlitStatusMDELControl_->withinTolerance(-1.0) && errorsList_.contains("Bad MDEL value for PSL16114I1004:y:mm:encsp"))
		errorsList_.removeAll("Bad MDEL value for PSL16114I1004:y:mm:encsp");
}

void SGMAddOnsCoordinator::checkAddOnsExitSlitGapMDELErrors(){
	if(!AddOnErrorAddOnsExitSlitStatusMDELControl_->withinTolerance(-1.0) && !errorsList_.contains("Bad MDEL value for BL1611-ID-1:AddOns:ExitSlitGap:Y:mm"))
		errorsList_.append("Bad MDEL value for BL1611-ID-1:AddOns:ExitSlitGap:Y:mm");
	else if(AddOnErrorAddOnsExitSlitStatusMDELControl_->withinTolerance(-1.0) && errorsList_.contains("Bad MDEL value for BL1611-ID-1:AddOns:ExitSlitGap:Y:mm"))
		errorsList_.removeAll("Bad MDEL value for BL1611-ID-1:AddOns:ExitSlitGap:Y:mm");
}

void SGMAddOnsCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;
		qDebug() << "Checking start up value from the OLD grating as " << SGMGratingControl_->value();
		onSGMGratingControlChanged(SGMGratingControl_->value());
		qDebug() << "Checking start up value from the OLD harmonic as " << SGMHarmonicControl_->value();
		onSGMHarmonicControlChanged(SGMHarmonicControl_->value());
		qDebug() << "Checking start up value from OLD exit slit gap as " << SGMExitSlitGapControl_->value();
		sgmInitiatedMovement_ = true;
		AddOnsExitSlitGapControl_->move(SGMExitSlitGapControl_->value());
		qDebug() << "Checking start up value from OLD exit slit gap status as " << SGMExitSlitGapStatusControl_->value();
		onSGMExitSlitGapStatusControlChanged(SGMExitSlitGapStatusControl_->value());

		AddOnsErrorCheckControl_->move(1.0);
	}
}
