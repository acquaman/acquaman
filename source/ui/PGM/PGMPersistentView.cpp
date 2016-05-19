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

    energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
    energyControlEditor_->setMaximumWidth(200);
    energyControlEditor_->setControlFormat('f', 1);
    energyControlEditor_->setUnits("eV");

    isMovingText_ = new QLabel("Moving");
    isMovingIcon_ = new QLabel();
    isMovingIcon_->setPixmap(QIcon(":/16x16/greenLEDOff.png").pixmap(25));

    QHBoxLayout *movingLayout = new QHBoxLayout;
    movingLayout->addWidget(isMovingText_);
    movingLayout->addWidget(isMovingIcon_);

    connect(PGMBeamline::pgm()->energy(), SIGNAL(movingChanged(bool)), this, SLOT(onMonoMoving(bool)));

    // Layout for energy control editor
    QHBoxLayout *energyLayout = new QHBoxLayout;
    energyLayout->addWidget(energyControlEditor_);
    energyLayout->addLayout(movingLayout);

    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addLayout(energyLayout);

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

void PGMPersistentView::onMonoMoving(bool isMoving){
    if(isMoving){
        isMovingIcon_->setPixmap(QIcon(":/16x16/greenLEDOn.png").pixmap(25));
    } else {
        isMovingIcon_->setPixmap(QIcon(":/16x16/greenLEDOff.png").pixmap(25));
    }
}


