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


#ifndef AMADDACTIONDIALOG3_H
#define AMADDACTIONDIALOG3_H

#include <QDialog>

namespace Ui {
class AMAddActionDialog3;
}

class QListWidgetItem;

class AMAction3;

class AMAddActionDialog3 : public QDialog
{
	Q_OBJECT

public:
	/// Constructor. The view is populated during construction with all actions currently registered in the AMActionRegistry [ActionInfo/Action].
	explicit AMAddActionDialog3(QWidget *parent = 0);

	virtual ~AMAddActionDialog3();

signals:
	void actionAddedFromDialog(AMAction3 *action);

protected slots:
	/// Called when the selected item is changed in the list view; this updates the item-specific title and description shown on the right of the dialog, and enables the 'Add to Workflow' button.
	void onListWidgetCurrentIndexChanged(QListWidgetItem* current, QListWidgetItem* previous);

	/// Called when the workflow button is clicked. Attempts to instantiate the action based on a default-constructed AMActionInfo, and add it to the workflow.
	void onAddToWorkflowButtonClicked();

protected:
	/// Helper function to clear the existing actions displayed in the list view, and populate it with all the actions registered in AMActionRegistry.
	void populateWithRegisteredActions();

	/// UI elements
	Ui::AMAddActionDialog3 *ui;
};

#endif // AMADDACTIONDIALOG_H
