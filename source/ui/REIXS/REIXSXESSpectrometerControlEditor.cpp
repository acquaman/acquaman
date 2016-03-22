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

#include "REIXSXESSpectrometerControlEditor.h"

#include <QStringBuilder>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>

#include "util/AMErrorMonitor.h"
#include "util/AMFontSizes.h"

REIXSXESSpectrometerControlEditor::REIXSXESSpectrometerControlEditor(REIXSSpectrometer* spectrometer, QWidget *parent) :
	QGroupBox(parent)
{
	if (objectName().isEmpty())
		setObjectName("REIXSXESSpectrometerControlEditor");

	spectrometer_ = spectrometer;

	resize(285, 350);
	setupUi();
	setupConnections();

	populateGratingComboBox();
	updateMaskPosition();
	updateCurrentEnergyStatus();
	if(spectrometer_->eV() > 0)
		energyBox_->setValue(spectrometer_->eV());
}

REIXSXESSpectrometerControlEditor::~REIXSXESSpectrometerControlEditor()
{
}

/*
void REIXSXESSpectrometerControlEditor::onMoveButtonClicked()
{
	spectrometer_->specifyGrating(gratingSelectorBox_->currentIndex());
	spectrometer_->specifyDetectorTiltOffset(tiltOffsetBox_->value());
	spectrometer_->specifyFocusOffset(defocusOffsetBox_->value());

	int failureExplanation = spectrometer_->move(energyBox_->value());
	if(failureExplanation != AMControl::NoFailure)
		onSpectrometerMoveFailed(failureExplanation);
}  DM UPDATE CODE TO VERIFY GRATING CHANGE FOLLOWS:
*/

void REIXSXESSpectrometerControlEditor::onMoveButtonClicked()
{
	int confirm = QMessageBox::Yes;
	if (spectrometer_->grating() != gratingSelectorBox_->currentIndex())
	{
		QMessageBox confirmBox;
		confirmBox.setText("A new grating has been selected.");
		confirmBox.setInformativeText("Do you want to change gratings?");
		confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Abort);
		confirm = confirmBox.exec();
	}

	if (confirm == QMessageBox::Yes)
	{
		spectrometer_->specifyGrating(gratingSelectorBox_->currentIndex());
		spectrometer_->specifyDetectorTiltOffset(tiltOffsetBox_->value());
		spectrometer_->specifyFocusOffset(defocusOffsetBox_->value());

		int failureExplanation = spectrometer_->move(energyBox_->value());
		if(failureExplanation != AMControl::NoFailure)
			onSpectrometerMoveFailed(failureExplanation);
	}
	else gratingSelectorBox_->setCurrentIndex(spectrometer_->grating());
}

void REIXSXESSpectrometerControlEditor::onGratingComboBoxActivated(int grating)
{
	Q_UNUSED(grating)
	energyBox_->setMaximum(spectrometer_->spectrometerCalibration()->gratingAt(grating).evRangeMax());
	energyBox_->setMinimum(spectrometer_->spectrometerCalibration()->gratingAt(grating).evRangeMin());
	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::populateGratingComboBox()
{
	gratingSelectorBox_->blockSignals(true);

	gratingSelectorBox_->clear();

	for(int i=0, cc=spectrometer_->spectrometerCalibration()->gratingCount(); i<cc; i++)
		gratingSelectorBox_->addItem(spectrometer_->spectrometerCalibration()->gratingAt(i).name());


	if(spectrometer_->grating() != -1) {
		gratingSelectorBox_->setCurrentIndex(spectrometer_->grating());
		energyBox_->setMaximum(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).evRangeMax());
		energyBox_->setMinimum(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).evRangeMin());

	}
	gratingSelectorBox_->blockSignals(false);

	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus(double eV)
{
	if(eV < 0) {
		energyFeedbackLabel_->setText("out of position");
	}

	else if(fabs(eV - energyBox_->value()) < 0.001) {
		energyFeedbackLabel_->setText(QString());	// we're exactly what the user asked for, all is well, don't need to show anything.
	}
	else {
		energyFeedbackLabel_->setText(QString("at %1").arg(eV));
	}
}

void REIXSXESSpectrometerControlEditor::updateMaskPosition()
{
	double arg1 = spectrometer_->gratingMask()->value();


	if (fabs(arg1 - 8.5) < 0.01) {
		maskFeedbackLabel_->setText(QString("Currently: Pinhole"));
		maskComboBox_->blockSignals(true);
		maskComboBox_->setCurrentIndex(0);
		maskComboBox_->blockSignals(false);
	}
	else if (fabs(arg1 - 12.9) < 0.01) {
		maskFeedbackLabel_->setText(QString("Currently: Mask"));
		maskComboBox_->blockSignals(true);
		maskComboBox_->setCurrentIndex(1);
		maskComboBox_->blockSignals(false);

	}
	else if (arg1 < 2) {
		maskFeedbackLabel_->setText(QString("Currently: Out"));
		maskComboBox_->blockSignals(true);
		maskComboBox_->setCurrentIndex(2);
		maskComboBox_->blockSignals(false);

	}
	else
		maskFeedbackLabel_->setText(QString("Currently: %1").arg(arg1));
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus() {
	updateCurrentEnergyStatus(spectrometer_->value());
}

void REIXSXESSpectrometerControlEditor::updateCurrentGratingStatus()
{
	QString gratingFeedbackText;
	if(spectrometer_->grating() == -1) {
		gratingFeedbackText = "Currently: unknown";
	}
	else if(spectrometer_->gratingInPosition() == false) {
		gratingFeedbackText = QString("Currently: %1 (out of position)").arg(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name());
	}
	else if(spectrometer_->grating() != gratingSelectorBox_->currentIndex()) {
		gratingFeedbackText = QString("Currently: %1").arg(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name());
	}
	else {
		// our drop-down selection matches the current grating, and it's in position. All is well -- don't need to tell the user anything.
	}

	gratingFeedbackLabel_->setText(gratingFeedbackText);
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveSucceeded()
{
	/*
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, QString("Spectrometer move to %1 finished.").arg(spectrometer_->value())));
	*/
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveFailed(int reason)
{
	Q_UNUSED(reason)
	/*
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer Move Failed"));
	*/
}

void REIXSXESSpectrometerControlEditor::onStopButtonClicked()
{
	spectrometer_->stop();
}


void REIXSXESSpectrometerControlEditor::on_maskComboBox_currentIndexChanged(const QString &arg1)
{
	if (arg1 == "Pinhole")
		spectrometer_->gratingMask()->move(8.5);
	else if (arg1 == "Mask")
		spectrometer_->gratingMask()->move(13);
	else if (arg1 == "Out")
		spectrometer_->gratingMask()->move(1.0);
}

void REIXSXESSpectrometerControlEditor::updateEnergyRange()
{
	float upperE = 0, lowerE =0;

	int grating = gratingSelectorBox_->currentIndex();

	double grooveDensity = spectrometer_->spectrometerCalibration()->gratingAt(grating).grooveDensity();
	double gratingRadius = spectrometer_->spectrometerCalibration()->gratingAt(grating).radius();
	double sinAlpha = sin(spectrometer_->spectrometerCalibration()->d2r(spectrometer_->spectrometerCalibration()->gratingAt(grating).alpha()));

	// Variables. From here on, we work in radians, instead of the degree convention used by REIXSXESCalibration.
	// beta is also the nominal incidence angle onto the detector. Get beta from spectrometer position:
	double beta = asin(sinAlpha - (grooveDensity * 0.0012398417/energyBox_->value()));

	double sinBeta = sin(beta);
	double cosBeta = cos(beta);

	double rPrime = gratingRadius * cosBeta;


	double gamma = spectrometer_->spectrometerCalibration()->d2r(tiltOffsetBox_->value());

	double singp = cosBeta*cos(gamma) + sinBeta*sin(gamma);
	double cosgp = sinBeta*cos(gamma) - cosBeta*sin(gamma);
	// Calculate bottom half of the axis. (low energies). Sign is -1

	double dx = 20;
	// sindb: sin("delta Beta"): the angle difference from the nominal beta.
	double sindb = -1 *( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*-1) );	//you can derive this from sinA/a=sinB/b and c^2=a^2+b^2-2ab*cosC
	//bp ("beta-prime") is the diffraction angle at detector point 'i', sinbp = sin( beta + db )
	//																		 = sinb*cos(db) + cosb*sindb
	//																		 = sinb*sqrt(1-sin^2(db)) + cosb*sindb
	double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
	//solving the grating equation for eV:
	lowerE = 0.0012398417*grooveDensity / (sinAlpha - sinbp);	// NOTE: we're adding in the user-specified energy offset here.

	sindb = ( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp) );
	sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
	upperE = 0.0012398417*grooveDensity / (sinAlpha - sinbp);	// NOTE: we're adding in the user-specified energy offset here.

	energyRangeLabel_->setText(QString("%1 eV - %2 eV (%3 eV)").arg(qRound(lowerE)).arg(qRound(upperE)).arg(qRound(upperE - lowerE)));

	/*
	example for MEG @ 392 0 tilt
	sindb =  0.00203589
	sinbp =  0.995825
	rPrime =  934.696
	cosgp =  0.995637
	cosBeta =  0.0933081
	sinBeta =  0.995637
	gamma =  -1.70245e-06
	beta =  1.47735
	grooveDensity =  1187.82
	sinAlpha =  0.999391
	*/

	/*
	qDebug()<< "sindb 0.00203589 = " << sindb;
	qDebug()<< "sinbp 0.995825 = " << sinbp;
	qDebug()<< "rPrime 934.696 = " << rPrime;
	qDebug() <<"cosgp 0.995637 = " << cosgp;
	qDebug()<< "cosBeta 0.0933081 = " << cosBeta;
	qDebug()<< "sinBeta 0.995637= " << sinBeta;
	qDebug()<< "gratingRadius = " << gratingRadius;
	qDebug()<< "gamma -1.70245e-06 = " << gamma;
	qDebug()<< "beta 1.47735 = " << beta;
	qDebug()<< "tiltOffsetBox_->value() = " << tiltOffsetBox_->value();
	qDebug()<< "grooveDensity 1187.82 = " << grooveDensity;
	qDebug()<< "energyBox_->value() = " << energyBox_->value();
	qDebug()<< "sinAlpha 0.999391 = " << sinAlpha;
	qDebug()<< "sinAlpha - (grooveDensity * 0.0012398417/energyBox_->value()) = " << sinAlpha - (grooveDensity * 0.0012398417/energyBox_->value());
	qDebug() << "The new energy range is: " << lowerE << "eV to " << upperE << "eV";
	*/
}

void REIXSXESSpectrometerControlEditor::setupUi()
{
	setWindowTitle("Spectrometer");
	setTitle("Spectrometer");

	QFormLayout *formLayout = new QFormLayout();
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	setLayout(formLayout);

	QLabel * gratingLabel = new QLabel("Grating");
	gratingSelectorBox_ = new QComboBox();

	formLayout->setWidget(0, QFormLayout::LabelRole, gratingLabel);
	formLayout->setWidget(0, QFormLayout::FieldRole, gratingSelectorBox_);

	gratingFeedbackLabel_ = new QLabel("Currently: LEG");
	energyFeedbackLabel_ = new QLabel("at 395 e");
	formLayout->setWidget(2, QFormLayout::LabelRole, gratingFeedbackLabel_);
	formLayout->setWidget(2, QFormLayout::FieldRole, energyFeedbackLabel_);

	QLabel *energyLabel = new QLabel("Energy");
	energyBox_ = createDoubleSpinBox(395, 80, 2400, " eV", 4);

	formLayout->setWidget(3, QFormLayout::LabelRole, energyLabel);
	formLayout->setWidget(3, QFormLayout::FieldRole, energyBox_);

	QLabel *labelDefocus = new QLabel("Defocus");
	defocusOffsetBox_ = createDoubleSpinBox(0, -200, 200, " mm");

	formLayout->setWidget(5, QFormLayout::LabelRole, labelDefocus);
	formLayout->setWidget(5, QFormLayout::FieldRole, defocusOffsetBox_);

	QLabel *labelTiltOffset = new QLabel("Tilt Offset");
	tiltOffsetBox_ = createDoubleSpinBox(0, -8, 8, " deg");

	formLayout->setWidget(6, QFormLayout::LabelRole, labelTiltOffset);
	formLayout->setWidget(6, QFormLayout::FieldRole, tiltOffsetBox_);

	energyRangeLabel_ = new QLabel("361 eV - 438 eV (78eV)");
	formLayout->setWidget(7, QFormLayout::FieldRole, energyRangeLabel_);

	stopButton_ = new QPushButton(QIcon(":/Close.png"), "Stop!");
	moveNowButton_ = new QPushButton(QIcon(":/system-software-update.png"), "Move Now");

	formLayout->setWidget(8, QFormLayout::LabelRole, stopButton_);
	formLayout->setWidget(8, QFormLayout::FieldRole, moveNowButton_);

	QLabel* maskLabel = new QLabel("Grating Mask");
	maskComboBox_ = new QComboBox();
	maskComboBox_->insertItems(0, QStringList() << "Pinhole" << "Mask" << "Out" );

	formLayout->setWidget(10, QFormLayout::LabelRole, maskLabel);
	formLayout->setWidget(10, QFormLayout::FieldRole, maskComboBox_);

	maskFeedbackLabel_ = new QLabel("Currently: Unknown");
	formLayout->setWidget(11, QFormLayout::FieldRole, maskFeedbackLabel_);

	QFont font = gratingFeedbackLabel_->font();
	font.setPointSize(AM_FONT_SMALL);
	gratingFeedbackLabel_->setFont(font);
	energyFeedbackLabel_->setFont(font);
}

void REIXSXESSpectrometerControlEditor::setupConnections()
{
	connect(spectrometer_, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus(double)));
	connect(spectrometer_, SIGNAL(calibrationChanged()), this, SLOT(populateGratingComboBox()));
	connect(spectrometer_, SIGNAL(gratingChanged(int)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_, SIGNAL(movingChanged(bool)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_, SIGNAL(moveSucceeded()), this, SLOT(onSpectrometerMoveSucceeded()));
	connect(spectrometer_, SIGNAL(moveFailed(int)), this, SLOT(onSpectrometerMoveFailed(int)));
	connect(spectrometer_->gratingMask(), SIGNAL(valueChanged(double)), this, SLOT(updateMaskPosition()));
	connect(spectrometer_->gratingMask(), SIGNAL(connected(bool)), this, SLOT(updateMaskPosition()));
	connect(maskComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_maskComboBox_currentIndexChanged(QString)));
	connect(energyBox_, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus()));
	connect(gratingSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingComboBoxActivated(int)));

	connect(stopButton_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
	connect(moveNowButton_, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));

	connect(energyBox_, SIGNAL(valueChanged(double)), this, SLOT(updateEnergyRange()));
	connect(tiltOffsetBox_, SIGNAL(valueChanged(double)), this, SLOT(updateEnergyRange()));
	connect(gratingSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEnergyRange()));
}

QDoubleSpinBox *REIXSXESSpectrometerControlEditor::createDoubleSpinBox(double value, double min, double max, QString suffix, double decimals)
{
	QDoubleSpinBox* spinBox = new QDoubleSpinBox();
	spinBox->setValue(value);
	spinBox->setMinimum(min);
	spinBox->setMaximum(max);
	spinBox->setSuffix(suffix);
	spinBox->setDecimals((int)decimals);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(spinBox->sizePolicy().hasHeightForWidth());
	spinBox->setSizePolicy(sizePolicy);

	return spinBox;
}
