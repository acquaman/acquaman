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

#include "REIXSXESLoadlockControl.h"
#include "beamline/AMPVControl.h"

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#endif


#include "QBoxLayout"

#include <cmath>
#include <QToolBar>


REIXSXESLoadlockControl::~REIXSXESLoadlockControl(){}

REIXSXESLoadlockControl::REIXSXESLoadlockControl(QWidget *parent) :
	QWidget(parent)
{
	//************CONTORLS**************
	sampleChamber_ = new REIXSSampleChamber(this);




	//***********UI ELEMENTS************

	//Main Vertical Layout, Buttons along the top, video and controls below
	QVBoxLayout* mainLayout = new QVBoxLayout;

	motorFeedback_ = new QLabel(QString("Load Lock Z: %1").arg(sampleChamber_->loadLockZ()->value()));

	mainLayout->addWidget(motorFeedback_);

	//wide central section, controls on left, video on right
	QHBoxLayout* wideLayout = new QHBoxLayout;

	QVBoxLayout* loadLockSlotButtonLayout = new QVBoxLayout;

	loadlockSlotOut_ = new QPushButton("Loadlock Z");
	loadlockSlotOut_->setMinimumHeight(100);
	loadlockSlotOut_->setMinimumWidth(100);


	loadlockSlot4_ = new QPushButton("Loadlock R");
	loadlockSlot4_->setMinimumHeight(100);
	loadlockSlot4_->setMinimumWidth(100);


	loadlockSlot3_ = new QPushButton("Sample x");
	loadlockSlot3_->setMinimumHeight(100);
	loadlockSlot3_->setMinimumWidth(100);


	loadlockSlot2_ = new QPushButton("Sample y");
	loadlockSlot2_->setMinimumHeight(100);
	loadlockSlot2_->setMinimumWidth(100);


	loadlockSlot1_ = new QPushButton("Sample z");
	loadlockSlot1_->setMinimumHeight(100);
	loadlockSlot1_->setMinimumWidth(100);


	loadLockSlotButtonLayout->addStretch(50);
	loadLockSlotButtonLayout->addWidget(loadlockSlotOut_);
	loadLockSlotButtonLayout->addWidget(loadlockSlot4_);
	loadLockSlotButtonLayout->addWidget(loadlockSlot3_);
	loadLockSlotButtonLayout->addWidget(loadlockSlot2_);
	loadLockSlotButtonLayout->addWidget(loadlockSlot1_);

	wideLayout->addLayout(loadLockSlotButtonLayout);

#ifdef AM_MOBILITY_VIDEO_ENABLED
	AMBeamlineCameraWidgetWithSourceTabs *cameraWidget;

	cameraWidget = new AMBeamlineCameraWidgetWithSourceTabs(QUrl("http://v2e1610-101.clsi.ca/mjpg/4/video.mjpg"),"Sample Transfer",0,false);
	wideLayout->addWidget(cameraWidget);
#endif

	mainLayout->addLayout(wideLayout);
	setLayout(mainLayout);

	connect(loadlockSlot1_, SIGNAL(clicked()), this, SLOT(onloadlockSlot1Clicked()));
	connect(loadlockSlot2_, SIGNAL(clicked()), this, SLOT(onloadlockSlot2Clicked()));
	connect(loadlockSlot3_, SIGNAL(clicked()), this, SLOT(onloadlockSlot3Clicked()));
	connect(loadlockSlot4_, SIGNAL(clicked()), this, SLOT(onloadlockSlot4Clicked()));
	connect(loadlockSlotOut_, SIGNAL(clicked()), this, SLOT(onloadlockSlotOutClicked()));


}

void REIXSXESLoadlockControl::onloadlockSlot1Clicked()
{
	motorFeedback_->setText(QString("Sample Z: %1").arg(sampleChamber_->z()->value()));
}

void REIXSXESLoadlockControl::onloadlockSlot2Clicked()
{
	motorFeedback_->setText(QString("Sample Y: %1").arg(sampleChamber_->y()->value()));
}

void REIXSXESLoadlockControl::onloadlockSlot3Clicked()
{
	motorFeedback_->setText(QString("Sample X: %1").arg(sampleChamber_->x()->value()));
}

void REIXSXESLoadlockControl::onloadlockSlot4Clicked()
{
	motorFeedback_->setText(QString("Load Lock R: %1").arg(sampleChamber_->loadLockR()->value()));
}

void REIXSXESLoadlockControl::onloadlockSlotOutClicked()
{
	motorFeedback_->setText(QString("Load Lock Z: %1").arg(sampleChamber_->loadLockZ()->value()));

}





REIXSSampleChamber::~REIXSSampleChamber(){}
REIXSSampleChamber::REIXSSampleChamber(QObject *parent)
	: AMCompositeControl("sampleChamber", "", parent) {

	setDescription("XES Sample Chamber Controls");

	// Motor information here was updated Nov. 2011 for the MDrive motors on the sample chamber, which still don't have unit conversion built into the driver.
	// All motors are currently running at a microstep setting of 256. Therefore one revolution of the motor is: 200*256 = 51200 steps.
	// Units per rev:
	// The X and Y motors are equipped with the IMS "D" lead screw: 0.0833"/rev (2.116mm/rev).
	// The Z stage has a 2.5mm/rev lead screw. However, it has a 10:1 gear reducer before the lead screw, so it's actually 0.25mm/rev.
	// The sample theta stage has 100 teeth on the circumference gear. One motor revolution advances by one tooth, so it's 360deg/100revs, or 3.6deg/rev.
	// The load lock theta stage also has 100 teeth on its circumference gear, for 3.6deg/rev.
	// [Guessing] The load lock Z stage looks like it has the same 2.5mm/lead screw rev. However, it also has a 90-degree gear from the motor to the lead screw with 20 teeth, or 1 lead screw rev/20 motor revs.   ie: (2.5mm/screwRev)*(1screwRev/20rev) = 0.125mm/rev.

	//								name	  PV base name        units unitsPerRev offset microsteps descript. tolerance startTimeoutSecs, parent
	x_ = new CLSMDriveMotorControl("sampleX", "SMTR1610-4-I21-08", "mm", 2.116, 0, 256, "Sample Chamber X", 0.5, 2.0, this);
	x_->setSettlingTime(0.5);
	x_->setMoveStartTolerance(x_->writeUnitConverter()->convertFromRaw(5));
	x_->setContextKnownDescription("X");

	y_ = new CLSMDriveMotorControl("sampleY", "SMTR1610-4-I21-10", "mm", 2.116, 0, 256, "Sample Chamber Y", 0.5, 2.0, this);
	y_->setSettlingTime(0.5);
	y_->setMoveStartTolerance(y_->writeUnitConverter()->convertFromRaw(5));
	y_->setContextKnownDescription("Y");

	z_ = new CLSMDriveMotorControl("sampleZ", "SMTR1610-4-I21-07", "mm", 0.25, 0, 256, "Sample Chamber Z", 0.5, 2.0, this);
	z_->setSettlingTime(0.5);
	z_->setMoveStartTolerance(z_->writeUnitConverter()->convertFromRaw(5));
	z_->setContextKnownDescription("Z");

	r_ = new CLSMDriveMotorControl("sampleTheta", "SMTR1610-4-I21-11", "deg", 3.6, 0, 256, "Sample Chamber Theta", 0.5, 2.0, this);
	r_->setSettlingTime(0.5);
	r_->setMoveStartTolerance(r_->writeUnitConverter()->convertFromRaw(5));
	r_->setContextKnownDescription("Theta");

	loadLockZ_ = new CLSMDriveMotorControl("loadLockZ", "SMTR1610-4-I21-09", "mm", 0.125, 0, 256, "Load Lock Z", 0.5, 2.0, this);
	loadLockZ_->setSettlingTime(0.2);
	loadLockZ_->setMoveStartTolerance(loadLockZ_->writeUnitConverter()->convertFromRaw(5));

	loadLockR_ = new CLSMDriveMotorControl("loadLockTheta", "SMTR1610-4-I21-12", "deg", 3.6, 0, 256, "Load Lock Theta", 0.5, 2.0, this);
	loadLockR_->setSettlingTime(0.2);
	loadLockR_->setMoveStartTolerance(loadLockR_->writeUnitConverter()->convertFromRaw(5));

	addChildControl(x_);
	addChildControl(y_);
	addChildControl(z_);
	addChildControl(r_);
	addChildControl(loadLockZ_);
	addChildControl(loadLockR_);
}
