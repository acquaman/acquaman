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


#include "AMLiveLoopActionEditor3.h"

#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>

 AMLiveLoopActionEditor3::~AMLiveLoopActionEditor3(){}
AMLiveLoopActionEditor3::AMLiveLoopActionEditor3(AMLoopAction3 *action, QWidget *parent)
	: QFrame(parent)
{
	action_ = action;
	setFrameStyle(QFrame::StyledPanel);

	loopCount_ = new QSpinBox();
	loopCount_->setRange(action_->currentIteration()+1, 9999);
	loopCount_->setValue(action_->loopCount());

	iteration_ = new QLabel(QString("%1").arg(action_->currentIteration()+1));

	connect(action_, SIGNAL(currentIterationChanged(int)), this, SLOT(onIterationUpdate(int)));
	connect(action_->info(), SIGNAL(infoChanged()), this, SLOT(onLoopCountChanged()));
	connect(loopCount_, SIGNAL(editingFinished()), this, SLOT(setLoopCount()));

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(new QLabel("Current: "));
	hl->addWidget(iteration_);
	hl->addWidget(new QLabel(" of "));
	hl->addWidget(loopCount_);
	hl->addStretch();

	setLayout(hl);
}

void AMLiveLoopActionEditor3::onIterationUpdate(int val)
{
	loopCount_->setMinimum(val+1);
	iteration_->setText(QString::number(val+1));
}

void AMLiveLoopActionEditor3::onLoopCountChanged()
{
	loopCount_->blockSignals(true);
	loopCount_->setValue(action_->loopCount());
	loopCount_->blockSignals(false);
}

void AMLiveLoopActionEditor3::setLoopCount()
{
	action_->setLoopCount(loopCount_->value());
}
