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


#include "AMSampleEditor.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePre2013.h"
#include "util/AMDateTimeUtils.h"

AMSampleEditor::AMSampleEditor(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	database_ = database;
	currentSample_ = 0; //NULL
	currentSamplePre2013_ = 0; //NULL

	nameLabel_ = new QLabel("No Sample");
	createdLabel_ = new QLabel("No Sample");
	changeSampleButton_ = new QPushButton("Change Sample");
	connect(changeSampleButton_, SIGNAL(clicked()), this, SLOT(onChangeSampleButtonClicked()));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(nameLabel_);
	vl->addWidget(createdLabel_);
	vl->addWidget(changeSampleButton_);
	setLayout(vl);
}

AMSampleEditor::~AMSampleEditor(){}

void AMSampleEditor::setSample(const AMSample *sample){
	currentSample_ = sample;
	nameLabel_->setText(currentSample_->name());
	createdLabel_->setText(AMDateTimeUtils::prettyDateTime(currentSample_->dateTime()));
}

void AMSampleEditor::setSamplePre2013(const AMSamplePre2013 *samplePre2013){
	currentSamplePre2013_ = samplePre2013;
	nameLabel_->setText(currentSamplePre2013_->name());
	createdLabel_->setText(AMDateTimeUtils::prettyDateTime(currentSamplePre2013_->dateTime()));
}

void AMSampleEditor::onChangeSampleButtonClicked(){
}
