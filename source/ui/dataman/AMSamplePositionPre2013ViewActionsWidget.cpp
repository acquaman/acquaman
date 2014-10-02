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


#include "AMSamplePositionPre2013ViewActionsWidget.h"

#include <QMenu>
#include <QApplication>
#include <QStyle>

AMSamplePositionPre2013ViewActionsWidget::AMSamplePositionPre2013ViewActionsWidget(int row, QWidget *parent) :
	QFrame(parent)
{
	row_ = row;

	QStyle* style = QApplication::style();
	markButton_->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
	moveToButton_->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
	removeButton_->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));

	moveToButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	markButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	markButton_->setToolTip("Memorize the current position and associate it with this sample");
	moveToButton_->setToolTip("Move to this sample's stored position now");
	removeButton_->setToolTip("Remove this sample from the sample plate");


	connect(markButton_, SIGNAL(clicked()), this, SLOT(onMarkButtonPressed()));
	connect(moveToButton_, SIGNAL(clicked()), this, SLOT(onMoveToButtonPressed()));
	connect(removeButton_, SIGNAL(clicked()), this, SLOT(onRemoveButtonPressed()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

AMSamplePositionPre2013ViewActionsWidget::~AMSamplePositionPre2013ViewActionsWidget()
{

}

void AMSamplePositionPre2013ViewActionsWidget::onCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *tempAction = popup.addAction("Additional Information");
	tempAction = popup.exec(this->mapToGlobal(point));

	if(tempAction && (tempAction->text() == "Additional Information") )
		emit additionalInformationRequested(row_);
}
