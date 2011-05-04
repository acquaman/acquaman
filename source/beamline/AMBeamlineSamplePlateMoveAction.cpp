#include "AMBeamlineSamplePlateMoveAction.h"

#include "ui/AMSamplePlateView.h"
#include "AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMBeamlineSamplePlateMoveAction::AMBeamlineSamplePlateMoveAction(int sampleID, AMSamplePlateItemModel *plateModel, QObject *parent) :
		AMBeamlineControlSetMoveAction(AMBeamline::bl()->currentSamplePositioner(), parent)
{
	sampleID_ = sampleID;
	samplePlateModel_ = plateModel;

	bool foundSampleOnPlate = false;
	if(samplePlateModel_){
		for(int x = 0; x < samplePlateModel_->rowCount(QModelIndex()); x++){
			AMSamplePosition* tmpSamplePos = qobject_cast<AMSamplePosition*>(samplePlateModel_->data(samplePlateModel_->index(x), AM::PointerRole).value<QObject*>());
			if(tmpSamplePos->sampleId() == sampleID_ ){
				qDebug() << "Found it in the model at " << x;
				foundSampleOnPlate = true;
				connect(samplePlateModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSamplePlateDataChanged(QModelIndex,QModelIndex)));
				setDescription("Move to sample \""+samplePlateModel_->data(samplePlateModel_->index(x), Qt::EditRole).toString()+"\"");
			}
//			tmpAction = samplePlateAddActionMenu_->addAction(samplePlateModel_->data(samplePlateModel_->index(x), Qt::EditRole).toString());
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

AMBeamlineActionItemView* AMBeamlineSamplePlateMoveAction::createView(int index){
	return new AMBeamlineSamplePlateMoveActionView(this, index);
}

void AMBeamlineSamplePlateMoveAction::onSamplePlateDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	AMSamplePosition* tmpSamplePos = qobject_cast<AMSamplePosition*>(samplePlateModel_->data(topLeft, AM::PointerRole).value<QObject*>());
	if(tmpSamplePos->sampleId() == sampleID_ ){
		qDebug() << "Yeah it was mine that changed";
		setDescription("Move to sample \""+samplePlateModel_->data(topLeft, Qt::EditRole).toString()+"\"");
		emit descriptionChanged(description());
	}
	else
		qDebug() << "Some other sample changed";
}


AMBeamlineSamplePlateMoveActionView::AMBeamlineSamplePlateMoveActionView(AMBeamlineSamplePlateMoveAction *samplePlateAction, int index, QWidget *parent) :
		AMBeamlineControlSetMoveActionView(samplePlateAction, index, parent)
{
	samplePlateAction_ = samplePlateAction;
	if(samplePlateAction_)
		connect(samplePlateAction_, SIGNAL(descriptionChanged(QString)), this, SLOT(onInfoChanged()));
	onInfoChanged();

}

void AMBeamlineSamplePlateMoveActionView::onInfoChanged(){
	QString sampleName, tmpStr;
	if(index_ != -1){
		sampleName.setNum(index_);
		sampleName.append(". ");
	}

	sampleName.append(samplePlateAction_->description());
	infoLabel_->setText(sampleName);
	setWindowTitle(samplePlateAction_->description());
}
