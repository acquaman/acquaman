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


#include "AMSampleMoveActionEditor.h"

#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>

#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"

AMSampleMoveActionEditor::AMSampleMoveActionEditor(AMSampleMoveActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout *hl = new QHBoxLayout();
	sampleSelectorBox_ = new QComboBox();
	hl->addWidget(new QLabel("Sample: "));
	hl->addWidget(sampleSelectorBox_);
	hl->addStretch(0);

	setLayout(hl);

	populateSamples();

	AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();

	if(info_->sample() && currentPlate){
		for(int x = 0, size = currentPlate->sampleCount(); x < size; x++)
			if(currentPlate->sampleAt(x) == info_->sample())
				sampleSelectorBox_->setCurrentIndex(x+1);
	}
	else
		sampleSelectorBox_->setCurrentIndex(0);

	connect(sampleSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSampleSeletectorBoxCurrentIndexChanged(int)));
}

AMSampleMoveActionEditor::~AMSampleMoveActionEditor()
{
}

void AMSampleMoveActionEditor::onSampleSeletectorBoxCurrentIndexChanged(int index){
	AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
	if(currentPlate && info_){
		if(index == 0)
			info_->setSample(0);
		else
			info_->setSample(currentPlate->sampleAt(sampleSelectorBox_->itemData(index).toInt()));
	}
}

void AMSampleMoveActionEditor::populateSamples(){
	if(!AMBeamline::bl()->samplePlate()){
		sampleSelectorBox_->addItem(QString("No Sample Plate"), QVariant(-1));
		sampleSelectorBox_->setEnabled(false);
	}
	else if(AMBeamline::bl()->samplePlate()->sampleCount() == 0){
		sampleSelectorBox_->addItem(QString("No Samples on Plate"), QVariant(-1));
		sampleSelectorBox_->setEnabled(false);
	}
	else{
		sampleSelectorBox_->addItem(QString("No Sample Selected"), QVariant(0));
		AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
		for(int x = 0, size = currentPlate->sampleCount(); x < size; x++)
			sampleSelectorBox_->addItem(QString(currentPlate->sampleAt(x)->name()), QVariant(x));
	}
}
