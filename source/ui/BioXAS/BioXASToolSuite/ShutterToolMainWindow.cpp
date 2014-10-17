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


#include "ShutterToolMainWindow.h"

#include <QGroupBox>
#include <QDebug>

ShutterToolMainWindow::ShutterToolMainWindow(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);

	model_ = 0;
	mainLayout_ = 0;

	appStatusLabel_ = 0;
	enableAutomaticShuttersOpenButton_ = 0;
	enableAutomaticShuttersOpenButton_ = 0;

	shuttersStatusLabel_ = 0;
	openShuttersButton_ = 0;
	closeShuttersButton_ = 0;

	createComponents();
	defaultSettings();
	makeConnections();
}



ShutterToolMainWindow::~ShutterToolMainWindow()
{

}



BioXASShutterToolController* ShutterToolMainWindow::model()
{
	return model_;
}



void ShutterToolMainWindow::onEnableButtonClicked()
{
	qDebug() << "ShutterToolMainWindow :: enable button clicked.";
	turnAutomaticShutterControlOn();
}



void ShutterToolMainWindow::onDisableButtonClicked()
{
	turnAutomaticShutterControlOff();
}



void ShutterToolMainWindow::onOpenButtonClicked()
{
	model()->openShutters();
}



void ShutterToolMainWindow::onCloseButtonClicked()
{
	model()->closeShutters();
}



void ShutterToolMainWindow::onModelOpened()
{
	shuttersStatusLabel_->setText("Shutters are open.");
}



void ShutterToolMainWindow::onModelClosed()
{
	shuttersStatusLabel_->setText("Shutters are closed.");
}



void ShutterToolMainWindow::onModelBetween()
{
	shuttersStatusLabel_->setText("Shutters are between.");
}



void ShutterToolMainWindow::enableAutomaticShuttersOpenGUI(bool enabled)
{
	enableAutomaticShuttersOpenButton_->setEnabled(enabled);

	if (enabled) {
		qDebug() << "ShutterToolMainWindow :: automatic shutters open gui is enabled.";

	} else {
		qDebug() << "ShutterToolMainWindow :: automatic shutters open gui is disabled.";
	}
}



void ShutterToolMainWindow::enableShutterControlGUI(bool enabled)
{
	openShuttersButton_->setEnabled(enabled);
	closeShuttersButton_->setEnabled(enabled);

	if (enabled)
		qDebug() << "ShutterToolMainWindow :: shutter control gui is enabled.";
	else
		qDebug() << "ShutterToolMainWindow :: shutter control gui is disabled.";
}



void ShutterToolMainWindow::onInjection()
{
	enableShutterControlGUI(false);
}



void ShutterToolMainWindow::onPostInjection()
{
	if (model()->shuttersConnected())
		enableShutterControlGUI(true);
	else
		enableShutterControlGUI(false);
}



void ShutterToolMainWindow::createComponents()
{
	model_ = new BioXASShutterToolController(this);
	mainLayout_ = new QVBoxLayout();

	// create gui -- allow user to turn on and off automatic shutter control
	QWidget *appStatusDisplay = createAppStatusDisplay();

	// create gui -- display current shutter status, with buttons to open/close
	QWidget *shutterDisplay = createShutterDisplay();

	mainLayout_->addWidget(appStatusDisplay);
	mainLayout_->addWidget(shutterDisplay);
}



void ShutterToolMainWindow::makeConnections()
{
	connect( model(), SIGNAL(aodShutterControlConnected(bool)), this, SLOT(enableAutomaticShuttersOpenGUI(bool)) );
	connect( model(), SIGNAL(shuttersConnected(bool)), this, SLOT(enableShutterControlGUI(bool)) );
	connect( model(), SIGNAL(preInjectionControlsDisabled()), this, SLOT(onInjection()) );
	connect( model(), SIGNAL(postInjectionControlsEnabled()), this, SLOT(onPostInjection()) );

	connect( enableAutomaticShuttersOpenButton_, SIGNAL(clicked()), this, SLOT(onEnableButtonClicked()) );
	connect( disableAutomaticShuttersOpenButton_, SIGNAL(clicked()), this, SLOT(onDisableButtonClicked()) );

	connect( openShuttersButton_, SIGNAL(clicked()), this, SLOT(onOpenButtonClicked()) );
	connect( closeShuttersButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()) );

	connect( model(), SIGNAL(shuttersOpen()), this, SLOT(onModelOpened()) );
	connect( model(), SIGNAL(shuttersClosed()), this, SLOT(onModelClosed()) );
	connect( model(), SIGNAL(shuttersBetween()), this, SLOT(onModelBetween()) );
}



void ShutterToolMainWindow::defaultSettings()
{
//	QGroupBox *mainWidget = new QGroupBox();
	this->setLayout(mainLayout_);

	setWindowTitle("BioXAS Shutter Tool");
//	setCentralWidget(mainWidget);

	enableAutomaticShuttersOpenGUI(false);
	enableShutterControlGUI(false);

	turnAutomaticShutterControlOff();
}



QWidget* ShutterToolMainWindow::createAppStatusDisplay()
{
	appStatusLabel_ = new QLabel();
	enableAutomaticShuttersOpenButton_ = new QPushButton("Enable");
	disableAutomaticShuttersOpenButton_ = new QPushButton("Disable");

	QHBoxLayout *onOffButtons = new QHBoxLayout();
	onOffButtons->addWidget(enableAutomaticShuttersOpenButton_);
	onOffButtons->addWidget(disableAutomaticShuttersOpenButton_);

	QVBoxLayout *appStatusLayout = new QVBoxLayout();
	appStatusLayout->addWidget(appStatusLabel_);
	appStatusLayout->addLayout(onOffButtons);

	QGroupBox *appStatusDisplay = new QGroupBox();
	appStatusDisplay->setLayout(appStatusLayout);

	return appStatusDisplay;
}



QWidget* ShutterToolMainWindow::createShutterDisplay()
{
	shuttersStatusLabel_ = new QLabel("Shutters are ---");

	QVBoxLayout *statusDisplayLayout = new QVBoxLayout();
	statusDisplayLayout->addWidget(shuttersStatusLabel_);

	openShuttersButton_ = new QPushButton("Open");
	closeShuttersButton_ = new QPushButton("Close");

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(openShuttersButton_);
	buttonLayout->addWidget(closeShuttersButton_);

	QVBoxLayout *displayLayout = new QVBoxLayout();
	displayLayout->addLayout(statusDisplayLayout);
	displayLayout->addLayout(buttonLayout);

	QGroupBox *shutterDisplay = new QGroupBox();
	shutterDisplay->setLayout(displayLayout);

	return shutterDisplay;
}



void ShutterToolMainWindow::turnAutomaticShutterControlOn()
{
	model()->setAutomaticShuttersOpen(true);
	appStatusLabel_->setText("Automatic shutter control is enabled.");
}



void ShutterToolMainWindow::turnAutomaticShutterControlOff()
{
	model()->setAutomaticShuttersOpen(false);
	appStatusLabel_->setText("Automatic shutter control is disabled.");
}
