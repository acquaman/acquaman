#include "IDEASPersistentView.h"

#include "beamline/IDEAS/IDEASBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

IDEASPersistentView::IDEASPersistentView(QWidget *parent) :
    QWidget(parent)
{
    beamStatusLabel_ = new QLabel("Beam is off!");

    beamOnButton_ = new QPushButton("Beam On");
    connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnClicked()));

    beamOffButton_ = new QPushButton("Beam Off");
    connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffClicked()));

    connect(IDEASBeamline::ideas(), SIGNAL(overallShutterStatus(bool)), this, SLOT(onShutterStatusChanged(bool)));

    energyControlEditor_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->monoEnergyControl());

    QHBoxLayout *beamChangeLayout = new QHBoxLayout;
    beamChangeLayout->addWidget(beamOnButton_);
    beamChangeLayout->addWidget(beamOffButton_);

    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addLayout(beamChangeLayout);
    mainPanelLayout->addWidget(beamStatusLabel_, 0, Qt::AlignCenter);
    mainPanelLayout->addWidget(energyControlEditor_);
    mainPanelLayout->addStretch();

    setLayout(mainPanelLayout);
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
