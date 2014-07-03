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


#include "REIXSSidebar.h"
#include "ui_REIXSSidebar.h"

#include <QBoxLayout>

#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "ui/REIXS/REIXSActionBasedControlEditor.h"
#include "ui/REIXS/REIXSScalerView.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRunner3.h"


REIXSSidebar::REIXSSidebar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::REIXSSidebar)
{
	// Setup additional UI elements
	////////////////////////
	ui->setupUi(this);
	//ui->verticalLayout->insertWidget(1, new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));

	beamlineEnergyEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->energy());
	ui->beamlineFormLayout->setWidget(1, QFormLayout::FieldRole, beamlineEnergyEditor_);

	userEnergyOffestEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->userEnergyOffset());
	ui->beamlineFormLayout->setWidget(2, QFormLayout::FieldRole, userEnergyOffestEditor_);

	gratingSelector_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoGratingSelector());
	ui->beamlineFormLayout->setWidget(3, QFormLayout::FieldRole, gratingSelector_);
	gratingSelector_->setEnabled(false);
	mirrorSelector_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoMirrorSelector());
	ui->beamlineFormLayout->setWidget(4, QFormLayout::FieldRole, mirrorSelector_);
	mirrorSelector_->setEnabled(false);

	monoSlitEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoSlit());
	ui->beamlineFormLayout->setWidget(5, QFormLayout::FieldRole, monoSlitEditor_);

	epuPolarizationEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->epuPolarization());
	ui->beamlineFormLayout->setWidget(6, QFormLayout::FieldRole, epuPolarizationEditor_);
	epuPolarizationAngleEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->epuPolarizationAngle());
	ui->beamlineFormLayout->setWidget(7, QFormLayout::FieldRole, epuPolarizationAngleEditor_);

	ui->verticalLayout->addStretch();

	detectorsGroupBox = new QGroupBox("Detector Signals:");

	detectorPanelLayout = new QVBoxLayout();

	XESValue = new QLabel("XES:\t\t0 counts");
	XESValue->setFixedHeight(55);
	TFYValue = new QLabel("TFY:\t\t0 counts");
	TFYValue->setFixedHeight(55);
	scalerContinuousButton = new QCheckBox("Enable Real-Time Updates");
	scalerContinuousButton->setChecked(REIXSBeamline::bl()->scaler()->isContinuous());
	detectorPanelLayout->addWidget(XESValue);
	detectorPanelLayout->addWidget(TFYValue);
	detectorPanelLayout->addWidget(new REIXSScalerView());
	detectorPanelLayout->addWidget(scalerContinuousButton);
	detectorsGroupBox->setLayout(detectorPanelLayout);
	ui->verticalLayout->addWidget(detectorsGroupBox);

	//	DetectorPanelLayout->addLayout(ui->gridLayout);
//	ui->gridLayout->addWidget(new REIXSScalerView(),1,1,2,2);
//	ui->detectorSignalsGroupBox->setLayout(DetectorPanelLayout);
//	ui->verticalLayout->addWidget(new REIXSScalerView());





	// Make connections
	//////////////////////

	connect(REIXSBeamline::bl()->valvesAndShutters(), SIGNAL(beamOnChanged(bool)), this, SLOT(onBeamOnChanged(bool)));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(countsPerSecondChanged(double)), this, SLOT(onMCPCountsPerSecondChanged(double)));

	connect(ui->beamOnButton, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	connect(ui->beamOffButton, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));
	connect(scalerContinuousButton, SIGNAL(clicked(bool)), this, SLOT(onScalerContinuousButtonToggled(bool)));
	connect(ui->MonoStopButton, SIGNAL(clicked()), this, SLOT(on_MonoStopButton_clicked()));

	connect(REIXSBeamline::bl()->photonSource()->ringCurrent(), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));

	// Get initial status:
	//////////////////////////

	onBeamOnChanged(REIXSBeamline::bl()->valvesAndShutters()->isBeamOn());
}

REIXSSidebar::~REIXSSidebar()
{
	delete ui;
}

void REIXSSidebar::onMCPCountsPerSecondChanged(double countsPerSecond)
{
	//ui->signalXESValue->setText(QString("%1").arg(countsPerSecond, 0, 'f', 0));
	//ui->signalXESValue->setText(QLocale(QLocale::English).toString(countsPerSecond, 'f', 0));
	XESValue->setText("XES\t\t" + QLocale(QLocale::English).toString(countsPerSecond, 'f', 0) + " counts");

//	if(countsPerSecond == 0)
//		countsPerSecond = 1;	// log(0) is undefined.

	//ui->signalXESBar->setValue(int(log10(countsPerSecond)*100));	// integer scale goes up to 600.  Highest count rate we'll see is 1e6.


}

void REIXSSidebar::onBeamOnButtonClicked()
{
	AMAction3 *beamOn = REIXSBeamline::bl()->buildBeamStateChangeAction(true);
	connect(beamOn, SIGNAL(succeeded()), beamOn, SLOT(deleteLater()));
	connect(beamOn, SIGNAL(cancelled()), beamOn, SLOT(deleteLater()));
	connect(beamOn, SIGNAL(failed()), beamOn, SLOT(deleteLater()));
	beamOn->start();
}

void REIXSSidebar::onBeamOffButtonClicked()
{
	AMAction3 *beamOff = REIXSBeamline::bl()->buildBeamStateChangeAction(false);
	connect(beamOff, SIGNAL(succeeded()), beamOff, SLOT(deleteLater()));
	connect(beamOff, SIGNAL(cancelled()), beamOff, SLOT(deleteLater()));
	connect(beamOff, SIGNAL(failed()), beamOff, SLOT(deleteLater()));
	beamOff->start();
}

void REIXSSidebar::onBeamOnChanged(bool isOn)
{
	if(isOn) {
		ui->beamlineStatusLED->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		ui->beamOnButton->setChecked(true);
	}
	else {
		ui->beamlineStatusLED->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		ui->beamOffButton->setChecked(true);
	}
}

//void REIXSSidebar::onTEYCountsChanged(double counts)
//{
//	ui->signalTEYBar->setValue(int(counts*600./1.e6));
//	ui->signalTEYValue->setText(QString::number(counts, 'e', 2));
//}

void REIXSSidebar::onTFYCountsChanged(double counts)
{
	//ui->signalTFYBar->setValue(int(counts*600./1.e6));
	//ui->signalTFYValue->setText(QString::number(counts, 'e', 2));
	//ui->signalTFYValue->setText(QLocale(QLocale::English).toString(counts, 'f', 0));
	TFYValue->setText("TFY\t\t" + QLocale(QLocale::English).toString(counts, 'f', 0) + " counts");

}

//void REIXSSidebar::onI0CountsChanged(double counts)
//{
//	ui->signalI0Bar->setValue(int(counts*600./1.e6));
//	ui->signalI0Value->setText(QString::number(counts, 'e', 2));
//}

void REIXSSidebar::onScalerContinuousButtonToggled(bool on)
{
	REIXSBeamline::bl()->scaler()->setContinuous(on);
}

void REIXSSidebar::onScalerContinuousModeChanged(double on)
{
	scalerContinuousButton->setChecked(bool(on));
}

void REIXSSidebar::on_MonoStopButton_clicked()
{
	REIXSBeamline::bl()->photonSource()->energy()->stop();
	AMActionRunner3::scanActionRunner()->cancelCurrentAction();
	AMActionRunner3::workflow()->cancelCurrentAction();
}

void REIXSSidebar::onRingCurrentChanged(double current)
{
	ui->ringCurrentValue->setText((QString("%1 mA").arg(int(current)))); //REIXSBeamline::bl()->photonSource()->ringCurrent()));
}

