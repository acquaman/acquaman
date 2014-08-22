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


#include "SGMBeamlineDetectorConnectionView.h"

//#include <QFormLayout>
//#include <QCheckBox>
//#include <QLabel>

//#include "beamline/SGM/SGMBeamline.h"
//#include "beamline/AMDetector.h"

//SGMBeamlineDetectorConnectionView::SGMBeamlineDetectorConnectionView(QWidget *parent) :
//	QWidget(parent)
//{
//	fl_ = new QFormLayout();
//	QList<AMDetector*> possibleDetectors;// = SGMBeamline::sgm()->possibleDetectorsForSet(SGMBeamline::sgm()->allDetectors());
//	for(int x = 0; x < possibleDetectors.count(); x++){
//		QCheckBox *tempCheckBox = new QCheckBox();
//		tempCheckBox->setEnabled(false);
//		if(possibleDetectors.at(x)->isConnected())
//			tempCheckBox->setChecked(true);
//		fl_->addRow(possibleDetectors.at(x)->detectorName(), tempCheckBox);
//	}
//	setLayout(fl_);

//	connect(SGMBeamline::sgm(), SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)), this, SLOT(onDetectorAvailabilityChanged(AMOldDetector*,bool)));
//}

//void SGMBeamlineDetectorConnectionView::onDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable){
//	for(int x = 0; x < fl_->rowCount(); x++){
//		QLabel *tempLabel;
//		tempLabel = qobject_cast<QLabel*>(fl_->itemAt(x, QFormLayout::LabelRole)->widget());
//		if(tempLabel && tempLabel->text() == detector->detectorName()){
//			QCheckBox *tempCheckBox = qobject_cast<QCheckBox*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
//			if(tempCheckBox)
//				tempCheckBox->setChecked(isAvailable);
//			break;
//		}
//	}
//}
