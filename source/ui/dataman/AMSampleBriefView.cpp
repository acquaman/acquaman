#include "AMSampleBriefView.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>

#include "dataman/AMSample.h"

AMSampleBriefView::AMSampleBriefView(AMSample *sample, QWidget *parent) :
	QWidget(parent)
{
	sample_ = sample;

	nameLabel_ = new QLabel("No Sample");

	showSampleInformationButton_ = new QPushButton("Show Sample Info");

	QVBoxLayout *mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(nameLabel_);
	if(sample_){
		nameLabel_->setText(sample_->name());
		mainVL_->addWidget(showSampleInformationButton_);
	}

	setLayout(mainVL_);
}
