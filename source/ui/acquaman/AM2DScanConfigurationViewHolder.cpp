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


#include "AM2DScanConfigurationViewHolder.h"

#include "actions/AMBeamline2DScanAction.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/AMWorkflowManagerView.h"

#include <QRadioButton>

AM2DScanConfigurationViewHolder::AM2DScanConfigurationViewHolder(AMWorkflowManagerView *workflow, AMScanConfigurationView *view, QWidget *parent)
	:AMScanConfigurationViewHolder(workflow, view, parent)
{
}

void AM2DScanConfigurationViewHolder::onAddToQueueRequested()
{
	if(!view_ || !view_->configuration())
		return;

	AMScanConfiguration *config = view_->configuration()->createCopy();
	/*
	config->setUserScanName(scanNameDictionaryLineEdit_->text());
	config->setUserExportNmae(exportNameDictionaryLineEdit_->text());
	*/
	AMBeamline2DScanAction* action = new AMBeamline2DScanAction(config);
	workflow_->insertBeamlineAction(-1, action);

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}

#include <QMessageBox>
void AM2DScanConfigurationViewHolder::onStartScanRequested(){

	if(!view_ || !view_->configuration())
		return;

	if(workflow_->isRunning() || workflow_->beamlineBusy())
		return;

	bool startNow = true;
	int position = 0;

	// check first: if there's already items in the workflow, we need to find out if they want to add this action to the end of the queue, add this action to the beginning of the queue,
	if(workflow_->actionItemCount()) {

		qDebug() << workflow_->actionItemCount();

		QMessageBox questionBox;
		questionBox.setText("There are already scans waiting in the workflow queue.");
		questionBox.setInformativeText("What do you want to do with this scan?");
		questionBox.setIcon(QMessageBox::Question);
		QPushButton* cancel = questionBox.addButton("Cancel", QMessageBox::RejectRole);
		QPushButton* addToBeginningAndStart = questionBox.addButton("Add to beginning and start", QMessageBox::YesRole);
		QPushButton* addToEndAndStart = questionBox.addButton("Add to end and start workflow", QMessageBox::YesRole);
		QPushButton* addToEnd = questionBox.addButton("Add to end", QMessageBox::YesRole);
		questionBox.setDefaultButton(addToEndAndStart);

		questionBox.exec();

		QAbstractButton* result = questionBox.clickedButton();
		if(result == cancel) {
			return;
		}
		else if(result == addToEnd) {
			position = -1;
			startNow = false;
		}
		else if(result == addToEndAndStart) {
			position = -1;
			startNow = false;
		}
		else if(result == addToBeginningAndStart) {
			position = 0;
			startNow = true;
		}
	}

	AMScanConfiguration *config = view_->configuration()->createCopy();
	/*
	config->setUserScanName(scanNameDictionaryLineEdit_->text());
	config->setUserExportNmae(exportNameDictionaryLineEdit_->text());
	*/
	AMBeamline2DScanAction* action = new AMBeamline2DScanAction(config);
	workflow_->insertBeamlineAction(position, action, startNow);
}
