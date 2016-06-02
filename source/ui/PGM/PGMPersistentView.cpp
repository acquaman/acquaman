#include "PGMPersistentView.h"

#include "beamline/PGM/PGMBeamline.h"

#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
    PGMBladeCurrentView *bladeCurrentView = new PGMBladeCurrentView;

    QHBoxLayout *bladeLayout = new QHBoxLayout;
    bladeLayout->addWidget(bladeCurrentView);
    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
    bladeCurrentBox->setLayout(bladeLayout);

    energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
    energyControlEditor_->setControlFormat('f', 3);
    energyControlEditor_->setUnits("eV");

    // Layout for energy control editor
    QHBoxLayout *energyLayout = new QHBoxLayout;
    energyLayout->addWidget(energyControlEditor_);

    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addLayout(energyLayout);
    mainPanelLayout->addWidget(bladeCurrentBox);

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
