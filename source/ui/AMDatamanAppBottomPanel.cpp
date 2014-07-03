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


#include "AMDatamanAppBottomPanel.h"

 AMDatamanAppBottomPanel::~AMDatamanAppBottomPanel(){}
AMDatamanAppBottomPanel::AMDatamanAppBottomPanel(QWidget *parent)
	: AMBottomPanel(parent)
{
	QToolButton *addExperimentButton = new QToolButton;
	addExperimentButton->setIcon(QIcon(":/22x22/folder-new.png"));
	addExperimentButton->setToolTip("Add Experiment Folder...");
	connect(addExperimentButton, SIGNAL(clicked()), this, SIGNAL(addExperimentButtonClicked()));

	statusView_ = new AMStatusView;

	layout_->addSpacing(25);
	layout_->addWidget(addExperimentButton);
	layout_->addStretch();
	layout_->addWidget(statusView_);
}
