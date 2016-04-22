#include "PGMHVControlViewBranchB.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

PGMHVControlViewBranchB::PGMHVControlViewBranchB(QWidget *parent) :
    QWidget(parent)
{

    // Once PGMHVControl is created, these values will come from them and won't
    // be needed from this class.
    i0BeamlineOn = false;
    i0EndstationOn = false;
    teyOn = false;
    fl1On = false;
    fl2On = false;
    fl3On = false;

    positiveHVLabel_ = new QLabel("Positive HV");
    positiveHVLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    negativeHVLabel_ = new QLabel("Negative HV");
    negativeHVLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    i0BeamlineBiasLabel_ = new QLabel("I0 Beamline Bias:");
    i0EndstationBiasLabel_ = new QLabel("I0 Endstation Bias:");
    teyBiasLabel_ = new QLabel("TEY Bias:");

    deviceLabel_ = new QLabel("Device");
    deviceLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    deviceLabel_->setAlignment(Qt::AlignCenter);
    setpointLabel_ = new QLabel("Setpoint (V)");
    setpointLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    setpointLabel_->setAlignment(Qt::AlignCenter);
    onOffButtonLabel_ = new QLabel("On/Off");
    onOffButtonLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    statusIconLabel_ = new QLabel("Status");
    statusIconLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    statusIconLabel_->setAlignment(Qt::AlignCenter);
    actualVoltageLabel_ = new QLabel("Voltage");
    actualVoltageLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    actualVoltageLabel_->setAlignment(Qt::AlignCenter);

    flDetector1Label_ = new QLabel("FL Detector 1");
    flDetector2Label_ = new QLabel("FL Detector 2");
    flDetector3Label_ = new QLabel("FL Detector 3");

    i0BeamlineBiasValue_ = new QLabel("  V");
    i0BeamlineBiasValue_->setAlignment(Qt::AlignCenter);
    i0EndstationBiasValue_ = new QLabel("  V");
    i0EndstationBiasValue_->setAlignment(Qt::AlignCenter);
    teyBiasValue_ = new QLabel("  V");
    teyBiasValue_->setAlignment(Qt::AlignCenter);
    flDetector1Value_ = new QLabel("  V");
    flDetector1Value_->setAlignment(Qt::AlignCenter);
    flDetector2Value_ = new QLabel("  V");
    flDetector2Value_->setAlignment(Qt::AlignCenter);
    flDetector3Value_ = new QLabel("  V");
    flDetector3Value_->setAlignment(Qt::AlignCenter);

    i0BeamlineSetpoint_ = new QLineEdit();
    i0EndstationSetpoint_ = new QLineEdit();
    teySetpoint_ = new QLineEdit();
    flDetector1Setpoint_ = new QLineEdit();
    flDetector2Setpoint_ = new QLineEdit();
    flDetector3Setpoint_ = new QLineEdit();

    i0BeamlineVoltageOnOffButton_ = new QPushButton();
    i0BeamlineVoltageOnOffButton_->setText("On");
    i0EndstationVoltageOnOffButton_ = new QPushButton();
    i0EndstationVoltageOnOffButton_->setText("On");
    teyOnOffButton_ = new QPushButton();
    teyOnOffButton_->setText("On");
    flDetector1OnOffButton_ = new QPushButton();
    flDetector1OnOffButton_->setText("On");
    flDetector2OnOffButton_ = new QPushButton();
    flDetector2OnOffButton_->setText("On");
    flDetector3OnOffButton_ = new QPushButton();
    flDetector3OnOffButton_->setText("On");

    i0BeamlineLightLabel_ = new QLabel();
    i0BeamlineLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    i0BeamlineLightLabel_->setAlignment(Qt::AlignCenter);

    i0EndstationLightLabel_ = new QLabel();
    i0EndstationLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    i0EndstationLightLabel_->setAlignment(Qt::AlignCenter);

    teyLightLabel_ = new QLabel();
    teyLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    teyLightLabel_->setAlignment(Qt::AlignCenter);

    flDetector1LightLabel_ = new QLabel();
    flDetector1LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    flDetector1LightLabel_->setAlignment(Qt::AlignCenter);

    flDetector2LightLabel_ = new QLabel();
    flDetector2LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    flDetector2LightLabel_->setAlignment(Qt::AlignCenter);

    flDetector3LightLabel_ = new QLabel();
    flDetector3LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
    flDetector3LightLabel_->setAlignment(Qt::AlignCenter);

    mainVLayout_ = new QVBoxLayout;
    mainHLayout_ = new QHBoxLayout;


    mainGridLayout_ = new QGridLayout;


    // Positive HV Header
    mainGridLayout_->addWidget(positiveHVLabel_,     0, 1, 1, 1);

    // Negative HV Header
    mainGridLayout_->addWidget(negativeHVLabel_,     5, 1, 1, 1);

    // Headers
    mainGridLayout_->addWidget(deviceLabel_,         1, 2, 1, 1);
    mainGridLayout_->addWidget(setpointLabel_,       1, 3, 1, 1);
    mainGridLayout_->addWidget(statusIconLabel_,     1, 5, 1, 1);
    mainGridLayout_->addWidget(actualVoltageLabel_,  1, 6, 1, 1);

    // Positive HV Devices
    mainGridLayout_->addWidget(i0BeamlineBiasLabel_,   2, 2, 1, 1);
    mainGridLayout_->addWidget(i0EndstationBiasLabel_, 3, 2, 1, 1);
    mainGridLayout_->addWidget(teyBiasLabel_,          4, 2, 1, 1);

    mainGridLayout_->addWidget(i0BeamlineSetpoint_,    2, 3, 1, 1);
    mainGridLayout_->addWidget(i0EndstationSetpoint_,  3, 3, 1, 1);
    mainGridLayout_->addWidget(teySetpoint_,           4, 3, 1, 1);

    mainGridLayout_->addWidget(i0BeamlineVoltageOnOffButton_,   2, 4, 1, 1);
    mainGridLayout_->addWidget(i0EndstationVoltageOnOffButton_, 3, 4, 1, 1);
    mainGridLayout_->addWidget(teyOnOffButton_,                 4, 4, 1, 1);

    mainGridLayout_->addWidget(i0BeamlineLightLabel_,   2, 5, 1, 1);
    mainGridLayout_->addWidget(i0EndstationLightLabel_, 3, 5, 1, 1);
    mainGridLayout_->addWidget(teyLightLabel_,          4, 5, 1, 1);

    mainGridLayout_->addWidget(i0BeamlineBiasValue_,   2, 6, 1, 1);
    mainGridLayout_->addWidget(i0EndstationBiasValue_, 3, 6, 1, 1);
    mainGridLayout_->addWidget(teyBiasValue_,          4, 6, 1, 1);

    // Negative HV Devices Labels
    mainGridLayout_->addWidget(flDetector1Label_,      6, 2, 1, 1);
    mainGridLayout_->addWidget(flDetector2Label_,      7, 2, 1, 1);
    mainGridLayout_->addWidget(flDetector3Label_,      8, 2, 1, 1);

    mainGridLayout_->addWidget(flDetector1Setpoint_,   6, 3, 1, 1);
    mainGridLayout_->addWidget(flDetector2Setpoint_,   7, 3, 1, 1);
    mainGridLayout_->addWidget(flDetector3Setpoint_,   8, 3, 1, 1);

    mainGridLayout_->addWidget(flDetector1OnOffButton_,6, 4, 1, 1);
    mainGridLayout_->addWidget(flDetector2OnOffButton_,7, 4, 1, 1);
    mainGridLayout_->addWidget(flDetector3OnOffButton_,8, 4, 1, 1);

    mainGridLayout_->addWidget(flDetector1LightLabel_, 6, 5, 1, 1);
    mainGridLayout_->addWidget(flDetector2LightLabel_, 7, 5, 1, 1);
    mainGridLayout_->addWidget(flDetector3LightLabel_, 8, 5, 1, 1);

    mainGridLayout_->addWidget(flDetector1Value_, 6, 6, 1, 1);
    mainGridLayout_->addWidget(flDetector2Value_, 7, 6, 1, 1);
    mainGridLayout_->addWidget(flDetector3Value_, 8, 6, 1, 1);


    // Create a labeled group box then place main grid within
    // the horizontal and vertical layout with spacing on the edges.
    branchBHVbox_ = new QGroupBox("High Voltage Branch B");
    branchBHVbox_->setLayout(mainGridLayout_);

    mainVLayout_->addStretch();
    mainHLayout_->addStretch();
    mainHLayout_->addWidget(branchBHVbox_);
    mainHLayout_->addStretch();
    mainVLayout_->addLayout(mainHLayout_);
    mainVLayout_->addStretch();

    setLayout(mainVLayout_);

    connect(i0BeamlineVoltageOnOffButton_, SIGNAL(clicked()), this, SLOT(oni0BeamlineVoltageButtonClicked()));
    connect(i0EndstationVoltageOnOffButton_, SIGNAL(clicked()), this, SLOT(oni0EndstationVoltageButtonClicked()));
    connect(teyOnOffButton_, SIGNAL(clicked()), this, SLOT(onTEYVoltageButtonClicked()));
    connect(flDetector1OnOffButton_, SIGNAL(clicked()), this, SLOT(onFL1VoltageButtonClicked()));
    connect(flDetector2OnOffButton_, SIGNAL(clicked()), this, SLOT(onFL2VoltageButtonClicked()));
    connect(flDetector3OnOffButton_, SIGNAL(clicked()), this, SLOT(onFL3VoltageButtonClicked()));

}

void PGMHVControlViewBranchB::oni0BeamlineVoltageButtonClicked(){

    i0BeamlineOn = !i0BeamlineOn;

    if(i0BeamlineOn) {
        i0BeamlineLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        i0BeamlineVoltageOnOffButton_->setText("Off");
    }
    else {
        i0BeamlineLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        i0BeamlineVoltageOnOffButton_->setText("On");
    }
}

void PGMHVControlViewBranchB::oni0EndstationVoltageButtonClicked(){

    i0EndstationOn = !i0EndstationOn;

    if(i0EndstationOn) {
        i0EndstationLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        i0EndstationVoltageOnOffButton_->setText("Off");
    }
    else {
        i0EndstationLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        i0EndstationVoltageOnOffButton_->setText("On");
    }
}

void PGMHVControlViewBranchB::onTEYVoltageButtonClicked(){

    teyOn = !teyOn;

    if(teyOn) {
        teyLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        teyOnOffButton_->setText("Off");
    }
    else {
        teyLightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        teyOnOffButton_->setText("On");
    }
}

void PGMHVControlViewBranchB::onFL1VoltageButtonClicked(){

    fl1On = !fl1On;

    if(fl1On) {
        flDetector1LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        flDetector1OnOffButton_->setText("Off");
    }
    else {
        flDetector1LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        flDetector1OnOffButton_->setText("On");
    }
}

void PGMHVControlViewBranchB::onFL2VoltageButtonClicked(){

    fl2On = !fl2On;

    if(fl2On) {
        flDetector2LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        flDetector2OnOffButton_->setText("Off");
    }
    else {
        flDetector2LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        flDetector2OnOffButton_->setText("On");
    }
}

void PGMHVControlViewBranchB::onFL3VoltageButtonClicked(){

    fl3On = !fl3On;

    if(fl3On) {
        flDetector3LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
        flDetector3OnOffButton_->setText("Off");
    }
    else {
        flDetector3LightLabel_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
        flDetector3OnOffButton_->setText("On");
    }
}



