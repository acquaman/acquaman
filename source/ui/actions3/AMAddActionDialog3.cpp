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


#include "AMAddActionDialog3.h"

#include "actions3/AMActionRegistry3.h"
#include "actions3/AMActionRunner3.h"
#include <QIcon>
#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

AMAddActionDialog3::AMAddActionDialog3(QWidget *parent) :
	QDialog(parent, Qt::Tool)
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMAddActionDialog3"));
	resize(515, 397);
	verticalLayout_3 = new QVBoxLayout(this);
	verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
	titleLabel = new QLabel(this);
	titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
	QFont font;
	font.setBold(true);
	font.setWeight(75);
	titleLabel->setFont(font);

	verticalLayout_3->addWidget(titleLabel);

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	actionsListWidget = new QListWidget(this);
	actionsListWidget->setObjectName(QString::fromUtf8("actionsListWidget"));
	actionsListWidget->setMovement(QListView::Static);
	actionsListWidget->setResizeMode(QListView::Adjust);
	actionsListWidget->setViewMode(QListView::IconMode);
	actionsListWidget->setWordWrap(true);

	horizontalLayout->addWidget(actionsListWidget);

	verticalLayout = new QVBoxLayout();
	verticalLayout->setSpacing(2);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	actionNameLabel = new QLabel(this);
	actionNameLabel->setObjectName(QString::fromUtf8("actionNameLabel"));
	actionNameLabel->setFont(font);
	actionNameLabel->setWordWrap(true);

	verticalLayout->addWidget(actionNameLabel);

	actionDescriptionGroupBox = new QGroupBox(this);
	actionDescriptionGroupBox->setObjectName(QString::fromUtf8("actionDescriptionGroupBox"));
	verticalLayout_2 = new QVBoxLayout(actionDescriptionGroupBox);
	verticalLayout_2->setContentsMargins(4, 4, 4, 4);
	verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	actionDescription = new QLabel(actionDescriptionGroupBox);
	actionDescription->setObjectName(QString::fromUtf8("actionDescription"));
	actionDescription->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
	actionDescription->setWordWrap(true);

	verticalLayout_2->addWidget(actionDescription);


	verticalLayout->addWidget(actionDescriptionGroupBox);

	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayout->addItem(verticalSpacer);

	addToWorkflowButton = new QPushButton(this);
	addToWorkflowButton->setObjectName(QString::fromUtf8("addToWorkflowButton"));
	addToWorkflowButton->setEnabled(false);

	verticalLayout->addWidget(addToWorkflowButton);


	horizontalLayout->addLayout(verticalLayout);

	horizontalLayout->setStretch(0, 2);
	horizontalLayout->setStretch(1, 1);

	verticalLayout_3->addLayout(horizontalLayout);


	setWindowTitle(QApplication::translate("AMAddActionDialog3", "Add Action...", 0, QApplication::UnicodeUTF8));
	titleLabel->setText(QApplication::translate("AMAddActionDialog3", "Add an action...", 0, QApplication::UnicodeUTF8));
	actionNameLabel->setText(QApplication::translate("AMAddActionDialog3", "[None Selected]", 0, QApplication::UnicodeUTF8));
	actionDescriptionGroupBox->setTitle(QString());
	actionDescription->setText(QString());
	addToWorkflowButton->setText(QApplication::translate("AMAddActionDialog3", "Add to Workflow", 0, QApplication::UnicodeUTF8));

	connect(actionsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListWidgetCurrentIndexChanged(QListWidgetItem*,QListWidgetItem*)));
	populateWithRegisteredActions();

	connect(addToWorkflowButton, SIGNAL(clicked()), this, SLOT(onAddToWorkflowButtonClicked()));
	connect(actionsListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAddToWorkflowButtonClicked()));
}

AMAddActionDialog3::~AMAddActionDialog3()
{

}

void AMAddActionDialog3::onListWidgetCurrentIndexChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	Q_UNUSED(previous)

	if(current) {
		addToWorkflowButton->setEnabled(true);
		actionNameLabel->setText(current->text() % " Action");
		actionDescription->setText(current->data(AM::DescriptionRole).toString());
	}
	else {
		addToWorkflowButton->setEnabled(false);
		actionNameLabel->setText("[None Selected]");
		actionDescription->setText(QString());
	}
}

void AMAddActionDialog3::populateWithRegisteredActions()
{
	// first, clear the list widget
	actionsListWidget->clear();

	// grab the set of registered actions
	QHash<QString, AMActionInfoActionRegistration3> registeredActions = AMActionRegistry3::s()->infoAndActionRegistry();

	// iterate through it, and add items to the list widget
	QHashIterator<QString, AMActionInfoActionRegistration3> i(registeredActions);
	while (i.hasNext()) {
		i.next();

		const AMActionInfoActionRegistration3& registration = i.value();

		if (registration.exposeToAddActionDialog){

			QListWidgetItem* item = new QListWidgetItem(registration.shortDescription, actionsListWidget);
			item->setIcon(QIcon(registration.iconFileName));
			item->setData(AM::DescriptionRole, registration.longDescription);
			item->setData(AM::NameRole, i.key());	// this holds the class name of the AMActionInfo, so we can pull it out of AMActionRegistry later.
		}
	}

	if(actionsListWidget->count())
		actionsListWidget->setCurrentRow(0);
}

void AMAddActionDialog3::onAddToWorkflowButtonClicked()
{
	// Get the selected item from the list widget
	QListWidgetItem* currentItem = actionsListWidget->currentItem();
	if(!currentItem)
		return;

	// Get the class name (key) of the selected actionInfo
	QString className = currentItem->data(AM::NameRole).toString();
	// Get the registration for that actionInfo
	AMActionInfoActionRegistration3 registration = AMActionRegistry3::s()->infoAndActionRegistry().value(className);
	// Get the metaObject for the actionInfo. (If this class was no longer found in the registry, for whatever reason, this will be a default-constructed null registration.
	if(!registration.actionInfoMetaObject)
		return;

	// Every actionInfo should have a default constructor, (since they are database objects, and we thought that would be a good requirement anyway)
	AMActionInfo3* actionInfo = qobject_cast<AMActionInfo3*>(registration.actionInfoMetaObject->newInstance());
	if(!actionInfo) {
		AMErrorMon::alert(this, -1, QString("There was a problem creating the action info '%1', likely because no Q_INVOKABLE constructor was specified. Please report this bug to the Acquaman developers.").arg(className));
		return;
	}

	AMAction3* action = AMActionRegistry3::s()->createActionFromInfo(actionInfo);
	if(!action) {
		AMErrorMon::alert(this, -2, QString("There was an error creating the action for '%1', because of a problem in the registration system. Please report this bug to the Acquaman developers.").arg(className));
		return;
	}

	AMActionRunner3::workflow()->addActionToQueue(action);

	emit actionAddedFromDialog(action);
}
