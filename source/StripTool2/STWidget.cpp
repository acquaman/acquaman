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
    plotWidget_ = new STPlotWidget(this);

    // Set up UI.

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(plotWidget_);

    setLayout(mainLayout);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

    // Make connections.

    connect( variables_, SIGNAL(variableConnectedStateChanged(int)), this, SLOT(onVariableConnected(int)) );
    connect( variables_, SIGNAL(variableDescriptionChanged(int)), this, SLOT(onVariableAxisInfoChanged(int)) );
    connect( variables_, SIGNAL(variableUnitsChanged(int)), this, SLOT(onVariableAxisInfoChanged(int)) );
}

STWidget::~STWidget()
{

}

void STWidget::onVariableConnected(int variableIndex)
{
    STVariable *variable = variables_->variableAt(variableIndex);

    if (variable->isConnected()) {
        plotWidget_->plot()->addItem(variable->series());
    }
}

void STWidget::onVariableAxisInfoChanged(int variableIndex)
{
    STVariable *variable = variables_->variableAt(variableIndex);
    QString description = variable->description();
    QString units = variable->units();

    plotWidget_->setAxisName(description + " [" + units + "]");
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
    showEditorDialog(new STVariableCollectionEditor(variables_));
}

void STWidget::toEditPlot()
{
    showEditorDialog(new STPlotEditor(plotWidget_));
}

void STWidget::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *action = menu.addAction("Add variable");

    action = menu.addAction("Edit variables");
    if (variables_->variableCount() == 0)
        action->setEnabled(false);

    action = menu.addAction("Edit plot");

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {

        if (selected->text() == "Add variable") {
            toAddVariable();

        } else if (selected->text() == "Edit variables") {
            toEditVariables();

        } else if (selected->text() == "Edit plot") {
            toEditPlot();

        }
    }
}

void STWidget::showEditorDialog(STEditor *editor)
{
    STEditorDialog *dialog = new STEditorDialog(editor, this);
    dialog->show();
}
