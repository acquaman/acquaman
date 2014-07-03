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


#include "ReloadPVDialog.h"

 ReloadPVDialog::~ReloadPVDialog(){}
ReloadPVDialog::ReloadPVDialog(QList<QString> reloadList, QWidget *parent) :
    QDialog(parent)
{
    QLabel *dialogMessage = new QLabel("Previous pvs detected--reload pvs?");

    QStandardItemModel *contentsModel  = new QStandardItemModel(this);
    connect( contentsModel , SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)) );

    foreach (QString entry, reloadList)
    {
        QStandardItem *modelEntry = new QStandardItem(entry);
        modelEntry->setCheckable(true);
        modelEntry->setSelectable(false);
        contentsModel ->appendRow(modelEntry);
    }

    QListView *contentsView = new QListView(this);
    contentsView->setModel(contentsModel);
    contentsView->setMaximumHeight(100);

    QPushButton *okButton = new QPushButton("Ok");
    okButton->setDefault(true);
    connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );

    QPushButton *cancelButton = new QPushButton("Cancel");
    connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(dialogMessage);
    dialogLayout->addWidget(contentsView);
    dialogLayout->addLayout(buttonLayout);

    setWindowTitle("Reload PVs");
    setLayout(dialogLayout);
}



void ReloadPVDialog::onItemChanged(QStandardItem *modelItem)
{
    // the only thing we change about an item is its check state.

    QStandardItem *item = (QStandardItem *) modelItem;

    // if the item becomes checked, add it to the list of pv names to be reloaded.
    // else if unchecked, remove all instances of that name.

    if (item->checkState() == Qt::Checked)
    {
        if (!pvNamesList_.contains(item->text()))
            pvNamesList_.append(item->text());

    } else if (item->checkState() == Qt::Unchecked) {

        pvNamesList_.removeAll(item->text());
    }
}



QStringList ReloadPVDialog::pvNamesList()
{
    return pvNamesList_;
}
