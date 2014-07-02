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
#include <QGroupBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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



MainWindow::~MainWindow()
{

}



Model* MainWindow::model()
{
    return model_;
}



void MainWindow::onEnableButtonClicked()
{
    qDebug() << "MainWindow :: enable button clicked.";
    turnAutomaticShutterControlOn();
}



void MainWindow::onDisableButtonClicked()
{
    turnAutomaticShutterControlOff();
}



void MainWindow::onOpenButtonClicked()
{
    model()->openShutters();
}



void MainWindow::onCloseButtonClicked()
{
    model()->closeShutters();
}



void MainWindow::onModelOpened()
{
    shuttersStatusLabel_->setText("Shutters are open.");
}



void MainWindow::onModelClosed()
{
    shuttersStatusLabel_->setText("Shutters are closed.");
}



void MainWindow::onModelBetween()
{
    shuttersStatusLabel_->setText("Shutters are between.");
}



void MainWindow::enableAutomaticShuttersOpenGUI(bool enabled)
{
    enableAutomaticShuttersOpenButton_->setEnabled(enabled);

    if (enabled) {
        qDebug() << "MainWindow :: automatic shutters open gui is enabled.";

    } else {
        qDebug() << "MainWindow :: automatic shutters open gui is disabled.";
    }
}



void MainWindow::enableShutterControlGUI(bool enabled)
{
    openShuttersButton_->setEnabled(enabled);
    closeShuttersButton_->setEnabled(enabled);

    if (enabled)
        qDebug() << "MainWindow :: shutter control gui is enabled.";
    else
        qDebug() << "MainWindow :: shutter control gui is disabled.";
}



void MainWindow::onInjection()
{
    enableShutterControlGUI(false);
}



void MainWindow::onPostInjection()
{
    if (model()->shuttersConnected())
        enableShutterControlGUI(true);
    else
        enableShutterControlGUI(false);
}



void MainWindow::createComponents()
{
    model_ = new Model(this);
    mainLayout_ = new QVBoxLayout();

    // create gui -- allow user to turn on and off automatic shutter control
    QWidget *appStatusDisplay = createAppStatusDisplay();

    // create gui -- display current shutter status, with buttons to open/close
    QWidget *shutterDisplay = createShutterDisplay();

    mainLayout_->addWidget(appStatusDisplay);
    mainLayout_->addWidget(shutterDisplay);
}



void MainWindow::makeConnections()
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



void MainWindow::defaultSettings()
{
    QGroupBox *mainWidget = new QGroupBox();
    mainWidget->setLayout(mainLayout_);

    setWindowTitle("Shutter Tool");
    setCentralWidget(mainWidget);

    enableAutomaticShuttersOpenGUI(false);
    enableShutterControlGUI(false);

    turnAutomaticShutterControlOff();
}



QWidget* MainWindow::createAppStatusDisplay()
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



QWidget* MainWindow::createShutterDisplay()
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



void MainWindow::turnAutomaticShutterControlOn()
{
    model()->setAutomaticShuttersOpen(true);
    appStatusLabel_->setText("Automatic shutter control is enabled.");
}



void MainWindow::turnAutomaticShutterControlOff()
{
    model()->setAutomaticShuttersOpen(false);
    appStatusLabel_->setText("Automatic shutter control is disabled.");
}
