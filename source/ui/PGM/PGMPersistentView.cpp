#include "PGMPersistentView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSControlEditor.h"
#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/PGM/PGMBeamStatusView.h"
#include "ui/PGM/PGMBPMStatusView.h"


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
	// create the beamline status view
	QWidget *beamlineStatusView = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), true);
	connect(beamlineStatusView, SIGNAL(selectedComponentChanged(AMControl*)), this, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)) );

// NOTES:  remove this from the persistent view for this moment. We will decide what we want later..
//	// create the PGM blade current view
//	PGMBladeCurrentView *bladeCurrentView = new PGMBladeCurrentView;

//    QHBoxLayout *bladeLayout = new QHBoxLayout;
//    bladeLayout->addWidget(bladeCurrentView);
//    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
//    bladeCurrentBox->setLayout(bladeLayout);

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

	gratingControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->gratingSelectionControl());

	// Main layout
	QVBoxLayout *mainPanelLayout = new QVBoxLayout;
	mainPanelLayout->addWidget(beamlineStatusView);
	mainPanelLayout->addWidget(beamStatusView_);
	mainPanelLayout->addWidget(bpmBox);
	mainPanelLayout->addWidget(energyControlEditor_);
	mainPanelLayout->addWidget(gratingControlEditor_);
//    mainPanelLayout->addWidget(bladeCurrentBox);

	// Add final stretch to the layout, so the widgets appear new the top of the view.
	mainPanelLayout->addStretch();

	return mainPanelLayout;
}
