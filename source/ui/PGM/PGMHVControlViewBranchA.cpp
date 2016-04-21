#include "PGMHVControlViewBranchA.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

PGMHVControlViewBranchA::PGMHVControlViewBranchA(QWidget *parent) :
    QWidget(parent)
{

    positiveHVLabel_ = new QLabel("Positive HV");
    negativeHVLabel_ = new QLabel("Negative HV");
    i0BeamlineBiasLabel_ = new QLabel("I0 Beamline Bias:");
    i0EndstationBiasLabel_ = new QLabel("I0 Endstation Bias:");
    teyBiasLabel_ = new QLabel("TEY Bias:");

    deviceLabel_ = new QLabel("Device");
    deviceLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    setpointLabel_ = new QLabel("Setpoint");
    setpointLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    onOffLabel_ = new QLabel("On/Off");
    onOffLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));
    actualVoltageLabel_ = new QLabel("Actual Voltage");
    actualVoltageLabel_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    flDetector1Label_ = new QLabel("FL Detector 1");
    flDetector2Label_ = new QLabel("FL Detector 2");
    flDetector3Label_ = new QLabel("FL Detector 3");

    i0BeamlineBiasValue_ = new QLabel("   V");
    i0EndstationBiasValue_ = new QLabel("  V");
    teyBiasValue_ = new QLabel("  V");
    flDetector1Value_ = new QLabel("  V");
    flDetector2Value_ = new QLabel("  V");
    flDetector3Value_ = new QLabel("  V");

    i0BeamlineSetpoint_ = new QLineEdit("  V");
    i0EndstationSetpoint_ = new QLineEdit("  V");
    teySetpoint_ = new QLineEdit("  V");
    flDetector1Setpoint_ = new QLineEdit("  V");
    flDetector2Setpoint_ = new QLineEdit("  V");
    flDetector3Setpoint_ = new QLineEdit("  V");

    i0BeamlineVoltageOnOffButton_ = new QPushButton();
    i0EndstationVoltageOnOffButton_ = new QPushButton();
    teyOnOffButton_ = new QPushButton();
    flDetector1OnOffButton_ = new QPushButton();
    flDetector2OnOffButton_ = new QPushButton();
    flDetector3OnOffButton_ = new QPushButton();


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
    mainGridLayout_->addWidget(onOffLabel_,          1, 4, 1, 1);
    mainGridLayout_->addWidget(actualVoltageLabel_,  1, 5, 1, 1);

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

    mainGridLayout_->addWidget(i0BeamlineBiasValue_,   2, 5, 1, 1);
    mainGridLayout_->addWidget(i0EndstationBiasValue_, 3, 5, 1, 1);
    mainGridLayout_->addWidget(teyBiasValue_,          4, 5, 1, 1);

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

    mainGridLayout_->addWidget(flDetector1Value_, 6, 5, 1, 1);
    mainGridLayout_->addWidget(flDetector2Value_, 7, 5, 1, 1);
    mainGridLayout_->addWidget(flDetector3Value_, 8, 5, 1, 1);


    // Create a labeled group box then place main grid within
    // the horizontal and vertical layout with spacing on the edges.
    QGroupBox *voltageBox = new QGroupBox("High Voltage Branch A");
    voltageBox->setLayout(mainGridLayout_);

    mainVLayout_->addStretch();
    mainHLayout_->addStretch();
    mainHLayout_->addWidget(voltageBox);
    mainHLayout_->addStretch();
    mainVLayout_->addLayout(mainHLayout_);
    mainVLayout_->addStretch();

    setLayout(mainVLayout_);

}
