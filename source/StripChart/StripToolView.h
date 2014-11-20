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


#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include "AMQtGui.h"

#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolSidePanel.h"
#include "StripChart/ReloadPVDialog.h"
#include "StripChart/StripToolControlsPanel.h"

/// This class is the basic view for the application. The first (read: main) component is the plot, and the second is an instance of "quick controls" that can be shown or hidden using a checkbox. This class also checks whether reloading old pvs is a possibility and confirms with the user whether or not they want to.

class StripToolView : public QWidget
{
    Q_OBJECT
    
public:
    explicit StripToolView(QWidget *parent = 0);
    virtual ~StripToolView();

signals:
    void nameEntered(const QString &newName);
    void plotSelectionChanged(MPlotItem *newSelection);
    void listSelectionChanged(const QModelIndex &newSelection);
    void listItemToEdit(const QModelIndex &toEdit);

public:
    StripToolPlot* plotView() const;
    StripToolListView* listView() const;
    EntryWidget* nameEntry() const;
    WaterfallEntryWidget* waterfallEntry() const;
    TimeEntryWidget* timeEntry() const;

protected:
    StripToolSidePanel* sidePanel() const;
    StripToolControlsPanel* controlPanel() const;

protected slots:
    void toggleSidePanel();

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    bool sidePanelShown_;
    StripToolPlot *plotView_;
    StripToolSidePanel *sidePanel_;
    StripToolControlsPanel *controlPanel_;

};

#endif // STRIPTOOLVIEW_H
