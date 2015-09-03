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


#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
	// Set variables.

	variables_ = new STVariableCollection(this);
	selectedVariable_ = 0;
	plotWidget_ = new STPlotWidget(this);

	// Set up UI.

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(plotWidget_);

	setLayout(mainLayout);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

	// Get current settings.

	setTime();
	setTimeFilteringEnabled();

	// Make connections.
	connect( variables_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onCollectionRowAdded(QModelIndex, int, int)) );
	connect( variables_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onCollectionRowRemoved(QModelIndex, int, int)) );
	connect( variables_, SIGNAL(selectionChanged(STVariable*)), this, SLOT(onCollectionSelectionChanged(STVariable*)) );
	connect( variables_, SIGNAL(selectionDescriptionChanged(QString)), this, SLOT(updatePlotLeftAxisName()) );
	connect( variables_, SIGNAL(selectionUnitsChanged(QString)), this, SLOT(updatePlotLeftAxisName()) );
	connect( plotWidget_, SIGNAL(timeChanged(STTime *)), this, SLOT(setTime()) );
	connect( plotWidget_, SIGNAL(timeFilteringEnabled(bool)), this, SLOT(setTimeFilteringEnabled()) );
	connect( plotWidget_, SIGNAL(plotSelectionChanged(MPlotItem*)), this, SLOT(onPlotSelectionChanged(MPlotItem*)) );

}

STWidget::~STWidget()
{

}

void STWidget::setTime()
{
	variables_->setTime(plotWidget_->time());
}

void STWidget::setTimeFilteringEnabled()
{
	variables_->setTimeFilteringEnabled(plotWidget_->timeFilteringEnabled());
}

void STWidget::onCollectionRowAdded(const QModelIndex &index, int start, int end)
{
	Q_UNUSED(index)
	Q_UNUSED(end)

	STVariable *toAdd = variables_->variableAt(start);

	if (toAdd)
		plotWidget_->plot()->addItem(toAdd->series());
}

void STWidget::onCollectionRowRemoved(const QModelIndex &index, int start, int end)
{
	Q_UNUSED(index)
	Q_UNUSED(end)

	STVariable *toRemove = variables_->variableAt(start);

	if (toRemove)
		plotWidget_->plot()->removeItem(toRemove->series());
}

void STWidget::onCollectionSelectionChanged(STVariable *newSelection)
{
	if (newSelection) {
		plotWidget_->selector()->setSelection(newSelection->series());

	} else {
		plotWidget_->selector()->setSelection(0);
	}

	updatePlotLeftAxisName();
}

void STWidget::onPlotSelectionChanged(MPlotItem *plotSelection)
{
	QList<STVariable*> matches = variables_->variablesWithSeries(plotSelection);

	if (matches.size() > 0) {
		variables_->setSelectedVariable(matches.first());

	} else {
		variables_->setSelectedVariable(0);
	}
}

void STWidget::toAddVariable()
{
	bool ok;
	QString name = QInputDialog::getText(this, "Add Variable", "PV Name :", QLineEdit::Normal, "", &ok);

	if (ok && !name.isEmpty())
		variables_->addVariable(name);
}

void STWidget::toEditVariables()
{
	STVariableCollectionEditor *editor = new STVariableCollectionEditor(variables_);
	connect( editor, SIGNAL(addVariable()), this, SLOT(toAddVariable()) );

	showEditorDialog(editor);
}

void STWidget::toEditPlot()
{
	showEditorDialog(new STPlotEditor(plotWidget_));
}

void STWidget::updatePlotLeftAxisName()
{
	STVariable *selection = variables_->selectedVariable();

	if (selection) {
		QString description = variables_->data(variables_->indexOf(selection), Qt::DisplayRole).toString();

		if (selection->hasUnits()) {
			QString units = selection->units();
			plotWidget_->plot()->axisLeft()->setAxisName(description + " [" + units + "]");

		} else {
			plotWidget_->plot()->axisLeft()->setAxisName(description);
		}

	} else {
		plotWidget_->plot()->axisLeft()->setAxisName("");
	}
}

void STWidget::onCustomContextMenuRequested(QPoint position)
{
	QMenu menu(this);

	QAction *action = menu.addAction("Add variable");
	action = menu.addAction("Add SR1 Current");
	menu.addSeparator();

	action = menu.addAction("Edit variables");
	if (variables_->variableCount() == 0)
		action->setEnabled(false);
	action = menu.addAction("Edit plot");

	QAction *selected = menu.exec(mapToGlobal(position));

	if (selected) {

		if (selected->text() == "Add variable") {
			toAddVariable();

		} else if (selected->text() == "Add SR1 Current") {
			addSR1Current();

		} else if (selected->text() == "Edit variables") {
			toEditVariables();

		} else if (selected->text() == "Edit plot") {
			toEditPlot();

		}
	}
}

void STWidget::addSR1Current()
{
	variables_->addVariable("PCT1402-01:mA:fbk");

	STVariable *sr1Current = variables_->variablesWithName("PCT1402-01:mA:fbk").last();
	sr1Current->setDescription("SR1 Current");
	sr1Current->setUnits("mA");
	sr1Current->setColor(QColor(Qt::blue));
}

void STWidget::showEditorDialog(STEditor *editor)
{
	STEditorDialog *dialog = new STEditorDialog(editor, this);
	dialog->show();
}
