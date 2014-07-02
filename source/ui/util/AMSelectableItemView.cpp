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


#include "AMSelectableItemView.h"

 AMSelectableItemView::~AMSelectableItemView(){}
AMSelectableItemView::AMSelectableItemView(QWidget *parent)
	: QWidget(parent)
{
	checkBox_ = new QCheckBox;

	connect(checkBox_, SIGNAL(toggled(bool)), this, SIGNAL(itemSelected(bool)));

	itemLayout_ = new QHBoxLayout;
	itemLayout_->addWidget(checkBox_);
	itemLayout_->addStretch();

	setLayout(itemLayout_);
	setMaximumHeight(35);
}

AMSelectableItemView::AMSelectableItemView(const QString &text, bool checked, QWidget *parent)
	: QWidget(parent)
{
	checkBox_ = new QCheckBox(text);
	checkBox_->setChecked(checked);

	connect(checkBox_, SIGNAL(toggled(bool)), this, SIGNAL(itemSelected(bool)));

	itemLayout_ = new QHBoxLayout;
	itemLayout_->addWidget(checkBox_);
	itemLayout_->addStretch();

	setLayout(itemLayout_);
	setMaximumHeight(35);
}

void AMSelectableItemView::setText(const QString &text)
{
	checkBox_->setText(text);
}

void AMSelectableItemView::setSelected(bool isSelected)
{
	checkBox_->setChecked(isSelected);
}
