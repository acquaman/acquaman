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
    plotWidget_ = new STPlotWidget();

    // Set up UI.

    QVBoxLayout *mainLayout = new QVBoxLayout();

    plotWidget_->setPlotName("Storage ring current");
    plotWidget_->showPlotName(true);
    mainLayout->addWidget(plotWidget_);

    editButton_ = new QPushButton("Edit...", this);
    connect( editButton_, SIGNAL(clicked()), this, SLOT(onEditButtonClicked()) );
    mainLayout->addWidget(editButton_);

    setLayout(mainLayout);

    // Make connections.
    connect( variables_, SIGNAL(variableConnected(bool, int)), this, SLOT(onVariableConnected(bool, int)) );
    connect( plotWidget_, SIGNAL(addVariableClicked()), this, SLOT(toAddVariable()) );
    connect( plotWidget_, SIGNAL(editPlotClicked()), this, SLOT(toEditPlot()) );

    variables_->addVariable("PCT1402-01:mA:fbk");
}

STWidget::~STWidget()
{

}

void STWidget::onVariableConnected(bool isConnected, int variableIndex)
{
    STVariable *variable = variables_->variableAt(variableIndex);

    if (isConnected) {
        plotWidget_->plot()->addItem(variable->series());
    }
}

void STWidget::onEditButtonClicked()
{
    qDebug() << "Edit button clicked.";

    STVariableCollectionEditor *editor = new STVariableCollectionEditor(variables_);
    STEditorDialog *dialog = new STEditorDialog(editor, this);

    dialog->show();
}

void STWidget::toAddVariable()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Add Variable", "PV Name :", QLineEdit::Normal, "", &ok);

    if (ok && !name.isEmpty())
        variables_->addVariable(name);
}

void STWidget::toEditPlot()
{
    STPlotEditor *editor = new STPlotEditor(plotWidget_);
    STEditorDialog *dialog = new STEditorDialog(editor, this);

    dialog->show();
}
