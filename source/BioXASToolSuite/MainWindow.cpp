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

#include "shutterTool/ShutterToolMainWindow.h"
#include "motorTool/MotorToolMainScreen.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainLayout_ = 0;

	shutterToolButton_ = 0;
	motorToolButton_ = 0;

    createComponents();
	defaultSettings();
    makeConnections();
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
	hide();
}

void MainWindow::onMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolMainScreen = new MotorToolMainScreen();
	connect(motorToolMainScreen, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	motorToolMainScreen->move(QApplication::desktop()->screen()->rect().center() - motorToolMainScreen->rect().center());

	motorToolMainScreen->show();
	hide();
}

void MainWindow::onToolWindowClosed() {
	show();
}

void MainWindow::createComponents()
{
    mainLayout_ = new QVBoxLayout();

	shutterToolButton_ = new QPushButton("Shutter Tool");
	motorToolButton_ = new QPushButton("Motor Tool");

	mainLayout_->addWidget(shutterToolButton_);
	mainLayout_->addWidget(motorToolButton_);
}

void MainWindow::makeConnections()
{
	connect( shutterToolButton_, SIGNAL(clicked()), this, SLOT(onShutterToolButtonClicked()) );
	connect( motorToolButton_, SIGNAL(clicked()), this, SLOT(onMotorToolButtonClicked()) );
}

void MainWindow::defaultSettings()
{
    QGroupBox *mainWidget = new QGroupBox();
    mainWidget->setLayout(mainLayout_);

	setWindowTitle("BioXAS Beamline Toolsuite");
    setCentralWidget(mainWidget);
}
