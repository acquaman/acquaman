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


#include "EntryWidget.h"

EntryWidget::EntryWidget(QWidget *parent) :
    QWidget(parent)
{
    lineEdit_ = 0;
    addButton_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();
}



EntryWidget::~EntryWidget()
{
}



void EntryWidget::onAddButtonClicked()
{
    QString textEntered = lineEdit_->text();

    if (textEntered == ""){
        QMessageBox errorBox;
        errorBox.setText("Entry cannot be empty.");
        errorBox.exec();

    } else {
        emit entryComplete(textEntered);
    }

    emit reset();
}



void EntryWidget::toTestSignal(const QString &textEntered)
{
    qDebug() << "NameEntryWidget :: name entered : " << textEntered;
}



void EntryWidget::buildComponents()
{
    lineEdit_ = new QLineEdit();
    addButton_ = new QPushButton("Add");
}



void EntryWidget::makeConnections()
{
    connect( lineEdit_, SIGNAL(returnPressed()), this, SLOT(onAddButtonClicked()) );
    connect( this, SIGNAL(reset()), lineEdit_, SLOT(clear()) );
    connect( addButton_, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()) );
    connect( this, SIGNAL(entryComplete(QString)), this, SLOT(toTestSignal(QString)) );
}



void EntryWidget::defaultSettings()
{
    QHBoxLayout *entryLayout = new QHBoxLayout();
    entryLayout->addWidget(lineEdit_);
    entryLayout->addWidget(addButton_);

    setLayout(entryLayout);
}
