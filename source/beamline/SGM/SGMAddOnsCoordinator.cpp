#include "SGMAddOnsCoordinator.h"

#include "beamline/AMPVControl.h"

SGMAddOnsCoordinator::SGMAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	movingAddOnsExitSlitGap_ = false;

	SGMGratingControl_ = new AMPVControl("SGMOldGrating", "SG16114I1001:choice", "SG16114I1001:choice", QString(), this, 0.1);
	AddOnsGratingControl_ = new AMPVControl("SGMAddOnsGrating", "BL1611-ID-1:AddOns:grating", "BL1611-ID-1:AddOns:grating", QString(), this, 0.1);

	SGMExitSlitGapControl_ = new AMPVControl("SGMOldExitSlitGap", "PSL16114I1004:Y:mm:encsp", "PSL16114I1004:Y:mm:encsp", QString(), this, 0.5);
	AddOnsExitSlitGapControl_ = new AMPVControl("SGMAddOnsExitSlitGap", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", QString(), this, 0.5);
	SGMExitSlitGapFeedbackControl_ = new AMReadOnlyPVControl("SGMExitSlitGapFeedback", "PSL16114I1004:Y:mm:fbk", this);
	SGMExitSlitGapFeedbackControl_->setTolerance(0.5);

	SGMExitSlitGapStatusControl_ = new AMPVControl("SGMOldExitSlitGapStatus", "SMTR16114I1017:status", "SMTR16114I1017:status", QString(), this, 0.1);
	AddOnsExitSlitGapStatusControl_ = new AMPVControl("AddOnsExitSlitGapStatus", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", QString(), this, 0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(SGMGratingControl_);
	allControls_->addControl(AddOnsGratingControl_);
	allControls_->addControl(SGMExitSlitGapControl_);
	allControls_->addControl(AddOnsExitSlitGapControl_);
	allControls_->addControl(SGMExitSlitGapFeedbackControl_);
	allControls_->addControl(SGMExitSlitGapStatusControl_);
	allControls_->addControl(AddOnsExitSlitGapStatusControl_);

	connect(SGMGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlChanged(double)));
	connect(AddOnsGratingControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsGratingControlChanged(double)));

	connect(SGMExitSlitGapControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMExitSlitGapControlChanged(double)));
	connect(AddOnsExitSlitGapControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsExitSlitGapControlChanged(double)));
	connect(SGMExitSlitGapStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMExitSlitGapStatusControlChanged(double)));

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

void SGMAddOnsCoordinator::onSGMExitSlitGapControlChanged(double sgmExitSlitGap){
	Q_UNUSED(sgmExitSlitGap)
	if(!connectedOnce_)
		return;
	qDebug() << "Detected OLD exit slit gap move requested for " << SGMExitSlitGapControl_->value() << "versus " << AddOnsExitSlitGapControl_->value();
	if(!movingAddOnsExitSlitGap_ && !AddOnsExitSlitGapControl_->withinTolerance(SGMExitSlitGapControl_->value()))
		AddOnsExitSlitGapControl_->move(SGMExitSlitGapControl_->value());
}

void SGMAddOnsCoordinator::onAddOnsExitSlitGapControlChanged(double addOnsExitSlitGap){
	Q_UNUSED(addOnsExitSlitGap)
	if(!connectedOnce_)
		return;
	if(AddOnsExitSlitGapControl_->withinTolerance(-1)){
		qDebug() << "Ignoring startup value change for AddOns exit slit gap";
		return;
	}
	qDebug() << "Detected AddOns exit slit gap move requested for " << AddOnsExitSlitGapControl_->value();
	if(!SGMExitSlitGapControl_->withinTolerance(AddOnsExitSlitGapControl_->value())){
		if(AddOnsExitSlitGapControl_->value() < SGMExitSlitGapControl_->value())
			SGMExitSlitGapControl_->move(AddOnsExitSlitGapControl_->value());
		else{
			qDebug() << "Needs two step exit slit gap process";
			movingAddOnsExitSlitGap_ = true;
			connect(SGMExitSlitGapFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlFullyOpened(double)));
			SGMExitSlitGapControl_->move(350);
		}
	}
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
}

void SGMAddOnsCoordinator::onSGMGratingControlFullyOpened(double sgmExitSlitGap){
	qDebug() << "Listening to the exit slit gap feedback as " << sgmExitSlitGap << movingAddOnsExitSlitGap_ << SGMExitSlitGapStatusControl_->withinTolerance(0) << SGMExitSlitGapFeedbackControl_->withinTolerance(350);
	if(movingAddOnsExitSlitGap_ && SGMExitSlitGapStatusControl_->withinTolerance(0) && SGMExitSlitGapFeedbackControl_->withinTolerance(350)){
		qDebug() << "Got into second stage move";
		disconnect(SGMExitSlitGapFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onSGMGratingControlFullyOpened(double)));
		SGMExitSlitGapControl_->move(AddOnsExitSlitGapControl_->value());
		movingAddOnsExitSlitGap_ = false;
	}
}

void SGMAddOnsCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;
		qDebug() << "Checking start up value from the OLD grating as " << SGMGratingControl_->value();
		onSGMGratingControlChanged(SGMGratingControl_->value());
		qDebug() << "Checking start up value from OLD exit slit gap as " << SGMExitSlitGapControl_->value();
		onSGMExitSlitGapControlChanged(SGMExitSlitGapControl_->value());
		qDebug() << "Checking start up value from OLD exit slit gap status as " << SGMExitSlitGapStatusControl_->value();
		onSGMExitSlitGapStatusControlChanged(SGMExitSlitGapStatusControl_->value());
	}
}
