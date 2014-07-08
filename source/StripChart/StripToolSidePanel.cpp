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


#include "StripToolSidePanel.h"

StripToolSidePanel::StripToolSidePanel(QWidget *parent) : QWidget(parent)
{
    listView_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolSidePanel object created.";
}



StripToolSidePanel::~StripToolSidePanel()
{
}



StripToolListView* StripToolSidePanel::listView() const
{
    return listView_;
}



EntryWidget* StripToolSidePanel::nameEntry() const
{
    return nameEntry_;
}



TimeEntryWidget* StripToolSidePanel::timeEntry() const
{
    return timeEntry_;
}



WaterfallEntryWidget* StripToolSidePanel::waterfallEntry() const
{
    return waterfallEntry_;
}



void StripToolSidePanel::buildComponents()
{
    listView_ = new StripToolListView(this);
    nameEntry_ = new EntryWidget(this);
    timeEntry_ = new TimeEntryWidget(this);
    waterfallEntry_ = new WaterfallEntryWidget(this);
}



void StripToolSidePanel::makeConnections()
{
}



void StripToolSidePanel::defaultSettings()
{
    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addWidget(nameEntry_);
    controlsLayout->addWidget(timeEntry_);
    controlsLayout->addWidget(waterfallEntry_);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(250);
}

