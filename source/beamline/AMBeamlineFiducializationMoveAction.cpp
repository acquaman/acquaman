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
