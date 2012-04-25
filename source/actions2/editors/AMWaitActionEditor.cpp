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


#include "AMWaitActionEditor.h"

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>

AMWaitActionEditor::AMWaitActionEditor(AMWaitActionInfo *info, QWidget *parent)
    : QFrame(parent) {

    info_ = info;
    setFrameStyle(QFrame::StyledPanel);

    spinBox_ = new QDoubleSpinBox();
    spinBox_->setDecimals(1);
    spinBox_->setRange(0, 3600*24*7);

    QHBoxLayout* hl = new QHBoxLayout(this);
    hl->addWidget(new QLabel("Waiting for"));
    hl->addWidget(spinBox_);
    hl->addWidget(new QLabel("seconds."));
    hl->addStretch();

    spinBox_->setValue(info->secondsToWait());
    connect(spinBox_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
}

void AMWaitActionEditor::onSpinBoxEditingFinished()
{
    info_->setSecondsToWait(spinBox_->value());
}
