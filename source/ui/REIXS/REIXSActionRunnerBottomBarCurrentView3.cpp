#include "REIXSActionRunnerBottomBarCurrentView3.h"
#include "beamline/REIXS/REIXSBeamline.h"

#include <QHBoxLayout>
#include <QInputDialog>

REIXSActionRunnerBottomBarCurrentView3::REIXSActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent) :
	AMActionRunnerBottomBarCurrentView3(actionRunner, parent)
{


	addXESScanTime_ = new QToolButton;
	addXESScanTime_->setIcon(QIcon(":/22x22/list-add.png"));
	addXESScanTime_->setToolTip("Add time to scan.");
	addXESScanTime_->setEnabled(false);
	addXESScanTime_->setVisible(false);

	QHBoxLayout* hl = qobject_cast<QHBoxLayout*>(layout());
	hl->addWidget(addXESScanTime_);
	setLayout(hl);

	connect(addXESScanTime_, SIGNAL(clicked()), this, SLOT(onAddXESScanTimeClicked()));


}


void REIXSActionRunnerBottomBarCurrentView3::onCurrentActionChanged(AMAction3 *action)
{
	AMActionRunnerCurrentViewBase::onCurrentActionChanged(action);
	AMActionRunnerBottomBarCurrentView3::onCurrentActionChanged(action);

	action_ = action;

			if (action && !action->parentAction() && action->info()->name() == "XES"){

		action_ = qobject_cast<AMAction3 *>(action);
		addXESScanTime_->setEnabled(true);
		addXESScanTime_->setVisible(true);
	}

	else{

		addXESScanTime_->setEnabled(false);
		addXESScanTime_->setVisible(false);
	}

	if (!action){

		whatIsRunning_ = "No action running.";
		descriptionLabel_->setText(whatIsRunning_);
	}
}


void REIXSActionRunnerBottomBarCurrentView3::onAddXESScanTimeClicked()
{
	bool ok;
	double min = QInputDialog::getDouble(this, "Add time to XES Scan...", "Enter minutes to add:", 5, 0, 720, 0, &ok);

	if (ok)
	{
		double expectedDuration = action_->info()->expectedDuration() + (min * 60);

		if (REIXSBeamline::bl()->mcpDetector()->addAcquisitionTime(min * 60))
			action_->info()->setExpectedDuration(expectedDuration);
	}
}


