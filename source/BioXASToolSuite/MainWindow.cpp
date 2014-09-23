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

#include "shutterTool/ShutterToolMainWindow.h"
#include "motorTool/ui/MotorToolMainScreen.h"

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

void MainWindow::onMotorToolButtonClicked()
{
	MotorToolMainScreen *motorToolScreen = new MotorToolMainScreen();
	connect(motorToolScreen, SIGNAL(closed()), this, SLOT(onToolWindowClosed()));

	motorToolScreen->move(QApplication::desktop()->screen()->rect().center() - motorToolScreen->rect().center());
//	motorToolScreen->setFixedSize(motorToolScreen->rect().width(),motorToolScreen->rect().height());

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
	QPushButton *motorToolButton = new QPushButton("Motor Tool");

	mainLayout->addWidget(shutterToolButton);
	mainLayout->addWidget(motorToolButton);

	connect( shutterToolButton, SIGNAL(clicked()), this, SLOT(onShutterToolButtonClicked()) );
	connect( motorToolButton, SIGNAL(clicked()), this, SLOT(onMotorToolButtonClicked()) );
}
