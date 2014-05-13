#include "AMDetectorSelectorView.h"

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "ui/beamline/AMDetectorView.h"
#include "beamline/AMDetectorSelector.h"
#include "beamline/AMDetectorGroup.h"
#include "beamline/AMDetectorSet.h"

 AMDetectorSelectorView::~AMDetectorSelectorView(){}
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

 AMDetectorSelectorViewInternal::~AMDetectorSelectorViewInternal(){}
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
		AMDetectorSelectorViewInternalLineView *tmpDetectorView = 0;

		AMDetector *detector = 0;
		QStringList preferentialOrdering = detectorSelector_->preferentialOrdering();
		for(int x = 0; x < preferentialOrdering.count(); x++){
			detector = detectorSelector_->detectorGroup()->detectorByName(preferentialOrdering.at(x));
			if(detector){
				tmpDetectorView = new AMDetectorSelectorViewInternalLineView(detector, detectorSelector_->detectorIsSelected(detector));
				allDetectorViews_.insert(detector->name(), tmpDetectorView);
				if(detector->isConnected())
					connectedVL_->addWidget(tmpDetectorView);
				else{
					tmpDetectorView->setDetectorConnected(false);
					unconnectedVL_->addWidget(tmpDetectorView);
				}
				connect(tmpDetectorView, SIGNAL(detectorCheckedChanged(bool)), this, SLOT(onDetectorCheckedChanged(bool)));
			}
		}

		AMDetectorSet *connectedDetectors = detectorSelector_->detectorGroup()->connectedDetectors();
		for(int x = 0; x < connectedDetectors->count(); x++){
			if(!preferentialOrdering.contains(connectedDetectors->at(x)->name())){
				tmpDetectorView = new AMDetectorSelectorViewInternalLineView(connectedDetectors->at(x), detectorSelector_->detectorIsSelected(connectedDetectors->at(x)));
				tmpDetectorView->setDetectorSelected(detectorSelector_->detectorIsDefault(connectedDetectors->at(x)));
				allDetectorViews_.insert(connectedDetectors->at(x)->name(), tmpDetectorView);
				connectedVL_->addWidget(tmpDetectorView);
				connect(tmpDetectorView, SIGNAL(detectorCheckedChanged(bool)), this, SLOT(onDetectorCheckedChanged(bool)));
			}
		}

		AMDetectorSet *unconnectedDetectors = detectorSelector_->detectorGroup()->unconnectedDetectors();
		for(int x = 0; x < unconnectedDetectors->count(); x++){
			if(!preferentialOrdering.contains(unconnectedDetectors->at(x)->name())){
				tmpDetectorView = new AMDetectorSelectorViewInternalLineView(unconnectedDetectors->at(x), detectorSelector_->detectorIsSelected(unconnectedDetectors->at(x)));
				tmpDetectorView->setDetectorConnected(false);
				tmpDetectorView->setDetectorSelected(detectorSelector_->detectorIsDefault(unconnectedDetectors->at(x)));
				allDetectorViews_.insert(unconnectedDetectors->at(x)->name(), tmpDetectorView);
				unconnectedVL_->addWidget(tmpDetectorView);
				connect(tmpDetectorView, SIGNAL(detectorCheckedChanged(bool)), this, SLOT(onDetectorCheckedChanged(bool)));
			}
		}

		if(connectedDetectors->count() != 0)
			noneConnectedLabel_->hide();
		if(unconnectedDetectors->count() != 0)
			noneUnconnectedLabel_->hide();

		connect(detectorSelector_, SIGNAL(detectorBecameConnected(AMDetector*)), this, SLOT(onDetectorBecameConnected(AMDetector*)));
		connect(detectorSelector_, SIGNAL(detectorBecameUnconnected(AMDetector*)), this, SLOT(onDetectorBecameUnconnected(AMDetector*)));
		connect(detectorSelector_, SIGNAL(selectedChanged(AMDetector*)), this, SLOT(onDetectorSelectorSelectedChanged(AMDetector*)));
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

	int insertionIndex = -1;
	int preferentialIndex = -1;
	QStringList preferentialOrdering = detectorSelector_->preferentialOrdering();
	for(int x = 0; x < preferentialOrdering.count(); x++)
		if(preferentialOrdering.at(x) == detector->name())
			preferentialIndex = x;


	if(preferentialIndex >= 0)
		insertionIndex = preferentialInsertionIndex(preferentialIndex, connectedVL_);

	if(preferentialIndex == -1)
		connectedVL_->addWidget(tmpDetectorView);
	else if(insertionIndex == -1)
		connectedVL_->insertWidget(0, tmpDetectorView);
	else
		connectedVL_->insertWidget(insertionIndex, tmpDetectorView);

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

	int insertionIndex = -1;
	int preferentialIndex = -1;
	QStringList preferentialOrdering = detectorSelector_->preferentialOrdering();
	for(int x = 0; x < preferentialOrdering.count(); x++)
		if(preferentialOrdering.at(x) == detector->name())
			preferentialIndex = x;

	if(preferentialIndex >= 0)
		insertionIndex = preferentialInsertionIndex(preferentialIndex, unconnectedVL_);

	if(preferentialIndex == -1)
		unconnectedVL_->addWidget(tmpDetectorView);
	else if(insertionIndex == -1)
		unconnectedVL_->insertWidget(0, tmpDetectorView);
	else
		unconnectedVL_->insertWidget(insertionIndex, tmpDetectorView);

	if(detectorSelector_->detectorGroup()->connectedDetectors()->count() == 0)
		noneConnectedLabel_->show();
	if(noneUnconnectedLabel_->isVisible())
		noneUnconnectedLabel_->hide();
}

void AMDetectorSelectorViewInternal::onDetectorCheckedChanged(bool selected){
	AMDetectorSelectorViewInternalLineView *castToLineView = qobject_cast<AMDetectorSelectorViewInternalLineView*>(QObject::sender());
	if(castToLineView)
		detectorSelector_->setDetectorSelectedByName(castToLineView->detectorName(), selected);
}

void AMDetectorSelectorViewInternal::onDetectorSelectorSelectedChanged(AMDetector *detector){
	AMDetectorSelectorViewInternalLineView *lineView = allDetectorViews_.value(detector->name());
	if(lineView)
		lineView->setDetectorSelected(detectorSelector_->detectorIsSelected(detector));
}

int AMDetectorSelectorViewInternal::preferentialInsertionIndex(int preferentialListIndex, QVBoxLayout *layout){
	if(preferentialListIndex == 0)
		return -1;

	QString priorDetectorName = detectorSelector_->preferentialOrdering().at(preferentialListIndex-1);

	AMDetectorSelectorViewInternalLineView *tmpLineView;
	for(int x = 0; x < layout->count(); x++){
		tmpLineView = qobject_cast<AMDetectorSelectorViewInternalLineView*>(layout->itemAt(x)->widget());
		if(tmpLineView && tmpLineView->detectorName() == priorDetectorName)
			return x+1;
	}

	return preferentialInsertionIndex(preferentialListIndex-1, layout);
}

 AMDetectorSelectorViewInternalLineView::~AMDetectorSelectorViewInternalLineView(){}
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
