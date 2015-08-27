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

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include "beamline/REIXS/REIXSBeamline.h"
#include "ui/beamline/AMControlMoveButton.h"
#include "ui/beamline/AMXYThetaControlMoveButton.h"


REIXSSampleChamberButtonPanel::REIXSSampleChamberButtonPanel(QWidget *parent) :
	QWidget(parent)
{
	setObjectName("REIXSSampleChamberButtonPanel");

	resize(527, 197);
	setupUi();
	initializeUiComponents();


	connect(stopAll_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
	connect(stopAll2_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
	connect(angleOffsetSpinBox_, SIGNAL(valueChanged(double)), sampleIn_, SLOT(setAngleOffset(double)));
	connect(angleOffsetSpinBox_, SIGNAL(valueChanged(double)), sampleOut_, SLOT(setAngleOffset(double)));
	connect(angleOffsetSpinBox_, SIGNAL(valueChanged(double)), sampleLeft_, SLOT(setAngleOffset(double)));
	connect(angleOffsetSpinBox_, SIGNAL(valueChanged(double)), sampleRight_, SLOT(setAngleOffset(double)));
}

REIXSSampleChamberButtonPanel::~REIXSSampleChamberButtonPanel()
{
}

void REIXSSampleChamberButtonPanel::onStopButtonClicked()
{
	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	chamber->x()->stop();
	chamber->y()->stop();
	chamber->z()->stop();
	chamber->r()->stop();
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
		sampleXdown_->press(isDown);
		break;
	case 4:
		sampleXup_->press(isDown);
		break;
	case 7:
		sampleYdown_->press(isDown);
		break;
	case 5:
		sampleYup_->press(isDown);
		break;


	case 12:
		sampleZup_->press(isDown);
		break;
	case 14:
		sampleZdown_->press(isDown);
		break;

	case 11:
		sampleCW_->press(isDown);
		break;
	case 10:
		sampleCCW_->press(isDown);
		break;
	}
}

void REIXSSampleChamberButtonPanel::setupUi()
{
	QIcon iconCW;
	iconCW.addFile(":/22x22/arrow-CW.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconCCW;
	iconCCW.addFile(":/22x22/arrow-CCW.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconUp;
	iconUp.addFile(":/22x22/go-up-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconDown;
	iconDown.addFile(":/22x22/go-down-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconPrevious;
	iconPrevious.addFile(":/22x22/go-previous-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconNext;
	iconNext.addFile(":/22x22/go-next-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconOut;
	iconOut.addFile(":/22x22/go-upleft-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconIn;
	iconIn.addFile(":/22x22/go-downright-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconStop;
	iconStop.addFile(":/stop.png", QSize(), QIcon::Normal, QIcon::Off);

	QSizePolicy customizedSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	customizedSizePolicy.setHorizontalStretch(0);
	customizedSizePolicy.setVerticalStretch(0);
	customizedSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());

	setSizePolicy(customizedSizePolicy);

	QGridLayout *gridLayout = new QGridLayout();
	setLayout(gridLayout);

	QLabel *beamViewLabel = new QLabel("Beam View (Y, Z)");
	customizedSizePolicy.setHeightForWidth(beamViewLabel->sizePolicy().hasHeightForWidth());
	beamViewLabel->setSizePolicy(customizedSizePolicy);

	QLabel *spectrometerViewLabel = new QLabel("Spectrometer View (X, R)");
	customizedSizePolicy.setHeightForWidth(spectrometerViewLabel->sizePolicy().hasHeightForWidth());
	spectrometerViewLabel->setSizePolicy(customizedSizePolicy);

	QLabel *inPlaneLabel = new QLabel("\nMove in Plate Plane:");
	customizedSizePolicy.setHeightForWidth(inPlaneLabel->sizePolicy().hasHeightForWidth());
	inPlaneLabel->setSizePolicy(customizedSizePolicy);


	sampleCW_ = createAMControlMoveButton("CW", iconCW);
	sampleCCW_ = createAMControlMoveButton("CCW", iconCCW);
	sampleXdown_ = createAMControlMoveButton("+X", iconNext);
	sampleXup_ = createAMControlMoveButton("-X", iconPrevious);
	sampleYup_ = createAMControlMoveButton("+Y", iconNext);
	sampleYdown_ = createAMControlMoveButton("-Y", iconPrevious);
	sampleZup_ = createAMControlMoveButton("-Z", iconDown);
	sampleZdown_ = createAMControlMoveButton("+Z", iconUp);
	sampleZup2_ = createAMControlMoveButton("-Z", iconDown);
	sampleZdown2_ = createAMControlMoveButton("+Z", iconUp);
	sampleLeft_ = createAMXYThetaControlMoveButton("Left", iconPrevious);
	sampleRight_ = createAMXYThetaControlMoveButton("Right", iconNext);
	sampleIn_    = createAMXYThetaControlMoveButton("In", iconIn);
	sampleOut_   = createAMXYThetaControlMoveButton("Out", iconOut);

	angleOffsetSpinBox_ = new QDoubleSpinBox();
	angleOffsetSpinBox_->setMaximum(15);
	angleOffsetSpinBox_->setMinimum(-15);
	angleOffsetSpinBox_->setSingleStep(0.1);
	angleOffsetSpinBox_->setDecimals(1);
	angleOffsetSpinBox_->setSuffix(" deg");
	QLabel *angleOffsetLabel = new QLabel("Angle\nOffset:");



	QFrame *line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);


	gridLayout->addWidget(beamViewLabel, 0, 0, 1, 3);
	gridLayout->addWidget(sampleZdown_, 3, 1, 1, 1);
	gridLayout->addWidget(sampleYdown_, 4, 0, 1, 1);
	gridLayout->addWidget(sampleZup_,	4, 1, 1, 1);
	gridLayout->addWidget(sampleYup_,	4, 2, 1, 1);


	gridLayout->addWidget(inPlaneLabel, 3, 4, 1, 3);
	gridLayout->addWidget(angleOffsetLabel, 4, 4, 1, 1);
	gridLayout->addWidget(angleOffsetSpinBox_, 4, 5, 1, 2);
	gridLayout->addWidget(sampleLeft_, 6, 4, 1, 1);
	gridLayout->addWidget(sampleRight_,	6, 6, 1, 1);
	gridLayout->addWidget(sampleZup2_, 6, 5, 1, 1);
	gridLayout->addWidget(sampleZdown2_,5, 5, 1, 1);
	gridLayout->addWidget(sampleOut_, 5, 4, 1, 1);
	gridLayout->addWidget(sampleIn_,5, 6, 1, 1);


	gridLayout->addWidget(spectrometerViewLabel, 0, 8, 1, 3);
	gridLayout->addWidget(sampleCW_,	3, 8, 1, 1);
	gridLayout->addWidget(sampleCCW_,	3, 9, 1, 1);
	gridLayout->addWidget(sampleXup_,	4, 8, 1, 1);
	gridLayout->addWidget(sampleXdown_, 4, 9, 1, 1);


	gridLayout->setColumnStretch(0,0);
	gridLayout->setColumnStretch(1,0);
	gridLayout->setColumnStretch(2,0);
	gridLayout->setColumnStretch(3,1);
	gridLayout->setColumnStretch(4,0);
	gridLayout->setColumnStretch(5,0);
	gridLayout->setColumnStretch(6,0);
	gridLayout->setColumnStretch(7,1);
	gridLayout->setColumnStretch(8,0);
	gridLayout->setColumnStretch(9,0);
	gridLayout->setColumnStretch(10,0);


	stopAll_ = createQToolButton("Stop", iconStop);
	stopAll2_ = createQToolButton("Stop", iconStop);
	stopAll2_->setMinimumWidth(64);
	stopAll3_ = createQToolButton("Stop", iconStop);

	gridLayout->addWidget(stopAll_, 5, 0, 1, 3);
	gridLayout->addWidget(stopAll2_, 5, 8, 1, 2);
	gridLayout->addWidget(stopAll3_, 7, 4, 1, 3);

}

void REIXSSampleChamberButtonPanel::initializeUiComponents()
{
	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	sampleXup_->setControl(chamber->x());
	sampleXup_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleXup_->setStepSizeIndex(1);
	sampleXup_->setDirectionReversed(true);
	sampleXdown_->setControl(chamber->x());
	sampleXdown_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleXdown_->setStepSizeIndex(1);

	sampleYup_->setControl(chamber->y());
	sampleYup_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleYup_->setStepSizeIndex(1);
	sampleYdown_->setControl(chamber->y());
	sampleYdown_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleYdown_->setStepSizeIndex(1);
	sampleYdown_->setDirectionReversed(true);

	sampleZup_->setControl(chamber->z());
	sampleZup_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	sampleZup_->setStepSizeIndex(1);
	sampleZup_->setDirectionReversed(true);// yup, down is up. Go figure.
	sampleZdown_->setControl(chamber->z());
	sampleZdown_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	sampleZdown_->setStepSizeIndex(1);

	sampleZup2_->setControl(chamber->z());
	sampleZup2_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	sampleZup2_->setStepSizeIndex(1);
	sampleZup2_->setDirectionReversed(true);// yup, down is up. Go figure.
	sampleZdown2_->setControl(chamber->z());
	sampleZdown2_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	sampleZdown2_->setStepSizeIndex(1);

	sampleCW_->setControl(chamber->r());
	sampleCW_->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	sampleCW_->setStepSizeIndex(1);
	sampleCW_->setDirectionReversed(true);
	sampleCCW_->setControl(chamber->r());
	sampleCCW_->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	sampleCCW_->setStepSizeIndex(1);

	sampleLeft_->setControlX(chamber->y());
	sampleLeft_->setControlY(chamber->x());
	sampleLeft_->setControlTheta(chamber->r());
	sampleLeft_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleLeft_->setStepSizeIndex(1);
	sampleLeft_->setDirectionXReversed(true);
	sampleLeft_->setDirectionYReversed(true);
	sampleRight_->setControlX(chamber->y());
	sampleRight_->setControlY(chamber->x());
	sampleRight_->setControlTheta(chamber->r());
	sampleRight_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleRight_->setStepSizeIndex(1);

	sampleIn_->setControlX(chamber->x());
	sampleIn_->setControlY(chamber->y());
	sampleIn_->setControlTheta(chamber->r());
	sampleIn_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleIn_->setStepSizeIndex(1);
	sampleIn_->setDirectionYReversed(true);
	sampleOut_->setControlX(chamber->x());
	sampleOut_->setControlY(chamber->y());
	sampleOut_->setControlTheta(chamber->r());
	sampleOut_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleOut_->setStepSizeIndex(1);
	sampleOut_->setDirectionXReversed(true);

	sampleLeft_->setAngleOffset(0);
	sampleRight_->setAngleOffset(0);
	sampleIn_->setAngleOffset(0);
	sampleOut_->setAngleOffset(0);
	angleOffsetSpinBox_->setValue(0);
}

QToolButton *REIXSSampleChamberButtonPanel::createQToolButton(QString text, QIcon icon)
{
	QToolButton *toolButton= new QToolButton();
	toolButton->setIconSize(QSize(32, 32));
	toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
	toolButton->setText(text);
	toolButton->setIcon(icon);

	QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(toolButton->sizePolicy().hasHeightForWidth());
	toolButton->setSizePolicy(sizePolicy);

	return toolButton;
}

AMControlMoveButton *REIXSSampleChamberButtonPanel::createAMControlMoveButton(QString text, QIcon icon)
{
	QFont font;
	font.setPointSize(9);

	AMControlMoveButton *controlMoveButton = new AMControlMoveButton();
	controlMoveButton->setMinimumSize(QSize(52, 46));
	controlMoveButton->setMaximumSize(QSize(52, 46));
	controlMoveButton->setFont(font);
	controlMoveButton->setIconSize(QSize(22, 22));
	controlMoveButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	controlMoveButton->setToolTip(text);
	controlMoveButton->setIcon(icon);

	return controlMoveButton;
}

AMXYThetaControlMoveButton *REIXSSampleChamberButtonPanel::createAMXYThetaControlMoveButton(QString text, QIcon icon)
{
	QFont font;
	font.setPointSize(9);

	AMXYThetaControlMoveButton *controlMoveButton = new AMXYThetaControlMoveButton();
	controlMoveButton->setMinimumSize(QSize(52, 46));
	controlMoveButton->setMaximumSize(QSize(52, 46));
	controlMoveButton->setFont(font);
	controlMoveButton->setIconSize(QSize(22, 22));
	controlMoveButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	controlMoveButton->setToolTip(text);
	controlMoveButton->setIcon(icon);

	return controlMoveButton;
}
