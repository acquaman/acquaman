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


#ifndef STRIPTOOLCONTROLSPANEL_H
#define STRIPTOOLCONTROLSPANEL_H

#include "AMQtGui.h"

#include <QWidget>

#include "StripChart/StripToolModel.h"
#include "StripChart/EntryWidget.h"
#include "StripChart/DoubleButtonWidget.h"
#include "StripChart/TimeEntryWidget.h"
#include "StripChart/WaterfallEntryWidget.h"

class StripToolControlsPanel : public QWidget
{
	Q_OBJECT

public:
	explicit StripToolControlsPanel(QWidget *parent = 0);
	~StripToolControlsPanel();

public:
	EntryWidget* nameEntry() const;
	WaterfallEntryWidget* waterfallEntry() const;
	TimeEntryWidget* timeEntry() const;
	QPushButton* sidebarButton() const;

public slots:

private:
	void buildComponents();
	void makeConnections();
	void defaultSettings();

private:
	EntryWidget *nameEntry_;
	/*
	DoubleButtonWidget *pauseResume_;
	*/
	TimeEntryWidget *timeEntry_;
	QPushButton *sidebarButton_;
	WaterfallEntryWidget *waterfallEntry_;

};

#endif // STRIPTOOLCONTROLSPANEL_H

