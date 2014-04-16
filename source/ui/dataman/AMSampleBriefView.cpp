#include "AMSampleBriefView.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>

#include "dataman/AMSample.h"
#include "dataman/database/AMConstDbObject.h"

AMSampleBriefView::AMSampleBriefView(const AMSample *sample, QWidget *parent) :
	QWidget(parent)
{
	sample_ = 0;

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
		}
	}
}
