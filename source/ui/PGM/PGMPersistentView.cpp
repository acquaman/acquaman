#include "PGMPersistentView.h"

#include "beamline/PGM/PGMBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
    // Set to null. Will instansiate when needed.
    beamOnAction_ = 0;
    beamOffAction_ = 0;

    exitSlitGapControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitGap());
    entranceSlitGapControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->entranceSlitGap());

    QHBoxLayout *exitSlitLayout = new QHBoxLayout;
    exitSlitLayout->addWidget(exitSlitGapControlEditor_);

    QHBoxLayout *entranceSlitLayout = new QHBoxLayout;
    entranceSlitLayout->addWidget(entranceSlitGapControlEditor_);

    beamOnButton_ = new QPushButton("Beam On");
    beamOffButton_ = new QPushButton("Beam Off");

    connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
    connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));


    QHBoxLayout *beamOnOffLayout = new QHBoxLayout;
    beamOnOffLayout->addWidget(beamOnButton_);
    beamOnOffLayout->addWidget(beamOffButton_);


    // Main layout
    QVBoxLayout *mainPanelLayout = newQVBoxLayout;
    mainPanelLayout->addLayout(exitSlitLayout);
    mainPanelLayout->addLayout(entranceSlitLayout);
    mainPanelLayout->addLayout(beamOnOffLayout);

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
// ToDo: Will not build until Issue 2164 - Shutters and Valves is completed.
void PGMPersistentView::onBeamOnButtonClicked(){
    // If Beam On Action already created, return.
    if(beamOnAction_)
        return;
    // Create beam on action, tell beamline to set up valves and shutters.
    beamOnAction_ = PGMBeamline::pgm()->createBeamOnActions();
    if(beamOnAction_){
        connect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
        connect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
        beamOnAction_->start();
    } else {
        AMErrorMon::error(this, 0, QString("Failed to create the beam on actions due to either unconnected or openned valves."));
    }
}

// ToDo: Will not build until Issue 2164 - Shutters and Valves is completed.
void PGMPersistentView::onBeamOnActionFinished(){
    disconnect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
    disconnect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));

    beamOnAction_->deleteLater();
    beamOnAction_ = 0;
}

// ToDo: Will not build until Issue 2164 - Shutters and Valves is completed.
void PGMPersistentView::onBeamOffButtonClicked(){
    // If Beam Off Action already created, return.
    if(beamOffAction_)
        return;
    // Create beam off action. Tells beamline to shut off shutters and close valves.
    beamOffAction_ = PGMBeamline::pgm()->createBeamOffActions();
    if (beamOffAction_) {
        connect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
        connect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
        beamOffAction_->start();
    } else {
        AMErrorMon::information(this, 0, QString("Failed to create the beam off actions due to either unconnected valves or closed shutters."));
    }
}

// ToDo: Will not build until Issue 2164 - Shutters and Valves is completed.
void PGMPersistentView::onBeamOffActionFinished(){
    disconnect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
    disconnect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));

    beamOffAction_->deleteLater();
    beamOffAction_ = 0;
}










