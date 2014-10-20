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


#ifndef STRIPTOOLSIDEPANEL_H
#define STRIPTOOLSIDEPANEL_H

#pragma GCC diagnostic ignored "-Wunused-private-field"
#include <QtGui>
#pragma GCC diagnostic warning "-Wunused-private-field"

#include <QWidget>

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/EntryWidget.h"
#include "StripChart/TimeEntryWidget.h"
#include "StripChart/WaterfallEntryWidget.h"

/// This class is a sort of optional sidebar that provides a number of ways for the user to interact with an added pv, as well as a quick way to add a new one.
class StripToolSidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolSidePanel(QWidget *parent = 0);
    ~StripToolSidePanel();
    friend class StripToolView;
    
signals:

public:
    StripToolListView* listView() const;
    EntryWidget *nameEntry() const;
    TimeEntryWidget *timeEntry() const;
    WaterfallEntryWidget *waterfallEntry() const;

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    StripToolListView *listView_;
    EntryWidget *nameEntry_;
    TimeEntryWidget *timeEntry_;
    WaterfallEntryWidget *waterfallEntry_;

};

#endif // STRIPTOOLSIDEPANEL_H
