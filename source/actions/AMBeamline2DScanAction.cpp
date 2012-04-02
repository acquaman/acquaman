#include "AMBeamline2DScanAction.h"

#include "acquaman/AM2DDacqScanController.h"

// AMBeamline2DScanAction
//////////////////////////////////////////////////

AMBeamline2DScanAction::AMBeamline2DScanAction(AMScanConfiguration *cfg, QObject *parent)
	: AMBeamlineScanAction(cfg, parent)
{
}

void AMBeamline2DScanAction::stopScanAtEndOfLine()
{
	AM2DDacqScanController *controller = qobject_cast<AM2DDacqScanController *>(ctrl_);
	if (controller){

		controller->stopAtTheEndOfLine();
		setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Stopping At The End Of The Line]");
		emit descriptionChanged();
	}
}

void AMBeamline2DScanAction::onScanResumed()
{
	AM2DDacqScanController *controller = qobject_cast<AM2DDacqScanController *>(ctrl_);

	if (controller && controller->stoppingAtTheEndOfLine())
		setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Stopping At The End Of The Line]");

	else
		setDescription(cfg_->description()+" on "+lastSampleDescription_+" [Running]");

	emit descriptionChanged();
	emit resumed();
}

AMBeamlineActionItemView *AMBeamline2DScanAction::createView(int index)
{
	return new AMBeamline2DScanActionView(this, index);
}

// AMBeamline2DScanActionView
///////////////////////////////////////////////

AMBeamline2DScanActionView::AMBeamline2DScanActionView(AMBeamline2DScanAction *scanAction, int index, QWidget *parent)
	: AMBeamlineScanActionView(scanAction, index, parent)
{
	stopAtEndOfLineButton_ = new QPushButton("Stop At End Of Line");
	stopAtEndOfLineButton_->setMaximumHeight(progressBar_->size().height());
	stopAtEndOfLineButton_->setFixedWidth(150);
	stopAtEndOfLineButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	stopAtEndOfLineButton_->setEnabled(false);
	connect(stopAtEndOfLineButton_, SIGNAL(clicked()), this, SLOT(onStopScanAtEndOfLineButtonClicked()));

	secondRowLayout_->addWidget(stopAtEndOfLineButton_, 0, Qt::AlignRight);
}

void AMBeamline2DScanActionView::onStopScanAtEndOfLineButtonClicked()
{
	AMBeamline2DScanAction *action = qobject_cast<AMBeamline2DScanAction *>(scanAction_);
	if (action){

		action->stopScanAtEndOfLine();
		stopAtEndOfLineButton_->setEnabled(false);
	}
}

void AMBeamline2DScanActionView::onScanStarted()
{
	stopAtEndOfLineButton_->setEnabled(true);
	AMBeamlineScanActionView::onScanStarted();
}
