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


#include "AMBeamlineFiducializationMoveAction.h"

#include <QLabel>

#include "AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMBeamlineFiducializationMoveAction::AMBeamlineFiducializationMoveAction(int fiducializationIndex, QObject *parent) :
		AMBeamlineControlSetMoveAction(AMBeamline::bl()->currentSamplePositioner(), parent)
{
	fiducializationIndex_ = fiducializationIndex;
	sampleDescription_ = "";
	if(fiducializationIndex_ < AMBeamline::bl()->currentFiducializations().count()){
		//Change 0-indexed source list to 1-indexed human-readable format
		sampleDescription_ = QString("Spot #%1").arg(fiducializationIndex_+1);
		setDescription(QString("Move to sample spot #%1").arg(fiducializationIndex_+1));
		setSetpoint(AMBeamline::bl()->currentFiducializations().at(fiducializationIndex_));
	}
	else{
		setDescription(QString("Error"));
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				AMBEAMLINEACTIONITEM_INVALID_FIDUCIALIZATION_INDEX,
				"Error, fiducialization index is invalid. Please report this bug to the Acquaman developers."));
	}
}

AMBeamlineActionItemView* AMBeamlineFiducializationMoveAction::createView(int index){
	return new AMBeamlineFiducializationMoveActionView(this, index);
}

AMBeamlineActionItem* AMBeamlineFiducializationMoveAction::createCopy() const{
	if(controlSet_){
		AMBeamlineFiducializationMoveAction *retVal = new AMBeamlineFiducializationMoveAction(fiducializationIndex_);
		retVal->setSetpoint(setpoint_);
		return retVal;
	}
	return 0; //NULL
}

QString AMBeamlineFiducializationMoveAction::sampleDescription() const{
	return sampleDescription_;
}

AMBeamlineFiducializationMoveActionView::AMBeamlineFiducializationMoveActionView(AMBeamlineFiducializationMoveAction *fiducializationAction, int index, QWidget *parent) :
		AMBeamlineControlSetMoveActionView(fiducializationAction, index, parent)
{
	fiducializationAction_ = fiducializationAction;
	if(fiducializationAction_)
		connect(fiducializationAction_, SIGNAL(descriptionChanged(QString)), this, SLOT(onInfoChanged()));
	onInfoChanged();
}

void AMBeamlineFiducializationMoveActionView::onInfoChanged(){
	QString sampleName, tmpStr;
	if(index_ != -1){
		sampleName.setNum(index_);
		sampleName.append(". ");
	}

	sampleName.append(fiducializationAction_->description());
	infoLabel_->setText(sampleName);
	setWindowTitle(fiducializationAction_->description());
}
