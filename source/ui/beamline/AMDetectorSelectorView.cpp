#include "AMDetectorSelectorView.h"

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "ui/beamline/AMDetectorView.h"

AMDetectorSelectorView::AMDetectorSelectorView(AMDetectorSelector *detectorSelector, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMDetectorSelectorViewInternal(detectorSelector);

	setTitle(detectorSelector->detectorGroup()->name());
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(internalView_);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(scrollArea_);
	vl->setContentsMargins(0, 0, 0, 0);
	setLayout(vl);
}

AMDetectorSelectorViewInternal::AMDetectorSelectorViewInternal(AMDetectorSelector *detectorSelector, QWidget *parent) :
	QWidget(parent)
{
	detectorSelector_ = detectorSelector;

	connectedVL_ = new QVBoxLayout();
	unconnectedVL_ = new QVBoxLayout();

	if(detectorSelector_){
		AMDetectorSelectorViewInternalLineView *tmpDetectorView;

		AMDetectorSet *connectedDetectors = detectorSelector_->detectorGroup()->connectedDetectors();
		for(int x = 0; x < connectedDetectors->count(); x++){
			tmpDetectorView = new AMDetectorSelectorViewInternalLineView(connectedDetectors->at(x), detectorSelector_->detectorIsSelected(connectedDetectors->at(x)));
			allDetectorViews_.insert(connectedDetectors->at(x)->name(), tmpDetectorView);
			connectedVL_->addWidget(tmpDetectorView);
		}

		AMDetectorSet *unconnectedDetectors = detectorSelector_->detectorGroup()->unconnectedDetectors();
		for(int x = 0; x < unconnectedDetectors->count(); x++){
			tmpDetectorView = new AMDetectorSelectorViewInternalLineView(unconnectedDetectors->at(x), detectorSelector_->detectorIsSelected(unconnectedDetectors->at(x)));
			tmpDetectorView->setDetectorConnected(false);
			allDetectorViews_.insert(unconnectedDetectors->at(x)->name(), tmpDetectorView);
			unconnectedVL_->addWidget(tmpDetectorView);
		}

		connect(detectorSelector_, SIGNAL(detectorBecameConnected(AMDetector*)), this, SLOT(onDetectorBecameConnected(AMDetector*)));
		connect(detectorSelector_, SIGNAL(detectorBecameUnconnected(AMDetector*)), this, SLOT(onDetectorBecameUnconnected(AMDetector*)));
	}

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(new QLabel("Connected"));
	mainVL->addLayout(connectedVL_);
	mainVL->addWidget(new QLabel("Unconnected"));
	mainVL->addLayout(unconnectedVL_);
	setLayout(mainVL);
}

void AMDetectorSelectorViewInternal::onDetectorBecameConnected(AMDetector *detector){
	if(!detector || !detectorSelector_ || !allDetectorViews_.contains(detector->name()))
		return;

	AMDetectorSelectorViewInternalLineView *tmpDetectorView = allDetectorViews_.value(detector->name());
	tmpDetectorView->setDetectorConnected(true);
	unconnectedVL_->removeWidget(tmpDetectorView);
	connectedVL_->addWidget(tmpDetectorView);
}

void AMDetectorSelectorViewInternal::onDetectorBecameUnconnected(AMDetector *detector){
	if(!detector || !detectorSelector_ || !allDetectorViews_.contains(detector->name()))
		return;

	AMDetectorSelectorViewInternalLineView *tmpDetectorView = allDetectorViews_.value(detector->name());
	tmpDetectorView->setDetectorConnected(false);
	connectedVL_->removeWidget(tmpDetectorView);
	unconnectedVL_->addWidget(tmpDetectorView);
}

AMDetectorSelectorViewInternalLineView::AMDetectorSelectorViewInternalLineView(AMDetector *detector, bool isSelected, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	detectorSelectedCheckBox_ = new QCheckBox();
	detectorSelectedCheckBox_->setChecked(isSelected);
	briefView_ = new AMDetectorGeneralBriefView(detector);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(detectorSelectedCheckBox_);
	hl->addWidget(briefView_);
	hl->setContentsMargins(0,0,0,0);

	connect(detectorSelectedCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(detectorCheckedChanged(bool)));

	setLayout(hl);
}

QString AMDetectorSelectorViewInternalLineView::detectorName() const{
	return detector_->name();
}

bool AMDetectorSelectorViewInternalLineView::detectorChecked(){
	return detectorSelectedCheckBox_->isChecked();
}

void AMDetectorSelectorViewInternalLineView::setDetectorSelected(bool selected){
	detectorSelectedCheckBox_->setChecked(selected);
}

void AMDetectorSelectorViewInternalLineView::setDetectorConnected(bool connected){
	briefView_->setEnabled(connected);
}
