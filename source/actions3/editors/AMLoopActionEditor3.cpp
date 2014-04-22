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


#include "AMLoopActionEditor3.h"

#include <QSpinBox>
#include <QBoxLayout>
#include <QLabel>

 AMLoopActionEditor3::~AMLoopActionEditor3(){}
AMLoopActionEditor3::AMLoopActionEditor3(AMLoopActionInfo3 *info, QWidget *parent)
	: QFrame(parent) {

	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	spinBox_ = new QSpinBox();
	spinBox_->setRange(1, 9999);

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Looping"));
	hl->addWidget(spinBox_);
	hl->addWidget(new QLabel("times"));
	hl->addStretch();

	spinBox_->setValue(info_->loopCount());
	connect(spinBox_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
	connect(info_, SIGNAL(loopCountChanged(int)), this, SLOT(onLoopCountChanged(int)));
}

void AMLoopActionEditor3::onSpinBoxEditingFinished()
{
	info_->setLoopCount(spinBox_->value());
}

void AMLoopActionEditor3::onLoopCountChanged(int val)
{
	spinBox_->blockSignals(true);
	spinBox_->setValue(val);
	spinBox_->blockSignals(false);
}
