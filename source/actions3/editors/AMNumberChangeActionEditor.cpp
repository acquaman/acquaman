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


#include "AMNumberChangeActionEditor.h"

#include <QLabel>
#include <QSpinBox>
#include <QBoxLayout>

#include "actions3/actions/AMNumberChangeAction.h"

 AMNumberChangeActionEditor::~AMNumberChangeActionEditor(){}
AMNumberChangeActionEditor::AMNumberChangeActionEditor(AMNumberChangeActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout* hl = new QHBoxLayout(this);

	descriptionLabel_ = new QLabel(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
	hl->addWidget(descriptionLabel_);
	hl->addWidget(new QLabel("Index: "));
	indexSpinBox_ = new QSpinBox();
	indexSpinBox_->setRange(0, AMNumberChangeActionSupport::AMNumberChangeActionList()->count()-1);
	indexSpinBox_->setValue(info_->indexToChange());
	hl->addWidget(indexSpinBox_);
	hl->addWidget(new QLabel("New Number: "));
	newNumberSpinBox_ = new QSpinBox();
	newNumberSpinBox_->setRange(-9999, 9999);
	newNumberSpinBox_->setValue(info_->newNumber());
	hl->addWidget(newNumberSpinBox_);
	hl->addStretch(0);

	connect(indexSpinBox_, SIGNAL(editingFinished()), this, SLOT(onIndexSpinBoxEditingFinished()));
	connect(newNumberSpinBox_, SIGNAL(editingFinished()), this, SLOT(onNewNumberSpinBoxEditingFinished()));
}

void AMNumberChangeActionEditor::onIndexSpinBoxEditingFinished(){
	info_->setIndexToChange(indexSpinBox_->value());
	descriptionLabel_->setText(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
}

void AMNumberChangeActionEditor::onNewNumberSpinBoxEditingFinished(){
	info_->setNewNumber(newNumberSpinBox_->value());
	descriptionLabel_->setText(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
}
