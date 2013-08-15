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


#include "AMBeamlineSamplePlatePre2013MoveAction.h"

#include "ui/dataman/AMSamplePlatePre2013View.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMBeamlineSamplePlatePre2013MoveAction::AMBeamlineSamplePlatePre2013MoveAction(int sampleID, AMSamplePlatePre2013ItemModel *plateModel, QObject *parent) :
		AMBeamlineControlSetMoveAction(AMBeamline::bl()->currentSamplePositioner(), parent)
{
	sampleID_ = sampleID;
	samplePlateModel_ = plateModel;
	sampleDescription_ = "";

	bool foundSampleOnPlate = false;
	if(samplePlateModel_){
		for(int x = 0; x < samplePlateModel_->rowCount(QModelIndex()); x++){
			AMSamplePositionPre2013* tmpSamplePos = qobject_cast<AMSamplePositionPre2013*>(samplePlateModel_->data(samplePlateModel_->index(x), AM::PointerRole).value<QObject*>());
			if(tmpSamplePos->sampleId() == sampleID_ ){
				foundSampleOnPlate = true;
				connect(samplePlateModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSamplePlateDataChanged(QModelIndex,QModelIndex)));
				sampleDescription_ = samplePlateModel_->data(samplePlateModel_->index(x), Qt::EditRole).toString();
				setDescription("Move to sample \""+samplePlateModel_->data(samplePlateModel_->index(x), Qt::EditRole).toString()+"\"");
			}
		}
	}
	if(!foundSampleOnPlate){
		setDescription(QString("Error"));
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				AMBEAMLINEACTIONITEM_SAMPLE_ID_NOT_ON_PLATE,
				"Error, sample ID not on this sample plate. Please report this bug to the Acquaman developers."));
	}
}

AMBeamlineActionItemView* AMBeamlineSamplePlatePre2013MoveAction::createView(int index){
	return new AMBeamlineSamplePlatePre2013MoveActionView(this, index);
}

AMBeamlineActionItem* AMBeamlineSamplePlatePre2013MoveAction::createCopy() const{
	if(sampleID_ && samplePlateModel_){
		AMBeamlineSamplePlatePre2013MoveAction *retVal = new AMBeamlineSamplePlatePre2013MoveAction(sampleID_, samplePlateModel_);
		retVal->setSetpoint(setpoint_);
		return retVal;
	}
	return 0; //NULL
}

QString AMBeamlineSamplePlatePre2013MoveAction::sampleDescription() const{
	return sampleDescription_;
}

void AMBeamlineSamplePlatePre2013MoveAction::onSamplePlateDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	Q_UNUSED(bottomRight)
	AMSamplePositionPre2013* tmpSamplePos = qobject_cast<AMSamplePositionPre2013*>(samplePlateModel_->data(topLeft, AM::PointerRole).value<QObject*>());
	if(tmpSamplePos->sampleId() == sampleID_ ){
		setDescription("Move to sample \""+samplePlateModel_->data(topLeft, Qt::EditRole).toString()+"\"");
		emit descriptionChanged(description());
	}
}


AMBeamlineSamplePlatePre2013MoveActionView::AMBeamlineSamplePlatePre2013MoveActionView(AMBeamlineSamplePlatePre2013MoveAction *samplePlateAction, int index, QWidget *parent) :
		AMBeamlineControlSetMoveActionView(samplePlateAction, index, parent)
{
	samplePlateAction_ = samplePlateAction;
	if(samplePlateAction_)
		connect(samplePlateAction_, SIGNAL(descriptionChanged(QString)), this, SLOT(onInfoChanged()));
	onInfoChanged();

}

void AMBeamlineSamplePlatePre2013MoveActionView::onInfoChanged(){
	QString sampleName, tmpStr;
	if(index_ != -1){
		sampleName.setNum(index_);
		sampleName.append(". ");
	}

	sampleName.append(samplePlateAction_->description());
	infoLabel_->setText(sampleName);
	setWindowTitle(samplePlateAction_->description());
}
