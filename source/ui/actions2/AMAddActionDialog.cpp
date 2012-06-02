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


#include "AMAddActionDialog.h"
#include "ui_AMAddActionDialog.h"

#include "actions2/AMActionRegistry.h"
#include "actions2/AMActionRunner.h"
#include <QIcon>
#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

AMAddActionDialog::AMAddActionDialog(QWidget *parent) :
	QDialog(parent, Qt::Tool),
    ui(new Ui::AMAddActionDialog)
{
    ui->setupUi(this);

	connect(ui->actionsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListWidgetCurrentIndexChanged(QListWidgetItem*,QListWidgetItem*)));
	populateWithRegisteredActions();

	connect(ui->addToWorkflowButton, SIGNAL(clicked()), this, SLOT(onAddToWorkflowButtonClicked()));
	connect(ui->actionsListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAddToWorkflowButtonClicked()));
}

AMAddActionDialog::~AMAddActionDialog()
{
	delete ui;
}

void AMAddActionDialog::onListWidgetCurrentIndexChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	Q_UNUSED(previous)

	if(current) {
		ui->addToWorkflowButton->setEnabled(true);
		ui->actionNameLabel->setText(current->text() % " Action");
		ui->actionDescription->setText(current->data(AM::DescriptionRole).toString());
	}
	else {
		ui->addToWorkflowButton->setEnabled(false);
		ui->actionNameLabel->setText("[None Selected]");
		ui->actionDescription->setText(QString());
	}
}

void AMAddActionDialog::populateWithRegisteredActions()
{
	// first, clear the list widget
	ui->actionsListWidget->clear();

	// grab the set of registered actions
	QHash<QString, AMActionInfoActionRegistration> registeredActions = AMActionRegistry::s()->infoAndActionRegistry();

	// iterate through it, and add items to the list widget
	QHashIterator<QString, AMActionInfoActionRegistration> i(registeredActions);
	while (i.hasNext()) {
		i.next();

		const AMActionInfoActionRegistration& registration = i.value();
		QListWidgetItem* item = new QListWidgetItem(registration.shortDescription, ui->actionsListWidget);
		item->setIcon(QIcon(registration.iconFileName));
		item->setData(AM::DescriptionRole, registration.longDescription);
		item->setData(AM::NameRole, i.key());	// this holds the class name of the AMActionInfo, so we can pull it out of AMActionRegistry later.
	}

	if(ui->actionsListWidget->count())
		ui->actionsListWidget->setCurrentRow(0);
}

void AMAddActionDialog::onAddToWorkflowButtonClicked()
{
	// Get the selected item from the list widget
	QListWidgetItem* currentItem = ui->actionsListWidget->currentItem();
	if(!currentItem)
		return;

	// Get the class name (key) of the selected actionInfo
	QString className = currentItem->data(AM::NameRole).toString();
	// Get the registration for that actionInfo
	AMActionInfoActionRegistration registration = AMActionRegistry::s()->infoAndActionRegistry().value(className);
	// Get the metaObject for the actionInfo. (If this class was no longer found in the registry, for whatever reason, this will be a default-constructed null registration.
	if(!registration.actionInfoMetaObject)
		return;

	// Every actionInfo should have a default constructor, (since they are database objects, and we thought that would be a good requirement anyway)
	AMActionInfo* actionInfo = qobject_cast<AMActionInfo*>(registration.actionInfoMetaObject->newInstance());
	if(!actionInfo) {
		AMErrorMon::alert(this, -1, QString("There was a problem creating the action info '%1', likely because no Q_INVOKABLE constructor was specified. Please report this bug to the Acquaman developers.").arg(className));
		return;
	}

	AMAction* action = AMActionRegistry::s()->createActionFromInfo(actionInfo);
	if(!action) {
		AMErrorMon::alert(this, -2, QString("There was an error creating the action for '%1', because of a problem in the registration system. Please report this bug to the Acquaman developers.").arg(className));
		return;
	}

	AMActionRunner::s()->addActionToQueue(action);
}
