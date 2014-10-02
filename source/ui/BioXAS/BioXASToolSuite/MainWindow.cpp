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

#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGroupBox>
#include <QBoxLayout>

#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h"
#include "ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);

	setWindowTitle("BioXAS Beamline Toolsuite");

	setupUi();
}

MainWindow::~MainWindow()
{

}

void MainWindow::onShutterToolButtonClicked()
{
	ShutterToolMainWindow *shutterToolWindow = new ShutterToolMainWindow();
	connect(shutterToolWindow, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	shutterToolWindow->move(QApplication::desktop()->screen()->rect().center() - shutterToolWindow->rect().center());
	shutterToolWindow->show();
	shutterToolWindow->setFixedSize(shutterToolWindow->rect().width(),shutterToolWindow->rect().height());

	hide();
}

void MainWindow::onMainBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASMainBeamLine);
	connect(motorToolScreen, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	motorToolScreen->move(QApplication::desktop()->screen()->rect().center() - motorToolScreen->rect().center());

	motorToolScreen->show();
	hide();
}

void MainWindow::onSideBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASSideBeamLine);
	connect(motorToolScreen, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	motorToolScreen->move(QApplication::desktop()->screen()->rect().center() - motorToolScreen->rect().center());

	motorToolScreen->show();
	hide();
}

void MainWindow::onImagingBLMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen(BioXASBeamlineDef::BioXASImagingBeamLine);
	connect(motorToolScreen, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	motorToolScreen->move(QApplication::desktop()->screen()->rect().center() - motorToolScreen->rect().center());

	motorToolScreen->show();
	hide();
}

void MainWindow::onToolWindowClosed() {
	show();
}

void MainWindow::setupUi()
{
	QGroupBox *mainWidget = new QGroupBox();
	setCentralWidget(mainWidget);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainWidget->setLayout(mainLayout);

	QPushButton *shutterToolButton = new QPushButton("Shutter Tool");
	QPushButton *mainBLMotorToolButton = new QPushButton("Main Beamline Motor Tool");
	QPushButton *sideBLMotorToolButton = new QPushButton("Side Beamline Motor Tool");
	QPushButton *imagingBLMotorToolButton = new QPushButton("Imaging Beamline Motor Tool");

	mainLayout->addWidget(shutterToolButton);
	mainLayout->addWidget(mainBLMotorToolButton);
	mainLayout->addWidget(sideBLMotorToolButton);
	mainLayout->addWidget(imagingBLMotorToolButton);

	connect( shutterToolButton, SIGNAL(clicked()), this, SLOT(onShutterToolButtonClicked()) );
	connect( mainBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onMainBLMotorToolButtonClicked()) );
	connect( sideBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onSideBLMotorToolButtonClicked()) );
	connect( imagingBLMotorToolButton, SIGNAL(clicked()), this, SLOT(onImagingBLMotorToolButtonClicked()) );
}
