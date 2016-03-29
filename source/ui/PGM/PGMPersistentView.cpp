#include "PGMPersistentView.h"

#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QProgressBar>
#include <QGridLayout>
#include <QInputDialog>



PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
    // ToDo: stripTool_ = new PGMScalerStripTool(this);

    beamStatusLabel_ = new QLabel("Beam is off.");

    beamOnButton_ = new QPushButton("Beam On");
    connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnClicked()));

    beamOffButton_ = new QPushButton("Beam Off");
    connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffClicked()));

    // ToDo:: connect PGMBeamline signal overallShutterStatus to onShutterStatusChange

    calibrateButton_ = new QPushButton("Calibrate Energy");
    connect(calibrateButton_, SIGNAL(clicked()), this, SLOT(onCalibrateClicked()));

    ringCurrent_ = new QLabel("     mA");
    ringCurrent_->setAlignment(Qt::AlignCenter);
    ringCurrent_->setFont(QFont("Lucida Grande", 16, QFont::Bold));

    energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->monoEnergyControl());
    energyControlEditor_->setMaximumWidth(200);
    energyControlEditor_->setControlFormat('f', 1);

    monoCrystal_ = new QLabel("Crystal");
    monoCrystal_->setAlignment(Qt::AlignCenter);
    monoEnergyRange_ = new QLabel(" ev - eV");
    monoEnergyRange_->setAlignment(Qt::AlignCenter);

    // Connects exposed ring current value to persistant view UI
    connect(PGMBeamline::bl()->exposedControlByName("ringCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));

    // Connects AMControl values from beamline instance to persistant view UI
    connect(PGMBeamline::pgm()->monoLowEV(),   SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
    connect(PGMBeamline::pgm()->monoHighEV(),  SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
    connect(PGMBeamline::pgm()->monoCrystal(), SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));

    connect(PGMBeamline::pgm()->monoLowEV(),   SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
    connect(PGMBeamline::pgm()->monoHighEV(),  SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
    connect(PGMBeamline::pgm()->monoCrystal(), SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));

    connect(scalerContinuousButton_, SIGNAL(clicked()), this, SLOT(onScalerContinuousButtonClicked()));

    // Layout for changing beam status
    QHBoxLayout *beamChangeLayout = new QHBoxLayout;
    beamChangeLayout->addWidget(beamOnButton_);
    beamChangeLayout->addWidget(beamOffButton_);
    // Layout for energy control editor
    QHBoxLayout *monoEnergyLayout = new QHBoxLayout;
    monoEnergyLayout->addWidget(energyControlEditor_);
    monoEnergyLayout->addWidget(calibrateButton_);
    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addWidget(ringCurrent_);
    mainPanelLayout->addLayout(beamChangeLayout);
    mainPanelLayout->addWidget(beamStatusLabel_, 0, Qt::AlignCenter);
    mainPanelLayout->addLayout(monoEnergyLayout);
    mainPanelLayout->addWidget(monoCrystal_);
    mainPanelLayout->addWidget(monoEnergyRange_);
    //ToDo: mainPanelLayout->addWidget(stripTool_);

    //ToDo: scaler panel layout & groupbox if needed

    // Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
    // the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.
    QGroupBox *persistentPanel = new QGroupBox("VLS-PGM Beamline");
    persistentPanel->setLayout(mainPanelLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(persistentPanel);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMaximumHeight(1000);
    setMinimumHeight(800);
    setMaximumWidth(400);
    setMinimumWidth(400);

    setLayout(mainLayout);
}

void PGMPersistentView::onBeamOnClicked()
{
    AMAction3 *beamOnAction; //ToDo: create PGMBeamline::pgm()->createBeamOnAction();
    beamOnAction->start();
}

void PGMPersistentView::onBeamOffClicked()
{
    AMAction3 *beamOffAction; //ToDo: create PGMBeamline::pgm()->createBeamOffAction();
    beamOffAction->start();

}

// Disables or enables the beam on and off button depending on current state of beam
void PGMPersistentView::onShutterStatusChanged(bool state)
{
    beamOnButton_->setDisabled(state);
    beamOffButton_->setEnabled(state);
    beamStatusLabel_->setText(state ? "Beam is on." : "Beam is off.");

}

void PGMPersistentView::onCrystalChanged()
{

    monoCrystal_->setText(PGMBeamline::bl()->exposedControlByName("monoCrystal")->
                          enumNameAt(PGMBeamline::bl()->exposedControlByName("monoCrystal")->value()));
    monoEnergyRange_->setText(QString("%1 eV - %2 eV").arg(PGMBeamline::pgm()->monoLowEV()->value()).arg(
                                  PGMBeamline::pgm()->monoHighEV()->value()));
}

void PGMPersistentView::onRingCurrentChanged(double current)
{
    ringCurrent_->setText(QString("%1 mA").arg(current));
    if(current < 10)
        ringCurrent_->setStyleSheet("QLabel { background-color : red; color : white }");
    else
        ringCurrent_->setStyleSheet("QLabel { color : black }");
}

void PGMPersistentView::onCalibrateClicked()
{
    //Create a input dialog box requesting user input calibration energy. Value confirmation is stored as bool used for logic
    bool inputDialogConfirmPressed;
    double newEnergyValue = QInputDialog::getDouble(this,"Monochromator Energy Calibration","Enter Current Calibrated Energy:",
                                                    PGMBeamline::pgm()->monoEnergyControl()->value(),
                                                    PGMBeamline::pgm()->monoLowEV()->value(),
                                                    PGMBeamline::pgm()->monoHighEV()->value(),
                                                    1,&inputDialogConfirmPressed);

    if(inputDialogConfirmPressed)
    {
        double oldAngleOffset = PGMBeamline::pgm()->monoAngleOffset()->value();
        double currentEnergyValue = PGMBeamline::pgm()->monoEnergyControl()->value();
        double mono2d = PGMBeamline::pgm()->mono2d()->value();
        double braggAngle = PGMBeamline::pgm()->monoBraggAngle()->value();
        //ToDo: confirm this number, may be beamline dept.
        double braggAngleTheta = 12398.4193;
        double deltaEnergy = newEnergyValue - currentEnergyValue;

        double angleDelta = -braggAngleTheta / (mono2d * currentEnergyValue * currentEnergyValue * cos(braggAngle * M_PI / 180)) * deltaEnergy * 180 / M_PI;

         PGMBeamline::pgm()->monoAngleOffset()->move(oldAngleOffset + angleDelta);

    }
}

void PGMPersistentView::onScalerContinuousButtonClicked()
{
    // ToDo: requires PGMBeamline::pgm()->scaler()

}










