/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "REIXSXESScanConfigurationView.h"
#include <QTimer>
#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>

#include <QDebug>

REIXSXESScanConfigurationView::REIXSXESScanConfigurationView(QWidget *parent) :
		AMScanConfigurationView(parent)
{
	QGroupBox* detectorOptions = new QGroupBox("Detector Setup");
	// detectorOptions->setFlat(true);
	QGroupBox* stopScanOptions = new QGroupBox("Stop scan when...");
	gratingSelector_ = new QComboBox();
	centerEVBox_ = new QDoubleSpinBox();
	defocusDistanceMmBox_ = new QDoubleSpinBox();
	detectorTiltBox_ = new QDoubleSpinBox();

	horizontalDetectorButton_ = new QRadioButton("Wide Window");
	verticalDetectorButton_ = new QRadioButton("High Resolution");

	startFromCurrentPositionOption_ = new QCheckBox("current position");

	maximumTotalCounts_ = new QDoubleSpinBox();
	maximumTimeEdit_ = new QTimeEdit();

	calibrationSelector_ = new QComboBox();

	/////////////////////

	defocusDistanceMmBox_->setRange(-1000, 1000);	/// \todo these are meaningless for now
	defocusDistanceMmBox_->setDecimals(2);

	centerEVBox_->setDecimals(2);
	centerEVBox_->setRange(10, 1400);
	detectorTiltBox_->setDecimals(2);
	detectorTiltBox_->setRange(-6, 6);	/// \todo these are meaningless for now
	detectorTiltBox_->setSingleStep(0.1);

	maximumTotalCounts_->setDecimals(0);
	maximumTotalCounts_->setRange(10, 1000000000);
	maximumTimeEdit_->setDisplayFormat("hh:mm:ss");

	//////////////////////

	QVBoxLayout* vl = new QVBoxLayout();
	QFormLayout* fl = new QFormLayout();

	fl->addRow("Grating", gratingSelector_);
	fl->addRow("Center on (eV)", centerEVBox_);
	fl->addRow("Defocus by (mm)", defocusDistanceMmBox_);
	fl->addRow("Detector tilt offset (deg)", detectorTiltBox_);

	QVBoxLayout* vl1 = new QVBoxLayout();
	vl1->addWidget(horizontalDetectorButton_);
	vl1->addWidget(verticalDetectorButton_);

	fl->addRow("Detector orientation", vl1);
	fl->addRow("Start from", startFromCurrentPositionOption_);
	fl->addRow("Calibration", calibrationSelector_);

	detectorOptions->setLayout(fl);

	QFormLayout* fl2 = new QFormLayout();
	fl2->addRow("Total Counts Reaches", maximumTotalCounts_);
	fl2->addRow("After this much time", maximumTimeEdit_);

	stopScanOptions->setLayout(fl2);

	vl->addWidget(detectorOptions);
	vl->addWidget(stopScanOptions);

	setLayout(vl);


	//////////////////////
	centerEVBox_->setValue(configuration_.centerEV());
	defocusDistanceMmBox_->setValue(configuration_.defocusDistanceMm());
	detectorTiltBox_->setValue(configuration_.detectorTiltOffset());
	if(configuration_.detectorOrientation() == 0)
		horizontalDetectorButton_->setChecked(true);
	else
		verticalDetectorButton_->setChecked(true);

	maximumTotalCounts_->setValue(configuration_.maximumTotalCounts());
	maximumTimeEdit_->setTime(QTime().addSecs(configuration_.maximumDurationSeconds()));
	/////////////////////////

	connect(centerEVBox_, SIGNAL(valueChanged(double)), &configuration_, SLOT(setCenterEV(double)));
	connect(defocusDistanceMmBox_, SIGNAL(valueChanged(double)), &configuration_, SLOT(setDefocusDistanceMm(double)));
	connect(detectorTiltBox_, SIGNAL(valueChanged(double)), &configuration_, SLOT(setDetectorTiltOffset(double)));

	connect(verticalDetectorButton_, SIGNAL(toggled(bool)), &configuration_, SLOT(setDetectorOrientation(bool)));
	connect(startFromCurrentPositionOption_, SIGNAL(toggled(bool)), &configuration_, SLOT(setShouldStartFromCurrentPosition(bool)));

	connect(maximumTotalCounts_, SIGNAL(valueChanged(double)), &configuration_, SLOT(setMaximumTotalCounts(double)));

	connect(maximumTimeEdit_, SIGNAL(timeChanged(QTime)), this, SLOT(onMaximumTimeEditChanged(QTime)));

	connect(calibrationSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCalibrationIndexChanged(int)));

	///////////////////////


	currentCalibrationId_ = -1;

	QTimer::singleShot(0, this, SLOT(onLoadCalibrations()));
}


#include "dataman/database/AMDatabase.h"
#include "util/AMDateTimeUtils.h"
void REIXSXESScanConfigurationView::onLoadCalibrations() {

	calibrationSelector_->blockSignals(true);

	calibrationSelector_->clear();

	calibrationSelector_->addItem("Default", -1);

	QSqlQuery q = AMDatabase::userdb()->query();

	q.prepare(QString("SELECT id, dateTime FROM %1").arg(calibration_.dbTableName()));
	q.exec();

	while(q.next()) {
		int id = q.value(0).toInt();
		QDateTime dateTime = q.value(1).toDateTime();
		calibrationSelector_->addItem(AMDateTimeUtils::prettyDateTime(dateTime), id);
	}

	// if we had a previously valid calibration, re-set it as current
	int newIndexForOldId;
	if(currentCalibrationId_ > 0 && (newIndexForOldId = calibrationSelector_->findData(currentCalibrationId_)) != -1) {
		calibrationSelector_->setCurrentIndex(newIndexForOldId);
	}
	else
		calibrationSelector_->setCurrentIndex(0);

	calibrationSelector_->blockSignals(false);

	onCalibrationIndexChanged(calibrationSelector_->currentIndex());
}

void REIXSXESScanConfigurationView::onCalibrationIndexChanged(int newIndex) {
	if(newIndex < 0)
		return;

	currentCalibrationId_ = calibrationSelector_->itemData(newIndex).toInt();

	configuration_.setSpectrometerCalibrationId(currentCalibrationId_);

	if(currentCalibrationId_ > 0) {
		calibration_.loadFromDb(AMDatabase::userdb(), currentCalibrationId_);
	}
	else {
		calibration_ = REIXSXESCalibration();
	}


	QStringList gratingNames = calibration_.gratingNames();
	qDebug() << gratingNames;
	// remove any extra gratings from the selector that aren't in this configuration
	while(gratingSelector_->count() > gratingNames.count())
		gratingSelector_->removeItem(gratingSelector_->count()-1);
	// set corresponding names
	for(int i=0; i<gratingSelector_->count(); i++) {
		QPair<double, double> evRange = calibration_.evRangeForGrating(i);
		gratingSelector_->setItemText(i, QString("%1 (%2 - %3 eV)").arg(gratingNames.at(i)).arg(evRange.first).arg(evRange.second));
	}
	// add extra names
	for(int i=gratingSelector_->count(); i<gratingNames.count(); i++) {
		QPair<double, double> evRange = calibration_.evRangeForGrating(i);
		gratingSelector_->addItem(QString("%1 (%2 - %3 eV)").arg(gratingNames.at(i)).arg(evRange.first).arg(evRange.second));
	}

	onSelectedGratingChanged(gratingSelector_->currentIndex());

}

void REIXSXESScanConfigurationView::onSelectedGratingChanged(int newGrating) {

	if(newGrating < 0)
		return;

	QPair<double, double> evRange = calibration_.evRangeForGrating(newGrating);
	centerEVBox_->setRange(evRange.first, evRange.second);

	configuration_.setGratingNumber(newGrating);
}

void REIXSXESScanConfigurationView::onMaximumTimeEditChanged(const QTime &time) {

	QTime baseTime(0,0);
	double totalSeconds = baseTime.secsTo(time);
	configuration_.setMaximumDurationSeconds(totalSeconds);
}
