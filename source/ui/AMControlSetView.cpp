/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMControlSetView.h"

/// Sets the title of the group box based on the name() function of the AMControlSet.
/// Loops through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Adds the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
AMControlSetView::AMControlSetView(AMControlSet *viewSet, bool configureOnly, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	setTitle(viewSet->name());
	QVBoxLayout *vl = new QVBoxLayout();
	AMControlEditor *tmpCE;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->at(x);
		tmpCE = new AMControlEditor(tmpCtrl, 0, false, configureOnly_);
		vl->addWidget(tmpCE);
		controlBoxes_.append(tmpCE);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(vl);
	setLayout(hl_);
	setFixedSize(300, 200);
}



/// Sets the title of the group box based on the name() function of the AMControlSet.
/// Loops through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Adds the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
AMOldControlSetView::AMOldControlSetView(AMControlSet *viewSet, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	boxTrigger_ = false;
	QFormLayout *fl = new QFormLayout();
	QWidget *tmpWidget;
	QDoubleSpinBox *tmpDSB;
	QComboBox *tmpCB;
	AMControl *tmpCtrl;
	QString tmpName;
	QVariant tmpVal;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->at(x);
		tmpVal.clear();
		tmpName = tmpCtrl->name();
		if(tmpCtrl->isEnum()){
			tmpCB = new QComboBox(this);
			tmpCB->addItems(tmpCtrl->enumNames());
			tmpCB->setCurrentIndex((int)tmpCtrl->value());
			tmpVal = tmpCtrl->enumNames().at((int)tmpCtrl->value());
			connect(tmpCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(onBoxUpdate(QString)));
			tmpWidget = tmpCB;
		}
		else{
			tmpDSB = new QDoubleSpinBox(this);
			tmpDSB->setValue(tmpCtrl->value());
			tmpDSB->setMaximum(tmpCtrl->maximumValue());
			tmpDSB->setMinimum(tmpCtrl->minimumValue());
			tmpVal = tmpCtrl->value();
			connect(tmpDSB, SIGNAL(valueChanged(QString)), this, SLOT(onBoxUpdate(QString)));
			tmpWidget = tmpDSB;
		}
		configValues_.insert(tmpName, tmpVal);
		QString adjName = tmpCtrl->objectName();
		adjName.replace(QRegExp("([A-Z])"), " \\1");
		QChar fCap = adjName[0].toUpper();
		adjName.replace(0, 1, fCap);
		fl->addRow(adjName, tmpWidget);
		controlBoxes_.append(tmpWidget);
		dirty_.append(false);
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(fl);
	setLayout(hl_);
	setFixedSize(300, 200);
}

void AMOldControlSetView::onBoxUpdate(const QString &value){
	Q_UNUSED(value);
	AMControl *tmpCtrl;
	QString tmpName;
	bool actualChange = false;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->at(x);
		tmpName = tmpCtrl->name();
		if(tmpCtrl->isEnum()){
			if( ((QComboBox*)(controlBoxes_.at(x)))->currentText() != configValues_[tmpName].toString() ){
				configValues_[tmpName] = ((QComboBox*)(controlBoxes_.at(x)))->currentText();
				if(dirty_.at(x)){
					dirty_[x] = false;
				}
				else
					actualChange = true;
			}
		}
		else{
			if( ((QDoubleSpinBox*)(controlBoxes_.at(x)))->value() != configValues_[tmpName].toDouble() ){
				configValues_[tmpName] = ((QDoubleSpinBox*)(controlBoxes_.at(x)))->value();
				if(dirty_.at(x)){
					dirty_[x] = false;
				}
				else
					actualChange = true;
			}
		}
	}
	if(actualChange){
		boxTrigger_ = true;
		emit configValuesChanged();
	}
}

void AMOldControlSetView::setConfigValues(QMap<QString, QVariant> configValues){
	QMap<QString, QVariant>::const_iterator i = configValues.constBegin();
	while(i != configValues.constEnd()){
		if( viewSet_->controlNamed(i.key()) && configValues_.value(i.key()) != i.value() ){
			dirty_[viewSet_->indexOfKey(i.key())] = true;
			if( viewSet_->controlNamed(i.key())->isEnum() && viewSet_->controlNamed(i.key())->enumNames().contains(i.value().toString()) )
				((QComboBox*)(boxByName(i.key())))->setCurrentIndex(viewSet_->controlNamed(i.key())->enumNames().indexOf(i.value().toString()));
			else if( !viewSet_->controlNamed(i.key())->isEnum() )
				((QDoubleSpinBox*)(boxByName(i.key())))->setValue(i.value().toDouble());
		}
		++i;
	}
	emit configValuesChanged();
}




