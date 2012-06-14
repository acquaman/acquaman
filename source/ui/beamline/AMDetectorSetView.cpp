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


#include "AMDetectorSetView.h"

AMDetectorSetView::AMDetectorSetView(AMDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMDetectorSetViewInternal(viewSet, configureOnly);
	initializeWidget(viewSet);
}

AMDetectorSetView::AMDetectorSetView(AMDetectorSet *viewSet, const AMDetectorInfoSet &infoSet, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMDetectorSetViewInternal(viewSet, infoSet);
	initializeWidget(viewSet);
}

int AMDetectorSetView::count() const{
	return internalView_->count();
}

AMDetectorSet* AMDetectorSetView::detectorSet(){
	return internalView_->detectorSet();
}

AMDetectorView* AMDetectorSetView::boxByName(const QString &name){
	return internalView_->boxByName(name);
}

const AMDetectorView* AMDetectorSetView::boxAt(int row) const{
	return internalView_->boxAt(row);
}

AMDetectorView* AMDetectorSetView::detailByName(const QString &name){
	return internalView_->detailByName(name);
}

const AMDetectorView* AMDetectorSetView::detailAt(int row) const{
	return internalView_->detailAt(row);
}

bool AMDetectorSetView::checkedAt(int row) const{
	return internalView_->checkedAt(row);
}

AMDetectorInfoSet AMDetectorSetView::currentValues(){
	return internalView_->currentValues();
}

AMDetectorInfoSet AMDetectorSetView::configValues(){
	return internalView_->configValues();
}

QDebug operator<<(QDebug d, const AMDetectorSetView& dsv){
	d << dsv.internalView_;
	return d;
}

void AMDetectorSetView::initializeWidget(AMDetectorSet *viewSet){
	setTitle(viewSet->name());
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(internalView_);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(internalView_, SIGNAL(currentValuesChanged()), this, SIGNAL(currentValuesChanged()));
	connect(internalView_, SIGNAL(configValuesChanged()), this, SIGNAL(configValuesChanged()));

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(scrollArea_);
	mainVL->setContentsMargins(0,0,0,0);
	setLayout(mainVL);
}

/*
QSize AMDetectorSetView::sizeHint() const{
	QSize internalSize = internalView_->sizeHint();
	internalSize.setWidth(internalSize.width()+10);
	return internalSize;
}*/

AMDetectorSetViewInternal::AMDetectorSetViewInternal(AMDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
	QWidget(parent)
	//QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	gl_ = new QGridLayout();
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
			detectorDetails_.append(tmpDetails);
			connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
			connect(tmpDetails, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		if(configureOnly_){
			tmpCheck = new QCheckBox();
			if(viewSet_->isDefaultAt(x))
				tmpCheck->setChecked(true);
			checkBoxes_.append(tmpCheck);
			connect(tmpCheck, SIGNAL(toggled(bool)), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		detectorBoxes_.append(tmpDV);

		gl_->addWidget(tmpLabel,		x, 0, 1, 1, 0);
		gl_->addWidget(tmpDV,			x, 2, 1, 1, 0);
		if(configureOnly_)
			gl_->addWidget(tmpCheck,	x, 1, 1, 1, 0);
		gl_->addWidget(tmpButton,		x, 3, 1, 1, 0);
	}

	gl_->setRowStretch(viewSet_->count(), 10);
	gl_->setVerticalSpacing(1);
	gl_->setHorizontalSpacing(1);
	setLayout(gl_);

	connect(viewSet_, SIGNAL(detectorAdded(int)), this, SLOT(onDetectorAddedToSet(int)));
	connect(viewSet_, SIGNAL(detectorRemoved(int)), this, SLOT(onDetectorRemovedFromSet(int)));
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

AMDetectorSetViewInternal::AMDetectorSetViewInternal(AMDetectorSet *viewSet, const AMDetectorInfoSet &infoSet, QWidget *parent) :
	QWidget(parent)
//	QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = true;
	gl_ = new QGridLayout();
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
			detectorDetails_.append(tmpDetails);
			connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
			connect(tmpDetails, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		if(configureOnly_){
			tmpCheck = new QCheckBox();
			if(infoSet.isActiveAt(x))
				tmpCheck->setChecked(true);
			checkBoxes_.append(tmpCheck);
			connect(tmpCheck, SIGNAL(toggled(bool)), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		detectorBoxes_.append(tmpDV);

		gl_->addWidget(tmpLabel,		x, 0, 1, 1, 0);
		gl_->addWidget(tmpDV,			x, 2, 1, 1, 0);
		if(configureOnly_)
			gl_->addWidget(tmpCheck,	x, 1, 1, 1, 0);
		gl_->addWidget(tmpButton,		x, 3, 1, 1, 0);
	}

	gl_->setRowStretch(viewSet_->count(), 10);
	gl_->setVerticalSpacing(1);
	gl_->setHorizontalSpacing(1);
	setLayout(gl_);

	connect(viewSet_, SIGNAL(detectorAdded(int)), this, SLOT(onDetectorAddedToSet(int)));
	connect(viewSet_, SIGNAL(detectorRemoved(int)), this, SLOT(onDetectorRemovedFromSet(int)));
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

int AMDetectorSetViewInternal::count() const{
	return viewSet_->count();
}

AMDetectorSet* AMDetectorSetViewInternal::detectorSet(){
	return viewSet_;
}

AMDetectorView* AMDetectorSetViewInternal::boxByName(const QString &name){
	return detectorBoxes_.at(viewSet_->indexOfKey(name));
}

const AMDetectorView *AMDetectorSetViewInternal::boxAt(int row) const{
	return detectorBoxes_.at(row);
}

AMDetectorView* AMDetectorSetViewInternal::detailByName(const QString &name){
	return detectorDetails_.at(viewSet_->indexOf(name));
}

const AMDetectorView *AMDetectorSetViewInternal::detailAt(int row) const{
	return detectorDetails_.at(row);
}

bool AMDetectorSetViewInternal::checkedAt(int row) const{
	if(!configureOnly_)
		return false;
	if(checkBoxes_.at(row))
		return checkBoxes_.at(row)->isChecked();
	return false;
}

AMDetectorInfoSet AMDetectorSetViewInternal::currentValues(){
	return viewSet_->toInfoSet();
}

AMDetectorInfoSet AMDetectorSetViewInternal::configValues(){
	AMDetectorInfoSet rv;

	if(!configureOnly_)
		return currentValues();

	for(int x = 0; x < viewSet_->count(); x++){
		if(detailAt(x))
			rv.addDetectorInfo(detailAt(x)->configurationSettings(), checkedAt(x));
		else
			rv.addDetectorInfo(boxAt(x)->configurationSettings(), checkedAt(x));
	}
	return rv;
}

QDebug operator<<(QDebug d, const AMDetectorSetViewInternal& dsv){
	for(int x = 0; x < dsv.count(); x++){
		if(dsv.boxAt(x) && dsv.boxAt(x)->configurationSettings())
			d << *(dsv.boxAt(x)->configurationSettings());
		if(dsv.detailAt(x) && dsv.detailAt(x)->configurationSettings())
			d << *(dsv.detailAt(x)->configurationSettings());
	}
	return d;
}

void AMDetectorSetViewInternal::setDisabled(bool disabled){
	for(int x = checkBoxes_.count()-1; x >= 0; x--){
		checkBoxes_[x]->setDisabled(disabled);
		if(detectorDetails_[x])
			gl_->itemAtPosition(x, 3)->widget()->setDisabled(disabled);
	}
}

void AMDetectorSetViewInternal::onDetectorAddedToSet(int index){
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	tmpD = viewSet_->detectorAt(index);
	tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
	connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
	tmpLabel = new QLabel(tmpD->description());
	tmpButton = new QPushButton("Details");
	if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
		tmpButton->setEnabled(false);
		detectorDetails_.insert(index, 0);
	}
	else{
		tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
		detectorDetails_.insert(index, tmpDetails);
		connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
		connect(tmpDetails, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
	}
	if(configureOnly_){
		tmpCheck = new QCheckBox();
		if(viewSet_->isDefaultAt(index))
			tmpCheck->setChecked(true);
		checkBoxes_.insert(index, tmpCheck);
		connect(tmpCheck, SIGNAL(toggled(bool)), this, SLOT(onDetectorSetConfigurationRequested()));
	}
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

/* NTBA - August 25th, 2011 (David Chevrier)
  Working, but SUPER INNEFFICIENT. Redraws the whole thing every time"
*/
void AMDetectorSetViewInternal::onDetectorRemovedFromSet(int index){
	Q_UNUSED(index)
	for(int x = gl_->rowCount()-1; x >= 0; x--){
		if(gl_->itemAtPosition(x, 0))
			gl_->itemAtPosition(x, 0)->widget()->hide();
		if(gl_->itemAtPosition(x, 1))
			gl_->itemAtPosition(x, 1)->widget()->hide();
		if(gl_->itemAtPosition(x, 2))
			gl_->itemAtPosition(x, 2)->widget()->hide();
		if(gl_->itemAtPosition(x, 3))
			gl_->itemAtPosition(x, 3)->widget()->hide();
	}
	delete layout();
	detectorBoxes_.clear();
	detectorDetails_.clear();
	checkBoxes_.clear();
	gl_ = new QGridLayout();
	AMDetectorView *tmpDV;
	AMDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
			detectorDetails_.append(tmpDetails);
			connect(tmpButton, SIGNAL(clicked()), tmpDetails, SLOT(show()));
			connect(tmpDetails, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		if(configureOnly_){
			tmpCheck = new QCheckBox();
			if(viewSet_->isDefaultAt(x))
				tmpCheck->setChecked(true);
			checkBoxes_.append(tmpCheck);
			connect(tmpCheck, SIGNAL(toggled(bool)), this, SLOT(onDetectorSetConfigurationRequested()));
		}
		detectorBoxes_.append(tmpDV);

		gl_->addWidget(tmpLabel,		x, 0, 1, 1, 0);
		gl_->addWidget(tmpDV,			x, 2, 1, 1, 0);
		if(configureOnly_)
			gl_->addWidget(tmpCheck,	x, 1, 1, 1, 0);
		gl_->addWidget(tmpButton,		x, 3, 1, 1, 0);
	}

	gl_->setRowStretch(viewSet_->count(), 10);
	gl_->setVerticalSpacing(1);
	gl_->setHorizontalSpacing(1);
	setLayout(gl_);


	/*
 int viewIndex = -1;
 qdebug() << "About to remove " << index << viewSet_->detectorAt(index) << viewSet_->count() << gl_->rowCount()-1;
 AMDetectorView *removeDV;
 for(int x = 0; x < gl_->rowCount()-1; x++){
  if(gl_->itemAtPosition(x, 2)){
   removeDV = qobject_cast<AMDetectorView*>(gl_->itemAtPosition(x, 2)->widget());
   if(removeDV->detector() == viewSet_->detectorAt(index)){
    viewIndex = x;
    qdebug() << "Found the detector at " << viewIndex;
   }
  }
 }
 removeDV = qobject_cast<AMDetectorView*>(gl_->itemAtPosition(viewIndex, 2)->widget());
 if(removeDV){
  gl_->removeWidget(removeDV);
  disconnect(removeDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
 }
 QLabel *removeLabel = qobject_cast<QLabel*>(gl_->itemAtPosition(viewIndex, 0)->widget());
 if(removeLabel)
  gl_->removeWidget(removeLabel);
 QPushButton *removeButton = qobject_cast<QPushButton*>(gl_->itemAtPosition(viewIndex, 3)->widget());
 if(removeButton)
  gl_->removeWidget(removeButton);
 QCheckBox *removeCheck = qobject_cast<QCheckBox*>(gl_->itemAtPosition(viewIndex, 1)->widget());
 if(removeCheck)
  gl_->removeWidget(removeCheck);
 */
}

void AMDetectorSetViewInternal::onDetectorSetSettingsChanged(){

}

void AMDetectorSetViewInternal::onDetectorSetConfigurationRequested(){
	emit configValuesChanged();
}
