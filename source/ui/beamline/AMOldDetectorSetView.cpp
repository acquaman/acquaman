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


#include "AMOldDetectorSetView.h"

AMOldDetectorInfoSetView::AMOldDetectorInfoSetView(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMOldDetectorInfoSetViewInternal(infoSet, parent);

	setTitle(infoSet.name());
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(internalView_);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(scrollArea_);
	mainVL->setContentsMargins(0,0,0,0);
	setLayout(mainVL);
}

AMOldDetectorInfoSetViewInternal::AMOldDetectorInfoSetViewInternal(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *mainVL = new QVBoxLayout();

	QLabel *tempLabel;
	QCheckBox *tempCheckBox;
	QHBoxLayout *tempHL;
	for(int x = 0, size = infoSet.count(); x < size; x++){
		tempLabel = new QLabel(infoSet.detectorInfoAt(x)->description());
		tempCheckBox = new QCheckBox();
		tempCheckBox->setChecked(infoSet.isActiveAt(x));
		tempHL = new QHBoxLayout();
		tempHL->addWidget(tempLabel);
		tempHL->addWidget(tempCheckBox);
		mainVL->addLayout(tempHL);
	}

	setLayout(mainVL);
}

AMOldDetectorSetView::AMOldDetectorSetView(AMOldDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMOldDetectorSetViewInternal(viewSet, configureOnly);
	initializeWidget(viewSet);
}

AMOldDetectorSetView::AMOldDetectorSetView(AMOldDetectorSet *viewSet, const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMOldDetectorSetViewInternal(viewSet, infoSet);
	initializeWidget(viewSet);
}

int AMOldDetectorSetView::count() const{
	return internalView_->count();
}

AMOldDetectorSet* AMOldDetectorSetView::detectorSet(){
	return internalView_->detectorSet();
}

AMOldDetectorView* AMOldDetectorSetView::boxByName(const QString &name){
	return internalView_->boxByName(name);
}

const AMOldDetectorView* AMOldDetectorSetView::boxAt(int row) const{
	return internalView_->boxAt(row);
}

AMOldDetectorView* AMOldDetectorSetView::detailByName(const QString &name){
	return internalView_->detailByName(name);
}

const AMOldDetectorView* AMOldDetectorSetView::detailAt(int row) const{
	return internalView_->detailAt(row);
}

bool AMOldDetectorSetView::checkedAt(int row) const{
	return internalView_->checkedAt(row);
}

AMOldDetectorInfoSet AMOldDetectorSetView::currentValues(){
	return internalView_->currentValues();
}

AMOldDetectorInfoSet AMOldDetectorSetView::configValues(){
	return internalView_->configValues();
}

QDebug operator<<(QDebug d, const AMOldDetectorSetView& dsv){
	d << dsv.internalView_;
	return d;
}

void AMOldDetectorSetView::initializeWidget(AMOldDetectorSet *viewSet){
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

AMOldDetectorSetViewInternal::AMOldDetectorSetViewInternal(AMOldDetectorSet *viewSet, bool configureOnly, QWidget *parent) :
	QWidget(parent)
	//QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	gl_ = new QGridLayout();
	AMOldDetectorView *tmpDV;
	AMOldDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMOldDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMOldDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMOldDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMOldDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
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

AMOldDetectorSetViewInternal::AMOldDetectorSetViewInternal(AMOldDetectorSet *viewSet, const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QWidget(parent)
//	QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = true;
	gl_ = new QGridLayout();
	AMOldDetectorView *tmpDV;
	AMOldDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMOldDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMOldDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMOldDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMOldDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
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

int AMOldDetectorSetViewInternal::count() const{
	return viewSet_->count();
}

AMOldDetectorSet* AMOldDetectorSetViewInternal::detectorSet(){
	return viewSet_;
}

AMOldDetectorView* AMOldDetectorSetViewInternal::boxByName(const QString &name){
	return detectorBoxes_.at(viewSet_->indexOfKey(name));
}

const AMOldDetectorView *AMOldDetectorSetViewInternal::boxAt(int row) const{
	return detectorBoxes_.at(row);
}

AMOldDetectorView* AMOldDetectorSetViewInternal::detailByName(const QString &name){
	return detectorDetails_.at(viewSet_->indexOf(name));
}

const AMOldDetectorView *AMOldDetectorSetViewInternal::detailAt(int row) const{
	return detectorDetails_.at(row);
}

bool AMOldDetectorSetViewInternal::checkedAt(int row) const{
	if(!configureOnly_)
		return false;
	if(checkBoxes_.at(row))
		return checkBoxes_.at(row)->isChecked();
	return false;
}

AMOldDetectorInfoSet AMOldDetectorSetViewInternal::currentValues(){
	return viewSet_->toInfoSet();
}

AMOldDetectorInfoSet AMOldDetectorSetViewInternal::configValues(){
	AMOldDetectorInfoSet rv;

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

QDebug operator<<(QDebug d, const AMOldDetectorSetViewInternal& dsv){
	for(int x = 0; x < dsv.count(); x++){
		if(dsv.boxAt(x) && dsv.boxAt(x)->configurationSettings())
			d << *(dsv.boxAt(x)->configurationSettings());
		if(dsv.detailAt(x) && dsv.detailAt(x)->configurationSettings())
			d << *(dsv.detailAt(x)->configurationSettings());
	}
	return d;
}

void AMOldDetectorSetViewInternal::setDisabled(bool disabled){
	for(int x = checkBoxes_.count()-1; x >= 0; x--){
		checkBoxes_[x]->setDisabled(disabled);
		if(detectorDetails_[x])
			gl_->itemAtPosition(x, 3)->widget()->setDisabled(disabled);
	}
}

void AMOldDetectorSetViewInternal::onDetectorAddedToSet(int index){
	AMOldDetectorView *tmpDV;
	AMOldDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMOldDetectorView *tmpDetails;
	tmpD = viewSet_->detectorAt(index);
	tmpDV = AMOldDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
	connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
	tmpLabel = new QLabel(tmpD->description());
	tmpButton = new QPushButton("Details");
	if(AMOldDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
		tmpButton->setEnabled(false);
		detectorDetails_.insert(index, 0);
	}
	else{
		tmpDetails = AMOldDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
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
void AMOldDetectorSetViewInternal::onDetectorRemovedFromSet(int index){
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
	AMOldDetectorView *tmpDV;
	AMOldDetector *tmpD;
	QLabel *tmpLabel;
	QPushButton *tmpButton;
	QCheckBox *tmpCheck;
	AMOldDetectorView *tmpDetails;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpD = viewSet_->detectorAt(x);
		tmpDV = AMOldDetectorViewSupport::createBriefDetectorView(tmpD, configureOnly_);
		connect(tmpDV, SIGNAL(settingsConfigureRequested()), this, SLOT(onDetectorSetConfigurationRequested()));
		tmpLabel = new QLabel(tmpD->description());
		tmpButton = new QPushButton("Details");
		if(AMOldDetectorViewSupport::supportedDetailedViews(tmpD).count() == 0){
			tmpButton->setEnabled(false);
			detectorDetails_.append(0);
		}
		else{
			tmpDetails = AMOldDetectorViewSupport::createDetailedDetectorView(tmpD, configureOnly_);
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

void AMOldDetectorSetViewInternal::onDetectorSetSettingsChanged(){

}

void AMOldDetectorSetViewInternal::onDetectorSetConfigurationRequested(){
	emit configValuesChanged();
}
