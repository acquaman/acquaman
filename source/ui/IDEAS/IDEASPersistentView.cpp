#include "IDEASPersistentView.h"

#include "beamline/IDEAS/IDEASBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QGridLayout>

IDEASPersistentView::IDEASPersistentView(QWidget *parent) :
    QWidget(parent)
{
    beamStatusLabel_ = new QLabel("Beam is off!");

    beamOnButton_ = new QPushButton("Beam On");
    connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnClicked()));

    beamOffButton_ = new QPushButton("Beam Off");
    connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffClicked()));

    connect(IDEASBeamline::ideas(), SIGNAL(overallShutterStatus(bool)), this, SLOT(onShutterStatusChanged(bool)));

    ringCurrent_ = new QLabel("     mA");
    ringCurrent_->setAlignment(Qt::AlignCenter);

    energyControlEditor_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->monoEnergyControl());
    energyControlEditor_->setMaximumWidth(200);
    energyControlEditor_->setControlFormat('f', 1);

    monoCrystal_ = new QLabel("Crystal");
    monoCrystal_->setAlignment(Qt::AlignCenter);
    monoEnergyRange_ = new QLabel(" eV - eV");
    monoEnergyRange_->setAlignment(Qt::AlignCenter);

    IOldLabel_ = new QLabel("Old");
    I0Label_ = new QLabel("I_0");
    ISampleLabel_ = new QLabel("Sample");
    IReferenceLabel_ = new QLabel("Reference");

    IOldValueLabel_ = new QLabel("0");
    I0ValueLabel_ = new QLabel("0");
    ISampleValueLabel_ = new QLabel("0");
    IReferenceValueLabel_ = new QLabel("0");

    IOldBar_ = new QProgressBar;
    IOldBar_->setTextVisible(false);
    IOldBar_->setRange(100,145);


    I0Bar_ = new QProgressBar;
    I0Bar_->setTextVisible(false);
    I0Bar_->setRange(100,145);

    ISampleBar_ = new QProgressBar;
    ISampleBar_->setTextVisible(false);
    ISampleBar_->setRange(100,145);

    IReferenceBar_ = new QProgressBar;
    IReferenceBar_->setTextVisible(false);
    IReferenceBar_->setRange(100,145);

    connect(IDEASBeamline::bl()->exposedDetectorByName("I_vac"), SIGNAL(newValuesAvailable()), this, SLOT(onOldCountsChanged()));
    connect(IDEASBeamline::bl()->exposedDetectorByName("I_0"), SIGNAL(newValuesAvailable()), this, SLOT(onI0CountsChanged()));
    connect(IDEASBeamline::bl()->exposedDetectorByName("I_sample"), SIGNAL(newValuesAvailable()), this, SLOT(onSampleCountsChanged()));
    connect(IDEASBeamline::bl()->exposedDetectorByName("I_ref"), SIGNAL(newValuesAvailable()), this, SLOT(onReferenceCountsChanged()));
    connect(IDEASBeamline::bl()->exposedControlByName("monoCrystal"), SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged(double)));
    connect(IDEASBeamline::bl()->exposedControlByName("ringCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));




    QHBoxLayout *beamChangeLayout = new QHBoxLayout;
    beamChangeLayout->addWidget(beamOnButton_);
    beamChangeLayout->addWidget(beamOffButton_);

    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addWidget(ringCurrent_);
    mainPanelLayout->addLayout(beamChangeLayout);
    mainPanelLayout->addWidget(beamStatusLabel_, 0, Qt::AlignCenter);
    mainPanelLayout->addWidget(energyControlEditor_);
    mainPanelLayout->addWidget(monoCrystal_);
    mainPanelLayout->addWidget(monoEnergyRange_);
    mainPanelLayout->addStretch();

    QGridLayout *detectorPanelLayout = new QGridLayout;
    detectorPanelLayout->addWidget(IOldLabel_,0,0);
    detectorPanelLayout->addWidget(IOldBar_,0,1,1,2);
    detectorPanelLayout->addWidget(IOldValueLabel_,0,3,1,2);
    detectorPanelLayout->addWidget(I0Label_,1,0);
    detectorPanelLayout->addWidget(I0Bar_,1,1,1,2);
    detectorPanelLayout->addWidget(I0ValueLabel_,1,3,1,2);
    detectorPanelLayout->addWidget(ISampleLabel_,2,0);
    detectorPanelLayout->addWidget(ISampleBar_,2,1,1,2);
    detectorPanelLayout->addWidget(ISampleValueLabel_,2,3,1,2);
    detectorPanelLayout->addWidget(IReferenceLabel_,3,0);
    detectorPanelLayout->addWidget(IReferenceBar_,3,1,1,2);
    detectorPanelLayout->addWidget(IReferenceValueLabel_,3,3,1,2);


    QGroupBox *persistentPanel = new QGroupBox("IDEAS Beamline");
    persistentPanel->setLayout(mainPanelLayout);

    QGroupBox *detectorPanel = new QGroupBox("Ion Chamber Currents");
    detectorPanel->setLayout(detectorPanelLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(persistentPanel);
    layout->addWidget(detectorPanel);

    setLayout(layout);

    setMaximumWidth(250);
}

void IDEASPersistentView::onBeamOnClicked()
{
    AMAction3 *beamOn = IDEASBeamline::ideas()->createBeamOnAction();
    beamOn->start();
}

void IDEASPersistentView::onBeamOffClicked()
{
    AMAction3 *beamOff = IDEASBeamline::ideas()->createBeamOffAction();
    beamOff->start();
}

void IDEASPersistentView::onShutterStatusChanged(bool state)
{
    beamOnButton_->setDisabled(state);
    beamOffButton_->setEnabled(state);
    beamStatusLabel_->setText(state ? "Beam is on!" : "Beam is off!");
}


void IDEASPersistentView::onOldCountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_vac")->data(&value);
    IOldBar_->setValue(int(200 + 10*log10(qAbs(value))));
    //qDebug() << "I_Old_bar" << int(200 + 10*log10(qAbs(value)));
    //qDebug() << "I_Old" << value << "log10(I_Old)" << log10(value) << log10(qAbs(value));
    IOldValueLabel_->setText(QString::number(value, 'e', 2));
}

void IDEASPersistentView::onI0CountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_0")->data(&value);
    I0Bar_->setValue(int(200 + 10*log10(qAbs(value))));
    //qDebug() << "I_0" << int(-3.25/log10(qAbs(value)));
    I0ValueLabel_->setText(QString::number(value, 'e', 2));

   // ui->signalI0Bar->setValue(int(counts*600./1.e6));
    //ui->signalI0Value->setText(QString::number(counts, 'e', 2));
}

void IDEASPersistentView::onSampleCountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_sample")->data(&value);
    ISampleBar_->setValue(int(200 + 10*log10(qAbs(value))));
    ISampleValueLabel_->setText(QString::number(value, 'e', 2));
}

void IDEASPersistentView::onReferenceCountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_ref")->data(&value);
    IReferenceBar_->setValue(int(200 + 10*log10(qAbs(value))));
    IReferenceValueLabel_->setText(QString::number(value, 'e', 2));
}

void IDEASPersistentView::onCrystalChanged(double crystal)
{
    monoCrystal_->setText(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->enumNameAt(crystal));
    monoEnergyRange_->setText(QString("%1 eV - %2 eV").arg(IDEASBeamline::bl()->exposedControlByName("monoLowEV")->value()).arg(IDEASBeamline::bl()->exposedControlByName("monoHighEV")->value()));
}

void IDEASPersistentView::onRingCurrentChanged(double current)
{
    ringCurrent_->setText(QString("%1 mA").arg(current));
}
