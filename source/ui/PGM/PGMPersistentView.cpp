#include "PGMPersistentView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/PGM/PGMBeamStatusView.h"
#include "ui/PGM/PGMBPMStatusView.h"



PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
    PGMBladeCurrentView *bladeCurrentView = new PGMBladeCurrentView;

    QHBoxLayout *bladeLayout = new QHBoxLayout;
    bladeLayout->addWidget(bladeCurrentView);
    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
    bladeCurrentBox->setLayout(bladeLayout);

	beamStatusView_ = new PGMBeamStatusView();
    bpmStatusView_ = new PGMBPMStatusView();

    // For organizational purpose on the persistent view. Create a groupbox with a header and then put a BPM layout inside of it.
    QGroupBox *bpmBox = new QGroupBox("Beam Position Monitors");
    QHBoxLayout *bpmLayout = new QHBoxLayout;
    bpmLayout->addWidget(bpmStatusView_);
    bpmBox->setLayout(bpmLayout);


    // Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
    // the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.

    energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
    energyControlEditor_->setControlFormat('f', 3);
    energyControlEditor_->setUnits("eV");

    // Layout for energy control editor
    QHBoxLayout *energyLayout = new QHBoxLayout;
    energyLayout->addWidget(energyControlEditor_);

    // Main layout
	QVBoxLayout *mainPanelLayout = new QVBoxLayout;
	mainPanelLayout->addWidget(beamStatusView_);
	mainPanelLayout->addWidget(bpmBox);
    mainPanelLayout->addLayout(energyLayout);
    mainPanelLayout->addWidget(bladeCurrentBox);

	// Add final stretch to the layout, so the widgets appear new the top of the view.
	mainPanelLayout->addStretch();

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
