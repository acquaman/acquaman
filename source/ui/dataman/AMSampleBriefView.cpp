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


#include "AMSampleBriefView.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>

#include "dataman/AMSample.h"
#include "dataman/database/AMConstDbObject.h"
#include "ui/dataman/AMSampleView.h"

AMSampleBriefView::AMSampleBriefView(const AMSample *sample, QWidget *parent) :
	QWidget(parent)
{
	sample_ = 0; //NULL
	sampleReadOnlyView_ = 0; //NULL

	QLabel *nameTitleLabel = new QLabel("Name: ");
	nameTitleLabel->setObjectName("AMSampleBriefViewLabel");
	nameLabel_ = new QLabel("No Sample");
	showSampleInformationButton_ = new QPushButton("Show Sample Info");

	QVBoxLayout *mainVL = new QVBoxLayout();
	QHBoxLayout *nameHL = new QHBoxLayout();
	nameHL->addWidget(nameTitleLabel);
	nameHL->addWidget(nameLabel_);
	nameHL->addStretch();
	mainVL->addLayout(nameHL);
	mainVL->addWidget(showSampleInformationButton_);

	showSampleInformationButton_->hide();
	connect(showSampleInformationButton_, SIGNAL(clicked()), this, SLOT(onShowSampleInformationButtonClicked()));
	setSample(sample);

	setLayout(mainVL);

	setStyleSheet( "#AMSampleBriefViewLabel {"
				   "color: rgb(121, 121, 121);"
				   "font-weight: bold;"
				   "font-family: \"Lucida Grande\"; }");
}

void AMSampleBriefView::setSample(const AMSample *sample){
	if(sample_ != sample){
		sample_ = sample;

		if(sample_){
			nameLabel_->setText(sample_->name());
			showSampleInformationButton_->show();
		}
		else{
			nameLabel_->setText("No Sample");
			showSampleInformationButton_->hide();
			sampleReadOnlyView_->deleteLater();
			sampleReadOnlyView_ = 0;
		}
	}
}

void AMSampleBriefView::onShowSampleInformationButtonClicked(){
	if(!sampleReadOnlyView_ && sample_)
		sampleReadOnlyView_ = new AMSampleReadOnlyView(sample_);
	sampleReadOnlyView_->show();
}
