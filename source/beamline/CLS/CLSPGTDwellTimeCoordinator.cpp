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


#include "CLSPGTDwellTimeCoordinator.h"
#include "beamline/AMPVControl.h"

 CLSPGTDwellTimeCoordinator::~CLSPGTDwellTimeCoordinator(){}
CLSPGTDwellTimeCoordinator::CLSPGTDwellTimeCoordinator(const QString &PGTStubName, const QString &beamlineStubName, QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	internalRealTimeSet_ = false;
	internalLiveTimeSet_ = false;
	internalPeakTimeSet_ = false;
	internalDwellTimeSet_ = false;
	internalDwellModeSet_ = false;
	dwellMode_ = 3;
	dwellTime_ = 0;
	internalDwellTrigger_ = false;
	internalDwellTriggerIgnoreZero_ = false;

	realTimeControl_ = new AMPVControl("PGTRealTime", PGTStubName+":Preset:Real", "MCA1611-01:Preset:Real", "", this, 0.1);
	liveTimeControl_ = new AMPVControl("PGTLiveTime", PGTStubName+":Preset:Live", "MCA1611-01:Preset:Live", "", this, 0.1);
	peakTimeControl_ = new AMPVControl("PGTPeakTime", PGTStubName+":Preset:Peak", "MCA1611-01:Preset:Peak", "", this, 0.1);
        dwellTimeControl_ = new AMPVControl("PGTDwellTime", beamlineStubName+":AddOns:PGTDwellTime", "BL1611-ID-1:AddOns:PGTDwellTime", "", this, 0.1);
        dwellModeControl_ = new AMPVControl("PGTDwellMode", beamlineStubName+":AddOns:PGTDwellMode", "BL1611-ID-1:AddOns:PGTDwellMode", "", this, 0.1);
	dwellTriggerControl_ = new AMPVControl("PGTDwellTrigger", beamlineStubName+":AddOns:PGTDwellTrigger", beamlineStubName+":AddOns:PGTDwellTrigger", "", this, 0.1);
	oldTriggerControl_ = new AMPVControl("PGTOldTrigger", PGTStubName+":StartAcquisition.PROC", PGTStubName+":StartAcquisition.PROC", "", this, 0.1);
	oldROI0Count_ = new AMPVControl("PGTOldROI0", PGTStubName+":ROI:0", PGTStubName+":ROI:0", "", this, 0.1);
	oldClearSpectrumControl_ = new AMPVControl("PGTOldClearSpectrum", PGTStubName+":ClearSpectrum.PROC", PGTStubName+":ClearSpectrum.PROC", "", this, 0.1);
	oldAcquiringControl_ = new AMPVControl("PGTOldAcquiring", PGTStubName+":GetAcquire", PGTStubName+":GetAcquire", "", this, 0.1);
	oldElapsedEnableControl_ = new AMPVControl("PGTOldElapsedEnable", PGTStubName+":Preset", PGTStubName+":Preset", "", this, 0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(realTimeControl_);
	allControls_->addControl(liveTimeControl_);
	allControls_->addControl(peakTimeControl_);
	allControls_->addControl(dwellTimeControl_);
	allControls_->addControl(dwellModeControl_);
	allControls_->addControl(dwellTriggerControl_);
	allControls_->addControl(oldTriggerControl_);
	allControls_->addControl(oldROI0Count_);
	allControls_->addControl(oldClearSpectrumControl_);
	allControls_->addControl(oldAcquiringControl_);
	allControls_->addControl(oldElapsedEnableControl_);

	connect(realTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onRealTimeControlChanged(double)));
	connect(liveTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onLiveTimeControlChanged(double)));
	connect(peakTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onPeakTimeControlChanged(double)));
	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlChanged(double)));
	connect(dwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeModeChanged(double)));
	connect(dwellTriggerControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTriggerControlChanged(double)));
	connect(oldAcquiringControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAcquiringControlChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
}

void CLSPGTDwellTimeCoordinator::onRealTimeControlChanged(double realTime){
	Q_UNUSED(realTime);
	if(!connectedOnce_ || internalRealTimeSet_){
		internalRealTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	qDebug() << "Real time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onLiveTimeControlChanged(double liveTime){
	Q_UNUSED(liveTime);
	if(!connectedOnce_ || internalLiveTimeSet_){
		internalLiveTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	qDebug() << "Live time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onPeakTimeControlChanged(double peakTime){
	Q_UNUSED(peakTime);
	if(!connectedOnce_ || internalPeakTimeSet_){
		internalPeakTimeSet_ = false;
		return;
	}
	dwellMode_ = determineMode();
	dwellTime_ = determineTime();
	qDebug() << "Peak time says mode " << dwellMode_ << " time " << dwellTime_;
	internalSetDwellMode(dwellMode_);
	internalSetDwellTime(dwellTime_);
}

void CLSPGTDwellTimeCoordinator::onDwellTimeControlChanged(double dwellTime){
	if(!connectedOnce_ || internalDwellTimeSet_){
		internalDwellTimeSet_ = false;
		return;
	}
	qDebug() << "Dwell time changing from " << dwellTime_ << " to " << dwellTime;
	dwellTime_ = dwellTime;
	switch(dwellMode_){
	case 0:
		internalSetRealTime(dwellTime_);
		break;
	case 1:
		internalSetLiveTime(dwellTime_);
		break;
	case 2:
		internalSetPeakTime(dwellTime_);
		break;
	case 3:
		break;
	}
}

void CLSPGTDwellTimeCoordinator::onDwellTimeModeChanged(double dwellMode){
	if(!connectedOnce_ || internalDwellModeSet_){
		internalDwellModeSet_ = false;
		return;
	}
	qDebug() << "Switch dwell mode from " << dwellMode_ << " to " << dwellMode << " with time " << dwellTime_;
	switch(dwellMode_){
	case 0:
		internalSetRealTime(0.0);
		break;
	case 1:
		internalSetLiveTime(0.0);
		break;
	case 2:
		internalSetPeakTime(0.0);
		break;
	case 3:
		if(dwellMode != 0)
			internalSetRealTime(0.0);
		if(dwellMode != 1)
			internalSetLiveTime(0.0);
		if(dwellMode != 2)
			internalSetPeakTime(0.0);
		break;
	}
	dwellMode_ = int(dwellMode);
	switch(dwellMode_){
	case 0:
		internalSetRealTime(dwellTime_);
		break;
	case 1:
		internalSetLiveTime(dwellTime_);
		break;
	case 2:
		internalSetPeakTime(dwellTime_);
		break;
	case 3:
		break;
	}
}

void CLSPGTDwellTimeCoordinator::onDwellTriggerControlChanged(double dwellTrigger){
	if(!connectedOnce_)
		return;

	qDebug() << "New trigger value is " << dwellTrigger;

	if(dwellTriggerControl_->withinTolerance(0.0))
		return;

	if(oldROI0Count_->withinTolerance(0.0)){
		qDebug() << "I can trigger the PGT right away";
		internalDwellTrigger_ = true;
		oldTriggerControl_->move(1);
		return;
	}
	else{
		qDebug() << "I have to clear the PGT before I can trigger it";
		internalDwellTriggerIgnoreZero_ = true;
		connect(oldROI0Count_, SIGNAL(valueChanged(double)), this, SLOT(onOldROI0Changed(double)));
		oldClearSpectrumControl_->move(1);
	}
}

void CLSPGTDwellTimeCoordinator::onAllControlsConnected(bool connected){
	qDebug() << "Checking all PGT dwells";
	if(connected){
		connectedOnce_ = true;
		qDebug() << "All PGT dwells connected";
		dwellMode_ = determineMode();
		dwellTime_ = determineTime();
		internalSetDwellMode(dwellMode_);
		internalSetDwellTime(dwellTime_);
	}
}

void CLSPGTDwellTimeCoordinator::onOldROI0Changed(double roi0){
	if(!connectedOnce_)
		return;

	qDebug() << "Old ROI value is " << roi0;

	if(!oldROI0Count_->withinTolerance(0.0))
		return;

	qDebug() << "I've cleared the PGT, now I can trigger";
	disconnect(oldROI0Count_, SIGNAL(valueChanged(double)), this, SLOT(onOldROI0Changed(double)));
	internalDwellTrigger_ = true;
	oldTriggerControl_->move(1);
}

void CLSPGTDwellTimeCoordinator::onOldAcquiringControlChanged(double acquiring){
	if(!connectedOnce_)
		return;

	qDebug() << "Old acquiring value is " << acquiring;

	if(internalDwellTrigger_ && oldAcquiringControl_->withinTolerance(1))
		return;
	else if(internalDwellTriggerIgnoreZero_ && oldAcquiringControl_->withinTolerance(0))
		return;
	else if(oldAcquiringControl_->withinTolerance(1)){
		dwellTriggerControl_->move(1);
		return;
	}
	else if(internalDwellTrigger_)
		internalDwellTrigger_ = false;

	qDebug() << "Everything is done with the PGT, I can set the trigger back to normal now";
	if(internalDwellTriggerIgnoreZero_)
		internalDwellTriggerIgnoreZero_ = false;
	dwellTriggerControl_->move(0);
}

void CLSPGTDwellTimeCoordinator::internalSetRealTime(double realTime){
	if(realTimeControl_->withinTolerance(realTime))
		return;
	internalRealTimeSet_ = true;
	realTimeControl_->move(realTime);
}

void CLSPGTDwellTimeCoordinator::internalSetLiveTime(double liveTime){
	if(liveTimeControl_->withinTolerance(liveTime))
		return;
	internalLiveTimeSet_ = true;
	liveTimeControl_->move(liveTime);
}

void CLSPGTDwellTimeCoordinator::internalSetPeakTime(double peakTime){
	if(peakTimeControl_->withinTolerance(peakTime))
		return;
	internalPeakTimeSet_ = true;
	peakTimeControl_->move(peakTime);
}

void CLSPGTDwellTimeCoordinator::internalSetDwellTime(double dwellTime){
	if(dwellTimeControl_->withinTolerance(dwellTime))
		return;
	internalDwellTimeSet_ = true;
	dwellTimeControl_->move(dwellTime);
}

void CLSPGTDwellTimeCoordinator::internalSetDwellMode(int dwellMode){
	if(dwellModeControl_->withinTolerance(dwellMode))
		return;
	internalDwellModeSet_ = true;
	dwellModeControl_->move(dwellMode);
}

int CLSPGTDwellTimeCoordinator::determineMode() const{
	if(allControls_->isConnected()){
		double realTime = realTimeControl_->value();
		double liveTime = liveTimeControl_->value();
		double peakTime = peakTimeControl_->value();

		if( (realTime > 0.1 && liveTime > 0.1) || (realTime > 0.1 && peakTime > 0.1) || (peakTime > 0.1 && liveTime > 0.1) )
			return 3;
		else if(realTime > 0.1)
			return 0;
		else if(liveTime > 0.1)
			return 1;
		else if(peakTime > 0.1)
			return 2;
		else
			return 3;
	}
	return 3;
}

double CLSPGTDwellTimeCoordinator::determineTime() const{
	if(allControls_->isConnected()){
		double realTime = realTimeControl_->value();
		double liveTime = liveTimeControl_->value();
		double peakTime = peakTimeControl_->value();

		if( (realTime > 0.1 && liveTime > 0.1) || (realTime > 0.1 && peakTime > 0.1) || (peakTime > 0.1 && liveTime > 0.1) )
			return 0.0;
		else if(realTime > 0.1)
			return realTime;
		else if(liveTime > 0.1)
			return liveTime;
		else if(peakTime > 0.1)
			return peakTime;
		else
			return 0.0;
	}
	return 0.0;
}
