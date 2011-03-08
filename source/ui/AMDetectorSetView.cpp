#include "AMDetectorSetView.h"

AMDetectorSetView::AMDetectorSetView(AMDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	setTitle(viewSet->name());
	//vl_ = new QVBoxLayout();
	gl_ = new QGridLayout();
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->at(x);
		tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD);
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD);
			detectorDetails_.append(tmpDetails);
			connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
		}
		if(configureOnly_)
			tmpCheck = new QCheckBox();
		detectorBoxes_.append(tmpDV);
		gl_->addWidget(tmpLabel,		x, 0, 1, 1, 0);
		gl_->addWidget(tmpDV,			x, 2, 1, 1, 0);
		if(configureOnly_)
			gl_->addWidget(tmpCheck,	x, 1, 1, 1, 0);
		gl_->addWidget(tmpButton,		x, 3, 1, 1, 0);
		//connect(tmpCE, SIGNAL(setpointRequested(double)), this, SLOT(onConfigurationValueChanged()));
	}

	gl_->setRowStretch(viewSet_->count(), 10);
	gl_->setVerticalSpacing(1);
	gl_->setHorizontalSpacing(1);
	setLayout(gl_);

	connect(viewSet_, SIGNAL(detectorAdded(int)), this, SLOT(onDetectorAddedToSet(int)));
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	//connect(viewSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onControlSetValuesChanged(AMControlInfoList)));
}

void AMDetectorSetView::onDetectorAddedToSet(int index){
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	tmpD = viewSet_->at(index);
	tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD);
	tmpLabel = new QLabel(tmpD->description());
	tmpButton = new QPushButton("Details");
	if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
		tmpButton->setEnabled(false);
		detectorDetails_.append(0);
	}
	else{
		tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD);
		detectorDetails_.append(tmpDetails);
		connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
	}
	if(configureOnly_)
		tmpCheck = new QCheckBox();
	detectorBoxes_.insert(index, tmpDV);
	gl_->addWidget(tmpLabel,		index, 0, 1, 1, 0);
	gl_->addWidget(tmpDV,			index, 2, 1, 1, 0);
	if(configureOnly_)
		gl_->addWidget(tmpCheck,	index, 1, 1, 1, 0);
	gl_->addWidget(tmpButton,		index, 3, 1, 1, 0);

	for(int x = 0; x < viewSet_->count()-1; x++){
		gl_->setRowStretch(x ,1);
	}
	gl_->setRowStretch(viewSet_->count(), 10);
}

void AMDetectorSetView::onControlSetValuesChanged(AMDetectorInfoSet infoList){

}

void AMDetectorSetView::onConfigurationValueChanged(){

}
