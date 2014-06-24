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


#include "REIXSXESSpectrometerControlEditor.h"
#include "ui_REIXSXESSpectrometerControlEditor.h"

#include "util/AMErrorMonitor.h"
#include <QStringBuilder>
#include <QMessageBox>

#include "util/AMFontSizes.h"

REIXSXESSpectrometerControlEditor::REIXSXESSpectrometerControlEditor(REIXSSpectrometer* spectrometer, QWidget *parent) :
	QGroupBox(parent),
	ui_(new Ui::REIXSXESSpectrometerControlEditor)
{
	ui_->setupUi(this);
	QFont font = ui_->gratingFeedbackLabel->font();
	font.setPointSize(AM_FONT_SMALL);
	ui_->gratingFeedbackLabel->setFont(font);
	ui_->energyFeedbackLabel->setFont(font);

	spectrometer_ = spectrometer;

	populateGratingComboBox();
	updateMaskPosition();
	updateCurrentEnergyStatus();
	if(spectrometer_->eV() > 0) ui_->energyBox->setValue(spectrometer_->eV());

	connect(spectrometer_, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus(double)));
	connect(spectrometer_, SIGNAL(calibrationChanged()), this, SLOT(populateGratingComboBox()));
	connect(spectrometer_, SIGNAL(gratingChanged(int)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_, SIGNAL(movingChanged(bool)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_->gratingMask(), SIGNAL(valueChanged(double)), this, SLOT(updateMaskPosition()));
	connect(spectrometer_->gratingMask(), SIGNAL(connected(bool)), this, SLOT(updateMaskPosition()));

	connect(ui_->energyBox, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus()));
	connect(ui_->gratingSelectorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingComboBoxActivated(int)));


	connect(ui_->moveNowButton, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));

	connect(spectrometer_, SIGNAL(moveSucceeded()), this, SLOT(onSpectrometerMoveSucceeded()));
	connect(spectrometer_, SIGNAL(moveFailed(int)), this, SLOT(onSpectrometerMoveFailed(int)));

	connect(ui_->stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

	connect(ui_->energyBox, SIGNAL(valueChanged(double)), this, SLOT(updateEnergyRange()));
	connect(ui_->tiltOffsetBox, SIGNAL(valueChanged(double)), this, SLOT(updateEnergyRange()));
	connect(ui_->gratingSelectorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEnergyRange()));



}

REIXSXESSpectrometerControlEditor::~REIXSXESSpectrometerControlEditor()
{
	delete ui_;
}


//void REIXSXESSpectrometerControlEditor::onMoveButtonClicked()
//{
//	spectrometer_->specifyGrating(ui_->gratingSelectorBox->currentIndex());
//	spectrometer_->specifyDetectorTiltOffset(ui_->tiltOffsetBox->value());
//	spectrometer_->specifyFocusOffset(ui_->defocusOffsetBox->value());

//	int failureExplanation = spectrometer_->move(ui_->energyBox->value());
//	if(failureExplanation != AMControl::NoFailure)
//		onSpectrometerMoveFailed(failureExplanation);
//}  DM UPDATE CODE TO VERIFY GRATING CHANGE FOLLOWS:

void REIXSXESSpectrometerControlEditor::onMoveButtonClicked()
{
	int confirm = QMessageBox::Yes;
	if (spectrometer_->grating() != ui_->gratingSelectorBox->currentIndex())
		{
			QMessageBox confirmBox;
			confirmBox.setText("A new grating has been selected.");
			confirmBox.setInformativeText("Do you want to change gratings?");
			confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Abort);
			confirm = confirmBox.exec();
		}

	if (confirm == QMessageBox::Yes)
	{
	spectrometer_->specifyGrating(ui_->gratingSelectorBox->currentIndex());
	spectrometer_->specifyDetectorTiltOffset(ui_->tiltOffsetBox->value());
	spectrometer_->specifyFocusOffset(ui_->defocusOffsetBox->value());

	int failureExplanation = spectrometer_->move(ui_->energyBox->value());
	if(failureExplanation != AMControl::NoFailure)
		onSpectrometerMoveFailed(failureExplanation);
	}
	else ui_->gratingSelectorBox->setCurrentIndex(spectrometer_->grating());
}

void REIXSXESSpectrometerControlEditor::onGratingComboBoxActivated(int grating)
{
	Q_UNUSED(grating)
	ui_->energyBox->setMaximum(spectrometer_->spectrometerCalibration()->gratingAt(grating).evRangeMax());
	ui_->energyBox->setMinimum(spectrometer_->spectrometerCalibration()->gratingAt(grating).evRangeMin());
	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::populateGratingComboBox()
{
	ui_->gratingSelectorBox->blockSignals(true);

	ui_->gratingSelectorBox->clear();

	for(int i=0, cc=spectrometer_->spectrometerCalibration()->gratingCount(); i<cc; i++)
		ui_->gratingSelectorBox->addItem(spectrometer_->spectrometerCalibration()->gratingAt(i).name());


	if(spectrometer_->grating() != -1) {
		ui_->gratingSelectorBox->setCurrentIndex(spectrometer_->grating());
		ui_->energyBox->setMaximum(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).evRangeMax());
		ui_->energyBox->setMinimum(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).evRangeMin());

	}
	ui_->gratingSelectorBox->blockSignals(false);

	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus(double eV)
{
	if(eV < 0) {
		ui_->energyFeedbackLabel->setText("out of position");
		//ui_->energyFeedbackLabel->setText(QString("at %1").arg(eV));

	}

	else if(fabs(eV - ui_->energyBox->value()) < 0.001) {
		ui_->energyFeedbackLabel->setText(QString());	// we're exactly what the user asked for; all is well, don't need to show anything.
	}
	else {
		ui_->energyFeedbackLabel->setText(QString("at %1").arg(eV));
	}
}

void REIXSXESSpectrometerControlEditor::updateMaskPosition()
{
	double arg1 = spectrometer_->gratingMask()->value();


	if (abs(arg1 - 8.5) < 0.01) {
		ui_->maskFeedbackLabel->setText(QString("Currently: Pinhole"));
		ui_->maskComboBox->blockSignals(true);
		ui_->maskComboBox->setCurrentIndex(0);
		ui_->maskComboBox->blockSignals(false);
	}
	else if (abs(arg1 - 12.9) < 0.01) {
		ui_->maskFeedbackLabel->setText(QString("Currently: Mask"));
		ui_->maskComboBox->blockSignals(true);
		ui_->maskComboBox->setCurrentIndex(1);
		ui_->maskComboBox->blockSignals(false);

	}
	else if (arg1 < 2) {
		ui_->maskFeedbackLabel->setText(QString("Currently: Out"));
		ui_->maskComboBox->blockSignals(true);
		ui_->maskComboBox->setCurrentIndex(2);
		ui_->maskComboBox->blockSignals(false);

	}
	else
		ui_->maskFeedbackLabel->setText(QString("Currently: %1").arg(arg1));
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus() {
	updateCurrentEnergyStatus(spectrometer_->value());
}

void REIXSXESSpectrometerControlEditor::updateCurrentGratingStatus()
{
	if(spectrometer_->grating() == -1) {

		ui_->gratingFeedbackLabel->setText("Currently: unknown");

	}
	else if(spectrometer_->gratingInPosition() == false) {
		ui_->gratingFeedbackLabel->setText(
					QString("Currently: %1 (out of position)")
					.arg(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name()));
	}
	else if(spectrometer_->grating() != ui_->gratingSelectorBox->currentIndex()) {
		ui_->gratingFeedbackLabel->setText(("Currently: " % spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name()));
	}
	else {
		// our drop-down selection matches the current grating, and it's in position. All is well -- don't need to tell the user anything.
		ui_->gratingFeedbackLabel->setText(QString());
	}
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveSucceeded()
{
//	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, QString("Spectrometer move to %1 finished.").arg(spectrometer_->value())));
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveFailed(int reason)
{
	Q_UNUSED(reason)
//	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer Move Failed"));
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

			int grating = ui_->gratingSelectorBox->currentIndex();

			double grooveDensity = spectrometer_->spectrometerCalibration()->gratingAt(grating).grooveDensity();
			double gratingRadius = spectrometer_->spectrometerCalibration()->gratingAt(grating).radius();
			double sinAlpha = sin(spectrometer_->spectrometerCalibration()->d2r(spectrometer_->spectrometerCalibration()->gratingAt(grating).alpha()));

			// Variables. From here on, we work in radians, instead of the degree convention used by REIXSXESCalibration.
			// beta is also the nominal incidence angle onto the detector. Get beta from spectrometer position:
			double beta = asin(sinAlpha - (grooveDensity * 0.0012398417/ui_->energyBox->value()));

			double sinBeta = sin(beta);
			double cosBeta = cos(beta);

			double rPrime = gratingRadius * cosBeta;


			double gamma = spectrometer_->spectrometerCalibration()->d2r(ui_->tiltOffsetBox->value());

			double singp = cosBeta*cos(gamma) + sinBeta*sin(gamma);
			double cosgp = sinBeta*cos(gamma) - cosBeta*sin(gamma);
			// Calculate bottom half of the axis. (low energies). Sign is -1

			double dx = 20;
			// sindb: sin("delta Beta"): the angle difference from the nominal beta.
			double sindb = -1 *( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*-1) );	//you can derive this from sinA/a=sinB/b and c^2=a^2+b^2-2ab*cosC
			//bp ("beta-prime") is the diffraction angle at detector point 'i'; sinbp = sin( beta + db )
			//																		 = sinb*cos(db) + cosb*sindb
			//																		 = sinb*sqrt(1-sin^2(db)) + cosb*sindb
			double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
			//solving the grating equation for eV:
			lowerE = 0.0012398417*grooveDensity / (sinAlpha - sinbp);	// NOTE: we're adding in the user-specified energy offset here.

			sindb = ( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp) );
			sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
			upperE = 0.0012398417*grooveDensity / (sinAlpha - sinbp);	// NOTE: we're adding in the user-specified energy offset here.

			ui_->energyRangeLabel->setText(QString("%1 eV - %2 eV (%3 eV)").arg(qRound(lowerE)).arg(qRound(upperE)).arg(qRound(upperE - lowerE)));

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

//			qDebug()<< "sindb 0.00203589 = " << sindb;
//			qDebug()<< "sinbp 0.995825 = " << sinbp;
//			qDebug()<< "rPrime 934.696 = " << rPrime;
//			qDebug() <<"cosgp 0.995637 = " << cosgp;
//			qDebug()<< "cosBeta 0.0933081 = " << cosBeta;
//			qDebug()<< "sinBeta 0.995637= " << sinBeta;
//			qDebug()<< "gratingRadius = " << gratingRadius;
//			qDebug()<< "gamma -1.70245e-06 = " << gamma;
//			qDebug()<< "beta 1.47735 = " << beta;
//			qDebug()<< "ui_->tiltOffsetBox->value() = " << ui_->tiltOffsetBox->value();
//			qDebug()<< "grooveDensity 1187.82 = " << grooveDensity;
//			qDebug()<< "ui_->energyBox->value() = " << ui_->energyBox->value();
//			qDebug()<< "sinAlpha 0.999391 = " << sinAlpha;
//			qDebug()<< "sinAlpha - (grooveDensity * 0.0012398417/ui_->energyBox->value()) = " << sinAlpha - (grooveDensity * 0.0012398417/ui_->energyBox->value());




//			qDebug() << "The new energy range is: " << lowerE << "eV to " << upperE << "eV";



}

