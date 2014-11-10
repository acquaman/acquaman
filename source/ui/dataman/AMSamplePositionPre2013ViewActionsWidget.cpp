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

	if (objectName().isEmpty())
				setObjectName(QString::fromUtf8("AMSamplePositionViewActionsWidget"));
	resize(214, 30);
	setAutoFillBackground(false);
	setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255));\n"
	"border: 1px solid rgb(102, 102, 102);"));
	horizontalLayout_ = new QHBoxLayout(this);
	horizontalLayout_->setSpacing(2);
	horizontalLayout_->setObjectName(QString::fromUtf8("horizontalLayout_"));
	horizontalLayout_->setContentsMargins(4, 2, 4, 3);
	markButton_ = new QToolButton(this);
	markButton_->setObjectName(QString::fromUtf8("markButton_"));

	horizontalLayout_->addWidget(markButton_);

	moveToButton_ = new QToolButton(this);
	moveToButton_->setObjectName(QString::fromUtf8("moveToButton_"));

	horizontalLayout_->addWidget(moveToButton_);

	QSpacerItem* horizontalSpacer = new QSpacerItem(31, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	horizontalLayout_->addItem(horizontalSpacer);

	removeButton_ = new QToolButton(this);
	removeButton_->setObjectName(QString::fromUtf8("removeButton_"));

	horizontalLayout_->addWidget(removeButton_);

	setWindowTitle(QApplication::translate("AMSamplePositionViewActionsWidget", "Sample Position Actions", 0, QApplication::UnicodeUTF8));
	markButton_->setText(QApplication::translate("AMSamplePositionViewActionsWidget", "Mark", 0, QApplication::UnicodeUTF8));
	moveToButton_->setText(QApplication::translate("AMSamplePositionViewActionsWidget", "Move To", 0, QApplication::UnicodeUTF8));
	removeButton_->setText(QApplication::translate("AMSamplePositionViewActionsWidget", "Remove", 0, QApplication::UnicodeUTF8));

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
