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


#include "REIXSSidebar.h"
#include "ui_REIXSSidebar.h"

#include <QBoxLayout>

#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "ui/REIXS/REIXSActionBasedControlEditor.h"
#include "ui/REIXS/REIXSActionBasedEnumControlEditor.h"


REIXSSidebar::REIXSSidebar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::REIXSSidebar)
{
	// Setup additional UI elements
	////////////////////////
	ui->setupUi(this);
	ui->verticalLayout->insertWidget(1, new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));

	beamlineEnergyEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->energy());
	ui->beamlineFormLayout->setWidget(1, QFormLayout::FieldRole, beamlineEnergyEditor_);

	monoSlitEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoSlit());
	ui->beamlineFormLayout->setWidget(4, QFormLayout::FieldRole, monoSlitEditor_);

	gratingSelector_ = new REIXSActionBasedEnumControlEditor(REIXSBeamline::bl()->photonSource()->monoGratingSelector());
	ui->beamlineFormLayout->setWidget(2, QFormLayout::FieldRole, gratingSelector_);
	mirrorSelector_ = new REIXSActionBasedEnumControlEditor(REIXSBeamline::bl()->photonSource()->monoMirrorSelector());
	ui->beamlineFormLayout->setWidget(3, QFormLayout::FieldRole, mirrorSelector_);


	// Make connections
	//////////////////////

	connect(REIXSBeamline::bl()->valvesAndShutters(), SIGNAL(beamOnChanged(bool)), this, SLOT(onBeamOnChanged(bool)));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(countsPerSecondChanged(double)), this, SLOT(onMCPCountsPerSecondChanged(double)));

	connect(ui->beamOnButton, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	connect(ui->beamOffButton, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));

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
	ui->signalXESValue->setText(QString("%1").arg(countsPerSecond, 0, 'e', 1));

	if(countsPerSecond == 0)
		countsPerSecond = 1;	// log(0) is undefined.

	ui->signalXESBar->setValue(int(log10(countsPerSecond)*100));	// integer scale goes up to 600.  Highest count rate we'll see is 1e6.

}

#include "actions2/AMActionRunner.h"
#include "actions2/actions/REIXS/REIXSBeamOnOffAction.h"
#include <QMessageBox>

void REIXSSidebar::onBeamOnButtonClicked()
{
	if(AMActionRunner::s()->actionRunning()) {
		if(QMessageBox::question(this, "Really turn the beam on?", "Actions are currently running in the workflow. Are you sure you want to turn the beam on?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
			AMActionRunner::s()->runActionImmediately(new REIXSBeamOnOffAction(true));	// runs in background.
	}
	else {
		AMActionRunner::s()->runActionImmediatelyInQueue(new REIXSBeamOnOffAction(true));
	}
}

void REIXSSidebar::onBeamOffButtonClicked()
{
	if(AMActionRunner::s()->actionRunning()) {
		if(QMessageBox::question(this, "Really turn the beam off?", "Actions are currently running in the workflow. Are you sure you want to turn the beam off?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
			AMActionRunner::s()->runActionImmediately(new REIXSBeamOnOffAction(false));	// runs in background.
	}
	else {
		AMActionRunner::s()->runActionImmediatelyInQueue(new REIXSBeamOnOffAction(false));
	}
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
