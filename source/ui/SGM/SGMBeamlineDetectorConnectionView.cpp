#include "SGMBeamlineDetectorConnectionView.h"

#include <QFormLayout>
#include <QCheckBox>
#include <QLabel>

#include "beamline/SGM/SGMBeamline.h"

SGMBeamlineDetectorConnectionView::SGMBeamlineDetectorConnectionView(QWidget *parent) :
	QWidget(parent)
{
	fl_ = new QFormLayout();
	QList<AMDetector*> possibleDetectors = SGMBeamline::sgm()->possibleDetectorsForSet(SGMBeamline::sgm()->allDetectors());
	for(int x = 0; x < possibleDetectors.count(); x++){
		QCheckBox *tempCheckBox = new QCheckBox();
		tempCheckBox->setEnabled(false);
		if(possibleDetectors.at(x)->isConnected())
			tempCheckBox->setChecked(true);
		fl_->addRow(possibleDetectors.at(x)->detectorName(), tempCheckBox);
	}
	setLayout(fl_);

	connect(SGMBeamline::sgm(), SIGNAL(detectorAvailabilityChanged(AMDetector*,bool)), this, SLOT(onDetectorAvailabilityChanged(AMDetector*,bool)));
}

void SGMBeamlineDetectorConnectionView::onDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable){
	for(int x = 0; x < fl_->rowCount(); x++){
		QLabel *tempLabel;
		tempLabel = qobject_cast<QLabel*>(fl_->itemAt(x, QFormLayout::LabelRole)->widget());
		if(tempLabel && tempLabel->text() == detector->detectorName()){
			QCheckBox *tempCheckBox = qobject_cast<QCheckBox*>(fl_->itemAt(x, QFormLayout::FieldRole)->widget());
			if(tempCheckBox)
				tempCheckBox->setChecked(isAvailable);
			break;
		}
	}
}
