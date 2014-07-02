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


#include "REIXSSampleChamberButtonPanel.h"
#include "ui_REIXSSampleChamberButtonPanel.h"

#include "ui/beamline/AMControlMoveButton.h"
#include "beamline/REIXS/REIXSBeamline.h"

REIXSSampleChamberButtonPanel::REIXSSampleChamberButtonPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::REIXSSampleChamberButtonPanel)
{
    ui->setupUi(this);

	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	ui->sampleXup->setControl(chamber->x());
	ui->sampleXup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleXup->setStepSizeIndex(1);
	ui->sampleXup->setDirectionReversed(true);
	ui->sampleXdown->setControl(chamber->x());
	ui->sampleXdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleXdown->setStepSizeIndex(1);

	ui->sampleYup->setControl(chamber->y());
	ui->sampleYup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleYup->setStepSizeIndex(1);
	ui->sampleYdown->setControl(chamber->y());
	ui->sampleYdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleYdown->setStepSizeIndex(1);
	ui->sampleYdown->setDirectionReversed(true);

	ui->sampleZup->setControl(chamber->z());
	ui->sampleZup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->sampleZup->setStepSizeIndex(2);
	ui->sampleZup->setDirectionReversed(true);// yup; down is up. Go figure.
	ui->sampleZdown->setControl(chamber->z());
	ui->sampleZdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->sampleZdown->setStepSizeIndex(2);

	ui->sampleCW->setControl(chamber->r());
	ui->sampleCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->sampleCW->setStepSizeIndex(1);
	ui->sampleCW->setDirectionReversed(true);
	ui->sampleCCW->setControl(chamber->r());
	ui->sampleCCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->sampleCCW->setStepSizeIndex(1);

//	ui->loadLockCW->setControl(chamber->loadLockR());
//	ui->loadLockCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
//	ui->loadLockCW->setStepSizeIndex(1);
//	ui->loadLockCCW->setControl(chamber->loadLockR());
//	ui->loadLockCCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
//	ui->loadLockCCW->setDirectionReversed(true);
//	ui->loadLockCCW->setStepSizeIndex(1);

//	ui->loadLockZup->setControl(chamber->loadLockZ());
//	ui->loadLockZup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
//	ui->loadLockZup->setStepSizeIndex(2);
//	ui->loadLockZdown->setControl(chamber->loadLockZ());
//	ui->loadLockZdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
//	ui->loadLockZdown->setDirectionReversed(true);
//	ui->loadLockZdown->setStepSizeIndex(2);

	connect(ui->stopAll, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
}

REIXSSampleChamberButtonPanel::~REIXSSampleChamberButtonPanel()
{
	delete ui;
}

void REIXSSampleChamberButtonPanel::onStopButtonClicked()
{
	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	chamber->x()->stop();
	chamber->y()->stop();
	chamber->z()->stop();
	chamber->r()->stop();
	chamber->loadLockR()->stop();
	chamber->loadLockZ()->stop();
}

void REIXSSampleChamberButtonPanel::onJoystickButtonChanged(int buttonId, bool isDown)
{
	// Mapping:
	/*
	  Dpad up: 4
	  Dpad down: 6
	  Dpad right: 5
	  Dpad left: 7

	  Triangle: 12
	  X: 14
	  Circle: 13
	  Square: 15

	  Right top trigger: 11
	  Left top trigger: 10

	  Right bottom trigger:9
	  Left bottom trigger: 8

	  Right joystick press: 2
	  Left joystick press:1

	  PS button: 16
	  Select: 0
	  Start: 3
	  */

	switch(buttonId) {
	case 6:
		ui->sampleXdown->press(isDown);
		break;
	case 4:
		ui->sampleXup->press(isDown);
		break;
	case 7:
		ui->sampleYdown->press(isDown);
		break;
	case 5:
		ui->sampleYup->press(isDown);
		break;


	case 12:
		ui->sampleZup->press(isDown);
		break;
	case 14:
		ui->sampleZdown->press(isDown);
		break;

	case 11:
		ui->sampleCW->press(isDown);
		break;
	case 10:
		ui->sampleCCW->press(isDown);
		break;
	}
}
