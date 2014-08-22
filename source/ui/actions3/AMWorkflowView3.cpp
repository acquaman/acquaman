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


#include "AMWorkflowView3.h"
#include "actions3/AMActionRunner3.h"

#include "util/AMFontSizes.h"

#include <QBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

 AMWorkflowView3::~AMWorkflowView3(){}
AMWorkflowView3::AMWorkflowView3(AMActionRunner3 *actionRunner, QWidget *parent) :
	QWidget(parent)
{
	layoutSpacer_ = 0;

	QHBoxLayout* hl = new QHBoxLayout();
	hl->setContentsMargins(12,12,12,12);
	hl->setSpacing(12);

	QLabel* titleIcon = new QLabel();
	titleIcon->setMaximumSize(36,36);
	titleIcon->setPixmap(QPixmap(":/user-away.png"));
	titleIcon->setScaledContents(true);
	hl->addWidget(titleIcon);
	//QLabel* titleLabel = new QLabel("Workflow");
	QLabel* titleLabel = new QLabel(actionRunner->actionRunnerTitle());
	titleLabel->setStyleSheet("font: " AM_FONT_XLARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	hl->addWidget(titleLabel);
	hl->addStretch(1);
	addActionButton_ = new QPushButton("Add Action...");
	hl->addWidget(addActionButton_);

	currentView_ = new AMActionRunnerCurrentView3(actionRunner);
	queueView_ = new AMActionRunnerQueueView3(actionRunner);
	historyView_ = new AMActionHistoryView3(actionRunner, actionRunner->loggingDatabase());
	addActionDialog_ = 0;

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);

	vl->addLayout(hl);
	vl->addWidget(historyView_);
	vl->addSpacing(12);
	vl->addWidget(currentView_);
	vl->addSpacing(12);
	vl->addWidget(queueView_);

	connect(queueView_, SIGNAL(collapsed(bool)), this, SLOT(onViewCollapsed()));
	connect(historyView_, SIGNAL(collapsed(bool)), this, SLOT(onViewCollapsed()));
	connect(addActionButton_, SIGNAL(clicked()), this, SLOT(onAddActionButtonClicked()));
}


#include <QSpacerItem>
void AMWorkflowView3::onViewCollapsed()
{
	// both collapsed but we don't have a spacer? Add it to bottom of layout to keep layout from spreading everything out.
	if(historyView_->isCollapsed() && queueView_->isCollapsed() && !layoutSpacer_) {
		layoutSpacer_ = new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding);
		layout()->addItem(layoutSpacer_);
	}

	// At least one not collapsed, but we do have a spacer? Get rid of it.
	else if((!historyView_->isCollapsed() || !queueView_->isCollapsed()) && layoutSpacer_) {
		layout()->removeItem(layoutSpacer_);
		QSpacerItem* spacer = layoutSpacer_;
		layoutSpacer_ = 0;
		delete spacer;
	}
}

void AMWorkflowView3::onAddActionButtonClicked()
{
	if(!addActionDialog_) {
		addActionDialog_ = new AMAddActionDialog3(this);
		connect(addActionDialog_, SIGNAL(actionAddedFromDialog(AMAction3*)), this, SIGNAL(actionAddedFromDialog(AMAction3*)));
	}
	addActionDialog_->show();
}
