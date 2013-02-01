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

	connectedGroupBox_ = new QGroupBox("Connected");
	connectedVL_ = new QVBoxLayout();
	noneConnectedLabel_ = new QLabel("<None>");
	connectedVL_->addWidget(noneConnectedLabel_);
	unconnectedGroupBox_ = new QGroupBox("Unconnected");
	unconnectedVL_ = new QVBoxLayout();
	noneUnconnectedLabel_ = new QLabel("<None>");
	unconnectedVL_->addWidget(noneUnconnectedLabel_);

	if(detectorSelector_){
		AMDetectorSelectorViewInternalLineView *tmpDetectorView;

		AMDetectorSet *connectedDetectors = detectorSelector_->detectorGroup()->connectedDetectors();
		for(int x = 0; x < connectedDetectors->count(); x++){
			tmpDetectorView = new AMDetectorSelectorViewInternalLineView(connectedDetectors->at(x), detectorSelector_->detectorIsSelected(connectedDetectors->at(x)));
			tmpDetectorView->setDetectorSelected(detectorSelector_->detectorIsDefault(connectedDetectors->at(x)));
			allDetectorViews_.insert(connectedDetectors->at(x)->name(), tmpDetectorView);
			connectedVL_->addWidget(tmpDetectorView);
		}

		AMDetectorSet *unconnectedDetectors = detectorSelector_->detectorGroup()->unconnectedDetectors();
		for(int x = 0; x < unconnectedDetectors->count(); x++){
			tmpDetectorView = new AMDetectorSelectorViewInternalLineView(unconnectedDetectors->at(x), detectorSelector_->detectorIsSelected(unconnectedDetectors->at(x)));
			tmpDetectorView->setDetectorConnected(false);
			tmpDetectorView->setDetectorSelected(detectorSelector_->detectorIsDefault(unconnectedDetectors->at(x)));
			allDetectorViews_.insert(unconnectedDetectors->at(x)->name(), tmpDetectorView);
			unconnectedVL_->addWidget(tmpDetectorView);
		}

		if(connectedDetectors->count() != 0)
			noneConnectedLabel_->hide();
		if(unconnectedDetectors->count() != 0)
			noneUnconnectedLabel_->hide();

		connect(detectorSelector_, SIGNAL(detectorBecameConnected(AMDetector*)), this, SLOT(onDetectorBecameConnected(AMDetector*)));
		connect(detectorSelector_, SIGNAL(detectorBecameUnconnected(AMDetector*)), this, SLOT(onDetectorBecameUnconnected(AMDetector*)));
	}

	QVBoxLayout *mainVL = new QVBoxLayout();
	connectedGroupBox_->setLayout(connectedVL_);
	mainVL->addWidget(connectedGroupBox_);
	unconnectedGroupBox_->setLayout(unconnectedVL_);
	mainVL->addWidget(unconnectedGroupBox_);
	setLayout(mainVL);
}

void AMDetectorSelectorViewInternal::onDetectorBecameConnected(AMDetector *detector){
	if(!detector || !detectorSelector_ || !allDetectorViews_.contains(detector->name()))
		return;

	AMDetectorSelectorViewInternalLineView *tmpDetectorView = allDetectorViews_.value(detector->name());
	tmpDetectorView->setDetectorConnected(true);
	unconnectedVL_->removeWidget(tmpDetectorView);
	connectedVL_->addWidget(tmpDetectorView);

	if(detectorSelector_->detectorGroup()->unconnectedDetectors()->count() == 0)
		noneUnconnectedLabel_->show();
	if(noneConnectedLabel_->isVisible())
		noneConnectedLabel_->hide();
}

void AMDetectorSelectorViewInternal::onDetectorBecameUnconnected(AMDetector *detector){
	if(!detector || !detectorSelector_ || !allDetectorViews_.contains(detector->name()))
		return;

	AMDetectorSelectorViewInternalLineView *tmpDetectorView = allDetectorViews_.value(detector->name());
	tmpDetectorView->setDetectorConnected(false);
	connectedVL_->removeWidget(tmpDetectorView);
	unconnectedVL_->addWidget(tmpDetectorView);

	if(detectorSelector_->detectorGroup()->connectedDetectors()->count() == 0)
		noneConnectedLabel_->show();
	if(noneUnconnectedLabel_->isVisible())
		noneUnconnectedLabel_->hide();
}

AMDetectorSelectorViewInternalLineView::AMDetectorSelectorViewInternalLineView(AMDetector *detector, bool isSelected, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	detectorSelectedCheckBox_ = new QCheckBox();
	detectorSelectedCheckBox_->setChecked(isSelected);
	briefView_ = new AMDetectorGeneralBriefView(detector);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(briefView_);
	hl->addWidget(detectorSelectedCheckBox_);
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
