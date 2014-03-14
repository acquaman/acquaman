#include "MainWindow.h"
#include <QGroupBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    model_ = 0;
    mainLayout_ = new QVBoxLayout();

    appStatusLabel_ = 0;
    enableButton_ = 0;
    enableButton_ = 0;

    shuttersStatus_ = 0;
    openButton_ = 0;
    closeButton_ = 0;

    createComponents();
    makeConnections();

    QGroupBox *mainWidget = new QGroupBox();
    mainWidget->setLayout(mainLayout_);

    setWindowTitle("Shutter Tool");
    setCentralWidget(mainWidget);

    turnAutomaticShutterControlOff();
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



void MainWindow::onModelConnected(bool isConnected)
{
    if (isConnected) {
        enableButton_->setEnabled(true);
        turnAutomaticShutterControlOn();

    } else {
        enableButton_->setEnabled(false);
        turnAutomaticShutterControlOff();
    }
}



void MainWindow::onModelOpened()
{
    shuttersStatus_->setText("Shutters are open.");
}



void MainWindow::onModelClosed()
{
    shuttersStatus_->setText("Shutters are closed.");
}



void MainWindow::onModelBetween()
{
    shuttersStatus_->setText("Shutters are between.");
}



void MainWindow::createComponents()
{
    model_ = new Model(this);

    // create gui -- allow user to turn on and off automatic shutter control
    QWidget *appStatusDisplay = createAppStatusDisplay();

    // create gui -- display current shutter status, with buttons to open/close
    QWidget *shutterDisplay = createShutterDisplay();

    mainLayout_->addWidget(appStatusDisplay);
    mainLayout_->addWidget(shutterDisplay);
}



QWidget* MainWindow::createAppStatusDisplay()
{
    appStatusLabel_ = new QLabel();
    enableButton_ = new QPushButton("Enable");
    disableButton_ = new QPushButton("Disable");

    QHBoxLayout *onOffButtons = new QHBoxLayout();
    onOffButtons->addWidget(enableButton_);
    onOffButtons->addWidget(disableButton_);

    QVBoxLayout *appStatusLayout = new QVBoxLayout();
    appStatusLayout->addWidget(appStatusLabel_);
    appStatusLayout->addLayout(onOffButtons);

    QGroupBox *appStatusDisplay = new QGroupBox();
    appStatusDisplay->setLayout(appStatusLayout);

    return appStatusDisplay;
}



QWidget* MainWindow::createShutterDisplay()
{
    shuttersStatus_ = new QLabel("---");

    QVBoxLayout *statusDisplayLayout = new QVBoxLayout();
    statusDisplayLayout->addWidget(shuttersStatus_);

    openButton_ = new QPushButton("Open");
    closeButton_ = new QPushButton("Close");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(openButton_);
    buttonLayout->addWidget(closeButton_);

    QVBoxLayout *displayLayout = new QVBoxLayout();
    displayLayout->addLayout(statusDisplayLayout);
    displayLayout->addLayout(buttonLayout);

    QGroupBox *shutterDisplay = new QGroupBox();
    shutterDisplay->setLayout(displayLayout);

    return shutterDisplay;
}



void MainWindow::makeConnections()
{
    connect( enableButton_, SIGNAL(clicked()), this, SLOT(onEnableButtonClicked()) );
    connect( disableButton_, SIGNAL(clicked()), this, SLOT(onDisableButtonClicked()) );

    connect( openButton_, SIGNAL(clicked()), this, SLOT(onOpenButtonClicked()) );
    connect( closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()) );

    connect( model(), SIGNAL(connected(bool)), this, SLOT(onModelConnected(bool)) );
    connect( model(), SIGNAL(shuttersOpen()), this, SLOT(onModelOpened()) );
    connect( model(), SIGNAL(shuttersClosed()), this, SLOT(onModelClosed()) );
    connect( model(), SIGNAL(shuttersBetween()), this, SLOT(onModelBetween()) );
}



void MainWindow::turnAutomaticShutterControlOn()
{
    model()->setAutomaticControl(true);

    appStatusLabel_->setText("Automatic shutter control is enabled.");

    enableButton_->setEnabled(true);
    openButton_->setEnabled(true);
    closeButton_->setEnabled(true);
}



void MainWindow::turnAutomaticShutterControlOff()
{
    model()->setAutomaticControl(false);

    appStatusLabel_->setText("Automatic shutter control is disabled.");

    openButton_->setEnabled(false);
    closeButton_->setEnabled(false);
}
