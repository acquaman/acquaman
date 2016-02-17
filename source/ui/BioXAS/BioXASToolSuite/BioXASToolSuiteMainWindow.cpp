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

#include "BioXASToolSuiteMainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGroupBox>
#include <QBoxLayout>

#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h"

BioXASToolSuiteMainWindow::BioXASToolSuiteMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);

	setWindowTitle("BioXAS Beamline Toolsuite");

	setupUi();
}

BioXASToolSuiteMainWindow::~BioXASToolSuiteMainWindow()
{

}

void BioXASToolSuiteMainWindow::onMainBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASMainBeamLine, false);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onSideBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASSideBeamLine, false);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onImagingBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASImagingBeamLine, false);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onMainBLPseudoMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASMainBeamLine, true);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onSideBLPseudoMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASSideBeamLine, true);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onImagingBLPseudoMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASImagingBeamLine, true);
	displayToolScreen(motorToolScreen);
}

void BioXASToolSuiteMainWindow::onToolWindowClosed()
{
	if (toolWindow_) {
		toolWindow_->deleteLater();
		toolWindow_ = 0; // NULL
	}

	show();
}

void BioXASToolSuiteMainWindow::displayToolScreen(QWidget * toolWindow)
{
	toolWindow_ = toolWindow;

	connect(toolWindow_, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	toolWindow_->move(QApplication::desktop()->screen()->rect().center() - toolWindow_->rect().center());
	toolWindow_->show();
	hide();
}

void BioXASToolSuiteMainWindow::setupUi()
{
	QPushButton *mainBLMotorToolButton = new QPushButton("MAXvMotor Tool");
	QPushButton *mainBLPseudoMotorToolButton = new QPushButton("Pseudo Motor Tool");
	QPushButton *sideBLMotorToolButton = new QPushButton("MAXvMotor Tool");
	QPushButton *sideBLPseudoMotorToolButton = new QPushButton("Pseudo Motor Tool");
	QPushButton *imagingBLMotorToolButton = new QPushButton("MAXvMotor Tool");
	QPushButton *imagingBLPseudoMotorToolButton = new QPushButton("Pseudo Motor Tool");

	connect( mainBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onMainBLMotorToolButtonClicked()) );
	connect( mainBLPseudoMotorToolButton, SIGNAL(clicked()), this, SLOT(onMainBLPseudoMotorToolButtonClicked()) );
	connect( sideBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onSideBLMotorToolButtonClicked()) );
	connect( sideBLPseudoMotorToolButton, SIGNAL(clicked()), this, SLOT(onSideBLPseudoMotorToolButtonClicked()) );
	connect( imagingBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onImagingBLMotorToolButtonClicked()) );
	connect( imagingBLPseudoMotorToolButton, SIGNAL(clicked()), this, SLOT(onImagingBLPseudoMotorToolButtonClicked()) );


	// the main layout
	QVBoxLayout *mainLayout = new QVBoxLayout();
	QGroupBox *mainWidget = new QGroupBox();
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);

	QVBoxLayout *bioXASToolGBLayout ;
	QGroupBox *bioXASToolGB;

	// main beamline
	bioXASToolGBLayout = new QVBoxLayout();
	bioXASToolGBLayout->addWidget(mainBLMotorToolButton);
	bioXASToolGBLayout->addWidget(mainBLPseudoMotorToolButton);

	bioXASToolGB = new QGroupBox("Main beamline tools");
	bioXASToolGB->setLayout(bioXASToolGBLayout);
	mainLayout->addWidget(bioXASToolGB);

	// side beamline
	bioXASToolGBLayout = new QVBoxLayout();
	bioXASToolGBLayout->addWidget(sideBLMotorToolButton);
	bioXASToolGBLayout->addWidget(sideBLPseudoMotorToolButton);

	bioXASToolGB = new QGroupBox("Side beamline tools");
	bioXASToolGB->setLayout(bioXASToolGBLayout);
	mainLayout->addWidget(bioXASToolGB);

	// imaging beamline
	bioXASToolGBLayout = new QVBoxLayout();
	bioXASToolGBLayout->addWidget(imagingBLMotorToolButton);
	bioXASToolGBLayout->addWidget(imagingBLPseudoMotorToolButton);

	bioXASToolGB = new QGroupBox("Imaging beamline tools");
	bioXASToolGB->setLayout(bioXASToolGBLayout);
	mainLayout->addWidget(bioXASToolGB);
}
