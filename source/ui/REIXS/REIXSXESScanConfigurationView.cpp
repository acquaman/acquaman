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


#include "REIXSXESScanConfigurationView.h"
#include <QTimer>
#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>

#include "ui/dataman/AMSamplePre2013Selector.h"

 REIXSXESScanConfigurationView::~REIXSXESScanConfigurationView(){}
REIXSXESScanConfigurationView::REIXSXESScanConfigurationView(REIXSXESScanConfiguration* config, QWidget *parent) :
		AMScanConfigurationView(parent)
{
	if(config) {
		configuration_ = config;
		ownsConfiguration_ = false;
	}
	else {
		configuration_ = new REIXSXESScanConfiguration(this);
		ownsConfiguration_ = true;
	}

	QGroupBox* detectorOptions = new QGroupBox("Initial Setup");
	QGroupBox* stopScanOptions = new QGroupBox("Stop scan when...");
	QGroupBox* nameOptions = new QGroupBox("Scan meta-data");

	//gratingSelector_ = new QComboBox();
	//centerEVBox_ = new QDoubleSpinBox();
	//defocusDistanceMmBox_ = new QDoubleSpinBox();
	//detectorTiltBox_ = new QDoubleSpinBox();

	energyBox_ = new QDoubleSpinBox();
	energyBox_->setDecimals(2);
	energyBox_->setEnabled(configuration_->applyEnergy());
	energyBox_->setRange(92,2000);
	energyBox_->setValue(configuration_->energy());
	applyEnergyBox_ = new QCheckBox("Beamline Energy");
	applyEnergyBox_->setChecked(configuration_->applyEnergy());

	slitWidthBox_= new QDoubleSpinBox();
	slitWidthBox_->setRange(5,500);
	slitWidthBox_->setValue(configuration_->slitWidth());
	slitWidthBox_->setSingleStep(5);
	slitWidthBox_->setDecimals(0);
	slitWidthBox_->setEnabled(configuration_->applySlitWidth());
	applySlitWidthBox_ = new QCheckBox("Slit Width");
	applySlitWidthBox_->setChecked(configuration_->applySlitWidth());


	polarizationBox_ = new QComboBox();
	polarizationBox_->addItem("Circular Left");
	polarizationBox_->addItem("Circular Right");
	polarizationBox_->addItem("Linear Horizontal");
	polarizationBox_->addItem("Linear Vertical +");
	polarizationBox_->addItem("Linear Vertical -");
	polarizationBox_->addItem("Linear Inclined");
	polarizationBox_->setCurrentIndex(configuration_->polarization());
	polarizationBox_->setEnabled(configuration_->applyPolarization());
	applyPolarizationBox_ = new QCheckBox("Polarization");
	applyPolarizationBox_->setChecked(configuration_->applyPolarization());

	polarizationAngleBox_ = new QDoubleSpinBox();
	polarizationAngleBox_->setDecimals(0);
	polarizationAngleBox_->setRange(-180,180);
	polarizationAngleBox_->setEnabled((configuration_->applyPolarization() && (configuration_->polarization() == 5)));
	polarizationAngleBox_->setValue(configuration_->polarizationAngle());
	polarizationAngleBox_->setSingleStep(10);

	polarizationAngleLabel_ = new QLabel();
	polarizationAngleLabel_->setText("      Angle");


	// removed:
	// horizontalDetectorButton_ = new QRadioButton("Wide Window");
	// verticalDetectorButton_ = new QRadioButton("High Resolution");

	//startFromCurrentPositionOption_ = new QCheckBox("current position");
	doNotClearExistingCountsOption_ = new QCheckBox("Do not clear previous detector data");

	maximumTotalCounts_ = new QDoubleSpinBox();
	maximumTimeEdit_ = new QTimeEdit();

//	calibrationSelector_ = new QComboBox();

	numberEdit_ = new QSpinBox();
	numberEdit_->setRange(0, 10000);
	nameEdit_ = new QLineEdit();
	nameEdit_->setText(configuration_->userScanName());
	sampleSelector_ = new AMSamplePre2013Selector(AMDatabase::database("user"));
	autoNamingCheckBox_ = new QCheckBox("from last sample move");

	/////////////////////

//	defocusDistanceMmBox_->setRange(-1000, 1000);	/// \todo these are meaningless for now
//	defocusDistanceMmBox_->setDecimals(2);

//	centerEVBox_->setDecimals(2);
//	centerEVBox_->setRange(10, 1400);
//	detectorTiltBox_->setDecimals(2);
//	detectorTiltBox_->setRange(-6, 6);	/// \todo these are meaningless for now
//	detectorTiltBox_->setSingleStep(0.1);

	maximumTotalCounts_->setDecimals(0);
	maximumTotalCounts_->setRange(10, 1000000000);
	maximumTotalCounts_->setValue(configuration_->maximumTotalCounts());
	maximumTimeEdit_->setDisplayFormat("hh:mm:ss");

	//////////////////////

	QVBoxLayout* vl = new QVBoxLayout();
	QGridLayout* gl = new QGridLayout();

//	fl->addRow("Grating", gratingSelector_);
//	fl->addRow("Center on (eV)", centerEVBox_);
//	fl->addRow("Defocus by (mm)", defocusDistanceMmBox_);
//	fl->addRow("Detector tilt offset (deg)", detectorTiltBox_);


	// removed:
//	QVBoxLayout* vl1 = new QVBoxLayout();
//	vl1->addWidget(horizontalDetectorButton_);
//	vl1->addWidget(verticalDetectorButton_);
//	fl->addRow("Detector orientation", vl1);

	gl->addWidget(applyEnergyBox_,0,0);
	gl->addWidget(energyBox_,0,1);
	gl->addWidget(applySlitWidthBox_,1,0);
	gl->addWidget(slitWidthBox_,1,1);
	gl->addWidget(applyPolarizationBox_,2,0);
	gl->addWidget(polarizationBox_,2,1);
	gl->addWidget(polarizationAngleLabel_,3,0);
	gl->addWidget(polarizationAngleBox_,3,1);

//	fl->addWidget("Start from", startFromCurrentPositionOption_);
	gl->addWidget(doNotClearExistingCountsOption_,4,0,1,2,0);
//	fl->addRow("Calibration", calibrationSelector_);

	detectorOptions->setLayout(gl);

	QFormLayout* fl2 = new QFormLayout();
	fl2->addRow("Total Counts Reaches", maximumTotalCounts_);
	fl2->addRow("After this much time", maximumTimeEdit_);

	stopScanOptions->setLayout(fl2);


	QFormLayout* fl3 = new QFormLayout();
	fl3->addRow("Scan name", nameEdit_);
	fl3->addRow("Number", numberEdit_);
	fl3->addRow("Sample", sampleSelector_);
	fl3->addRow("Set automatically", autoNamingCheckBox_);

	nameOptions->setLayout(fl3);

	vl->addWidget(detectorOptions);
	vl->addWidget(stopScanOptions);
	vl->addWidget(nameOptions);

	setLayout(vl);


	currentCalibrationId_ = -1;
//	onLoadCalibrations();

	//////////////////////
//	centerEVBox_->setValue(configuration_->centerEV());
//	defocusDistanceMmBox_->setValue(configuration_->defocusDistanceMm());
//	detectorTiltBox_->setValue(configuration_->detectorTiltOffset());
//	gratingSelector_->setCurrentIndex(configuration_->gratingNumber());

	maximumTotalCounts_->setValue(configuration_->maximumTotalCounts());
	maximumTimeEdit_->setTime(QTime().addSecs(int(configuration_->maximumDurationSeconds())));
//	startFromCurrentPositionOption_->setChecked(configuration_->shouldStartFromCurrentPosition());
	doNotClearExistingCountsOption_->setChecked(configuration_->doNotClearExistingCounts());

	nameEdit_->setText(configuration_->userScanName());
	numberEdit_->setValue(configuration_->scanNumber());
	sampleSelector_->setCurrentSample(configuration_->sampleId());
	autoNamingCheckBox_->setChecked(configuration_->namedAutomatically());
	onAutoNamingCheckboxClicked(configuration_->namedAutomatically());
	/////////////////////////

//	connect(gratingSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectedGratingChanged(int)));
//	connect(centerEVBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setCenterEV(double)));
//	connect(defocusDistanceMmBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setDefocusDistanceMm(double)));
//	connect(detectorTiltBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setDetectorTiltOffset(double)));

//	connect(startFromCurrentPositionOption_, SIGNAL(toggled(bool)), configuration_, SLOT(setShouldStartFromCurrentPosition(bool)));

	connect(applySlitWidthBox_, SIGNAL(toggled(bool)),slitWidthBox_,SLOT(setEnabled(bool)));
	connect(applySlitWidthBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setApplySlitWidth(bool)));
	connect(slitWidthBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setSlitWidth(double)));



	connect(applyEnergyBox_, SIGNAL(toggled(bool)), energyBox_, SLOT(setEnabled(bool)));
	connect(applyEnergyBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setApplyEnergy(bool)));
	connect(energyBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setEnergy(double)));

	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setApplyPolarization(bool)));
	connect(polarizationBox_, SIGNAL(currentIndexChanged(int)), configuration_, SLOT(setPolarization(int)));
	connect(polarizationAngleBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setPolarizationAngle(double)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), polarizationBox_, SLOT(setEnabled(bool)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(polarizationBox_, SIGNAL(activated(int)), this, SLOT(reviewPolarizationAngleBoxEnabled()));



	connect(doNotClearExistingCountsOption_, SIGNAL(toggled(bool)), configuration_, SLOT(setDoNotClearExistingCounts(bool)));

	connect(maximumTotalCounts_, SIGNAL(valueChanged(double)), configuration_, SLOT(setMaximumTotalCounts(double)));
	connect(maximumTimeEdit_, SIGNAL(timeChanged(QTime)), this, SLOT(onMaximumTimeEditChanged(QTime)));

//	connect(calibrationSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCalibrationIndexChanged(int)));

	connect(autoNamingCheckBox_, SIGNAL(clicked(bool)), this, SLOT(onAutoNamingCheckboxClicked(bool)));

	connect(nameEdit_, SIGNAL(textEdited(QString)), configuration_, SLOT(setUserScanName(QString)));
	connect(numberEdit_, SIGNAL(valueChanged(int)), configuration_, SLOT(setScanNumber(int)));
	connect(sampleSelector_, SIGNAL(currentSampleChanged(int)), configuration_, SLOT(setSampleId(int)));
	connect(autoNamingCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setNamedAutomatically(bool)));

	///////////////////////
}


#include "dataman/database/AMDatabase.h"
#include "util/AMDateTimeUtils.h"
//void REIXSXESScanConfigurationView::onLoadCalibrations() {

//	calibrationSelector_->blockSignals(true);

//	calibrationSelector_->clear();

//	calibrationSelector_->addItem("Default", -1);

//	QSqlQuery q = AMDatabase::database("user")->query();

//	q.prepare(QString("SELECT id, dateTime FROM %1").arg(calibration_.dbTableName()));
//	q.exec();

//	while(q.next()) {
//		int id = q.value(0).toInt();
//		QDateTime dateTime = q.value(1).toDateTime();
//		calibrationSelector_->addItem(AMDateTimeUtils::prettyDateTime(dateTime), id);
//	}
//	q.finish();

//	// if we had a previously valid calibration, re-set it as current
//	int newIndexForOldId;
//	if(currentCalibrationId_ > 0 && (newIndexForOldId = calibrationSelector_->findData(currentCalibrationId_)) != -1) {
//		calibrationSelector_->setCurrentIndex(newIndexForOldId);
//	}
//	else
//		calibrationSelector_->setCurrentIndex(0);

//	calibrationSelector_->blockSignals(false);

//	onCalibrationIndexChanged(calibrationSelector_->currentIndex());
//}

//void REIXSXESScanConfigurationView::onCalibrationIndexChanged(int newIndex) {
//	if(newIndex < 0)
//		return;

//	currentCalibrationId_ = calibrationSelector_->itemData(newIndex).toInt();

//	configuration_->setSpectrometerCalibrationId(currentCalibrationId_);

//	if(currentCalibrationId_ > 0) {
//		calibration_.loadFromDb(AMDatabase::database("user"), currentCalibrationId_);
//	}
//	else {
//		calibration_ = REIXSXESCalibration2();
//	}


//	int gratingCount = calibration_.gratingCount();
//	// remove any extra gratings from the selector that aren't in this configuration
//	while(gratingSelector_->count() > gratingCount)
//		gratingSelector_->removeItem(gratingSelector_->count()-1);
//	// set corresponding names
//	for(int i=0; i<gratingSelector_->count(); i++) {
//		const REIXSXESGratingInfo& g = calibration_.gratingAt(i);
//		double min = g.evRangeMin();
//		double max = g.evRangeMax();
//		gratingSelector_->setItemText(i, QString("%1 (%2 - %3 eV)").arg(g.name()).arg(min).arg(max));
//	}
//	// add extra names
//	for(int i=gratingSelector_->count(); i<gratingCount; i++) {
//		const REIXSXESGratingInfo& g = calibration_.gratingAt(i);
//		double min = g.evRangeMin();
//		double max = g.evRangeMax();
//		gratingSelector_->addItem(QString("%1 (%2 - %3 eV)").arg(g.name()).arg(min).arg(max));
//	}

//	// Select the calibration's current grating, if that's legit.
//	gratingSelector_->setCurrentIndex(configuration_->gratingNumber());
//	// This will update the energy range, and change the configuration's gratingNumber if the current one doesn't exist in this calibration.
//	onSelectedGratingChanged(gratingSelector_->currentIndex());

//}

//void REIXSXESScanConfigurationView::onSelectedGratingChanged(int newGrating) {

//	if(newGrating < 0)
//		return;

//	const REIXSXESGratingInfo& g = calibration_.gratingAt(newGrating);
//	centerEVBox_->setRange(g.evRangeMin(), g.evRangeMax());

//	configuration_->setGratingNumber(newGrating);
//}

void REIXSXESScanConfigurationView::onMaximumTimeEditChanged(const QTime &time) {

	QTime baseTime(0,0);
	double totalSeconds = baseTime.secsTo(time);
	configuration_->setMaximumDurationSeconds(int(totalSeconds));
}

void REIXSXESScanConfigurationView::onAutoNamingCheckboxClicked(bool autoOn)
{
	nameEdit_->setEnabled(!autoOn);
	numberEdit_->setEnabled(!autoOn);
	sampleSelector_->setEnabled(!autoOn);
}

void REIXSXESScanConfigurationView::reviewPolarizationAngleBoxEnabled()
{
	polarizationAngleBox_->setEnabled(configuration_->polarization() == 5 && configuration_->applyPolarization());
}
