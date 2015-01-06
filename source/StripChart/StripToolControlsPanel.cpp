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


#include "StripToolControlsPanel.h"
#include <QDebug>

StripToolControlsPanel::StripToolControlsPanel(QWidget *parent) : QWidget(parent)
{
	nameEntry_ = 0;
	/*
	pauseResume_ = 0;
	*/
	timeEntry_ = 0;
	sidebarButton_ = 0;
	waterfallEntry_ = 0;

	buildComponents();
	makeConnections();
	defaultSettings();

	qDebug() << "StripToolControlsPanel object created.";
}



StripToolControlsPanel::~StripToolControlsPanel()
{
}



EntryWidget* StripToolControlsPanel::nameEntry() const {
	return nameEntry_;
}



WaterfallEntryWidget* StripToolControlsPanel::waterfallEntry() const {
	return waterfallEntry_;
}



TimeEntryWidget* StripToolControlsPanel::timeEntry() const {
	return timeEntry_;
}



QPushButton* StripToolControlsPanel::sidebarButton() const {
	return sidebarButton_;
}



void StripToolControlsPanel::buildComponents()
{
	nameEntry_ = new EntryWidget();
	timeEntry_ = new TimeEntryWidget();
	sidebarButton_ = new QPushButton();
	waterfallEntry_ = new WaterfallEntryWidget();
}



void StripToolControlsPanel::makeConnections()
{

}



void StripToolControlsPanel::defaultSettings()
{
	QGridLayout *panelLayout = new QGridLayout();
	panelLayout->addWidget(nameEntry_, 0, 0);
	/*
	panelLayout->addWidget(pauseResume_, 0, 1);
	*/
	panelLayout->addWidget(timeEntry_, 0, 2);
	panelLayout->addWidget(sidebarButton_, 0, 3);
	panelLayout->addWidget(waterfallEntry_, 1, 0);

	QGroupBox *controlsGroup = new QGroupBox();
	controlsGroup->setLayout(panelLayout);
	controlsGroup->setFlat(true);

	QVBoxLayout *widgetLayout = new QVBoxLayout();
	widgetLayout->addWidget(controlsGroup);

	setLayout(widgetLayout);
}



