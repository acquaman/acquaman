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
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QToolButton>

#include "beamline/REIXS/REIXSBeamline.h"
#include "ui/beamline/AMControlMoveButton.h"

REIXSSampleChamberButtonPanel::REIXSSampleChamberButtonPanel(QWidget *parent) :
	QWidget(parent)
{
	setObjectName("REIXSSampleChamberButtonPanel");

	resize(527, 197);
	setupUi();
	initializeUiComponents();

	connect(stopAll_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
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
	QIcon iconPrevious;
	iconPrevious.addFile(":/22x22/go-previous-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconNext;
	iconNext.addFile(":/22x22/go-next-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconDown;
	iconDown.addFile(":/22x22/go-down-dark.png", QSize(), QIcon::Normal, QIcon::Off);
	QIcon iconStop;
	iconStop.addFile(":/stop.png", QSize(), QIcon::Normal, QIcon::Off);

	QSizePolicy customizedSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	customizedSizePolicy.setHorizontalStretch(0);
	customizedSizePolicy.setVerticalStretch(0);
	customizedSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());

	setSizePolicy(customizedSizePolicy);

	QGridLayout *gridLayout = new QGridLayout();
	setLayout(gridLayout);

	QLabel *sampleLabel1 = new QLabel("Front View (Y, Z, \316\270)");
	customizedSizePolicy.setHeightForWidth(sampleLabel1->sizePolicy().hasHeightForWidth());
	sampleLabel1->setSizePolicy(customizedSizePolicy);

	QFrame *line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);

	QLabel *sampleLabel2 = new QLabel("Along Beam");
	customizedSizePolicy.setHeightForWidth(sampleLabel2->sizePolicy().hasHeightForWidth());
	sampleLabel2->setSizePolicy(customizedSizePolicy);

	line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);

	line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);

	QLabel *loadLockLabel = new QLabel("Load Lock");

	customizedSizePolicy.setHeightForWidth(loadLockLabel->sizePolicy().hasHeightForWidth());
	loadLockLabel->setSizePolicy(customizedSizePolicy);

	gridLayout->addWidget(sampleLabel1, 0, 0, 1, 4);
	gridLayout->addWidget(line, 0, 4, 5, 1);
	gridLayout->addWidget(sampleLabel2, 0, 5, 1, 2);
	gridLayout->addWidget(line, 0, 8, 5, 1);
	gridLayout->addWidget(line, 0, 11, 5, 1);
	gridLayout->addWidget(loadLockLabel, 0, 12, 1, 3);

	sampleCW_ = createAMControlMoveButton("CW", iconCW);
	sampleCCW_ = createAMControlMoveButton("CCW", iconCCW);
	sampleXup_ = createAMControlMoveButton("+X", iconNext);
	sampleXdown_ = createAMControlMoveButton("-X", iconPrevious);
	sampleYup_ = createAMControlMoveButton("+Y", iconNext);
	sampleYdown_ = createAMControlMoveButton("-Y", iconPrevious);
	sampleZup_ = createAMControlMoveButton("+Z", iconDown);
	sampleZdown_ = createAMControlMoveButton("-Z", iconUp);

	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
	AMControlMoveButton *loadLockCW = createAMControlMoveButton("CW", iconCW);
	AMControlMoveButton *loadLockZup = createAMControlMoveButton("+Z", iconUp);
	AMControlMoveButton *loadLockCCW = createAMControlMoveButton("CW", iconCCW);
	AMControlMoveButton *loadLockZdown = createAMControlMoveButton("-Z", iconDown);

	gridLayout->addWidget(sampleCW_, 3, 0, 1, 1);
	gridLayout->addWidget(sampleZdown_, 3, 1, 1, 1);
	gridLayout->addWidget(sampleCCW_, 3, 3, 1, 1);
	gridLayout->addWidget(sampleXup_, 3, 6, 1, 1);
	gridLayout->addItem(horizontalSpacer, 3, 10, 1, 1);
	gridLayout->addWidget(loadLockCW, 3, 12, 1, 1);
	gridLayout->addWidget(loadLockZup, 3, 13, 1, 1);
	gridLayout->addWidget(loadLockCCW, 3, 14, 1, 1);

	gridLayout->addWidget(sampleYdown_, 4, 0, 1, 1);
	gridLayout->addWidget(sampleZup_, 4, 1, 1, 1);
	gridLayout->addWidget(sampleYup_, 4, 3, 1, 1);
	gridLayout->addWidget(sampleXdown_, 4, 5, 1, 1);
	gridLayout->addWidget(loadLockZdown, 4, 13, 1, 1);

	stopAll_ = createQToolButton("Stop", iconStop);

	gridLayout->addWidget(stopAll_, 5, 0, 1, 15);
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
	sampleZup_->setStepSizeIndex(2);
	sampleZup_->setDirectionReversed(true);// yup, down is up. Go figure.
	sampleZdown_->setControl(chamber->z());
	sampleZdown_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	sampleZdown_->setStepSizeIndex(2);

	sampleCW_->setControl(chamber->r());
	sampleCW_->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	sampleCW_->setStepSizeIndex(1);
	sampleCW_->setDirectionReversed(true);
	sampleCCW_->setControl(chamber->r());
	sampleCCW_->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	sampleCCW_->setStepSizeIndex(1);
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
	controlMoveButton->setText(text);
	controlMoveButton->setIcon(icon);

	return controlMoveButton;
}
