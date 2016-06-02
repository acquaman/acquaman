#include "PGMPersistentView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>


#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/PGM/PGMBladeCurrentView.h"


PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// setup the Beamline persistent view component
	QLayout * persistentViewLayout = createPersistentLayout();

	QGroupBox *persistentViewGroupBox = new QGroupBox("VLS-PGM Beamline");
	persistentViewGroupBox->setLayout(persistentViewLayout);

	QVBoxLayout *mainViewLayout = new QVBoxLayout;
	mainViewLayout->addWidget(persistentViewGroupBox);

	setLayout(mainViewLayout);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	setMaximumHeight(1000);
	setMinimumHeight(800);
	setMaximumWidth(400);
	setMinimumWidth(400);
}

QLayout* PGMPersistentView::createPersistentLayout()
{
	QVBoxLayout *persistentLayout = new QVBoxLayout;

	// create the beamline status view
	QWidget *beamlineStatusView = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), true);
	connect(beamlineStatusView, SIGNAL(selectedComponentChanged(AMControl*)), this, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)) );

	persistentLayout->addWidget(beamlineStatusView);

	// create the energy view / layout
	energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
	energyControlEditor_->setControlFormat('f', 3);
	energyControlEditor_->setUnits("eV");

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addWidget(energyControlEditor_);

// NOTES:  remove this from the persistent view for this moment. We will decide what we want later..
//	// create the PGM blade current view
//	PGMBladeCurrentView *bladeCurrentView = new PGMBladeCurrentView;

//    QHBoxLayout *bladeLayout = new QHBoxLayout;
//    bladeLayout->addWidget(bladeCurrentView);
//    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
//    bladeCurrentBox->setLayout(bladeLayout);

    // Main layout
	persistentLayout->addLayout(energyLayout);
	persistentLayout->addWidget(bladeCurrentBox);

	// add stretch for display purpose
	persistentLayout->addStretch();

	return persistentLayout;

}
