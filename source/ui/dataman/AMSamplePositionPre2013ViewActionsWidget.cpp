/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "ui_AMSamplePositionViewActionsWidget.h"

#include <QMenu>

AMSamplePositionPre2013ViewActionsWidget::AMSamplePositionPre2013ViewActionsWidget(int row, QWidget *parent) :
	QFrame(parent),
	ui(new Ui::AMSamplePositionViewActionsWidget)
{
	row_ = row;

	ui->setupUi(this);

	QStyle* style = QApplication::style();
	ui->markButton->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
	ui->moveToButton->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
	ui->removeButton->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));

	ui->moveToButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ui->markButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	ui->markButton->setToolTip("Memorize the current position and associate it with this sample");
	ui->moveToButton->setToolTip("Move to this sample's stored position now");
	ui->removeButton->setToolTip("Remove this sample from the sample plate");


	connect(ui->markButton, SIGNAL(clicked()), this, SLOT(onMarkButtonPressed()));
	connect(ui->moveToButton, SIGNAL(clicked()), this, SLOT(onMoveToButtonPressed()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonPressed()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

AMSamplePositionPre2013ViewActionsWidget::~AMSamplePositionPre2013ViewActionsWidget()
{
	delete ui;
}

void AMSamplePositionPre2013ViewActionsWidget::onCustomContextMenuRequested(QPoint point){
	QMenu popup(this);
	QAction *tempAction = popup.addAction("Additional Information");
	tempAction = popup.exec(this->mapToGlobal(point));

	if(tempAction && (tempAction->text() == "Additional Information") )
		emit additionalInformationRequested(row_);
}
