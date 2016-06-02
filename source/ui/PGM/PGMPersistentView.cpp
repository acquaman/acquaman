#include "PGMPersistentView.h"
<<<<<<< HEAD
#include "beamline/PGM/PGMBeamline.h"
#include "ui/PGM/PGMBladeCurrentView.h"

#include <QLabel>
#include <QGroupBox>
#include <QLayout>
=======

#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
>>>>>>> master

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
<<<<<<< HEAD
    bladeCurrentView_ = new PGMBladeCurrentView;

    QHBoxLayout *bladeLayout = new QHBoxLayout;
    bladeLayout->addWidget(bladeCurrentView_);
    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
    bladeCurrentBox->setLayout(bladeLayout);

    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addWidget(bladeCurrentBox);
    mainPanelLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
    // the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.
=======

    energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
    energyControlEditor_->setControlFormat('f', 3);
    energyControlEditor_->setUnits("eV");

    // Layout for energy control editor
    QHBoxLayout *energyLayout = new QHBoxLayout;
    energyLayout->addWidget(energyControlEditor_);

    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addLayout(energyLayout);

>>>>>>> master
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
<<<<<<< HEAD


=======
>>>>>>> master
