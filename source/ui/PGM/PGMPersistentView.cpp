#include "PGMPersistentView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include "beamline/PGM/PGMBeamline.h"
#include "beamline/PGM/PGMBranchSelectionControl.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
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

	entranceSlitPositionEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->entranceSlitGap());

	QHBoxLayout* branchesLayout = new QHBoxLayout();

	branchAGroupBox_ = new QGroupBox("Branch A");
	QVBoxLayout* branchALayout = new QVBoxLayout();
	branchAGroupBox_->setLayout(branchALayout);
	branchesLayout->addWidget(branchAGroupBox_);

	branchBGroupBox_ = new QGroupBox("Branch B");
	QVBoxLayout* branchBLayout = new QVBoxLayout();
	branchBGroupBox_->setLayout(branchBLayout);
	branchesLayout->addWidget(branchBGroupBox_);

	exitSlitAGapEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchAGap());
	exitSlitAPositionEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchAPosition());
	exitSlitAPositionTrackingEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchAPositionTracking());

	branchALayout->addWidget(exitSlitAGapEditor_);
	branchALayout->addWidget(exitSlitAPositionEditor_);
	branchALayout->addWidget(exitSlitAPositionTrackingEditor_);

	exitSlitBGapEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchBGap());
	exitSlitBPositionEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchBPosition());
	exitSlitBPositionTrackingEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitBranchBPositionTracking());

	branchBLayout->addWidget(exitSlitBGapEditor_);
	branchBLayout->addWidget(exitSlitBPositionEditor_);
	branchBLayout->addWidget(exitSlitBPositionTrackingEditor_);

	AMControl *branchSelectionControl = PGMBeamline::pgm()->branchSelectionControl();
	connect(branchSelectionControl, SIGNAL(valueChanged(double)), this, SLOT(onBranchSelectionChanged(double)));

	if (branchSelectionControl->isConnected())
		onBranchSelectionChanged(branchSelectionControl->value());

	// Main layout
	QVBoxLayout *mainPanelLayout = new QVBoxLayout;
	mainPanelLayout->addWidget(beamlineStatusView);
	mainPanelLayout->addWidget(beamStatusView_);
	mainPanelLayout->addWidget(bpmBox);
	mainPanelLayout->addWidget(energyControlEditor_);
	mainPanelLayout->addWidget(gratingControlEditor_);
	mainPanelLayout->addWidget(entranceSlitPositionEditor_);
	mainPanelLayout->addLayout(branchesLayout);

	// Add final stretch to the layout, so the widgets appear new the top of the view.
	mainPanelLayout->addStretch();

	return mainPanelLayout;
}

void PGMPersistentView::onBranchSelectionChanged(double value)
{
	PGMBranchSelectionControl::Branch branchValue = PGMBranchSelectionControl::Branch(value);

	if (branchValue == PGMBranchSelectionControl::BranchA){

		branchAGroupBox_->show();
		branchBGroupBox_->hide();
		branchAGroupBox_->setEnabled(true);
		branchBGroupBox_->setEnabled(false);
	}

	else if (branchValue == PGMBranchSelectionControl::BranchB){

		branchAGroupBox_->hide();
		branchBGroupBox_->show();
		branchAGroupBox_->setEnabled(false);
		branchBGroupBox_->setEnabled(true);
	}

	else {

		branchAGroupBox_->setEnabled(false);
		branchBGroupBox_->setEnabled(false);
	}
}
