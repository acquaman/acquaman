#include "PGMSlitControlView.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

#include "beamline/PGM/PGMBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

PGMSlitControlView::PGMSlitControlView(QWidget *parent) :
    QWidget(parent)
{

	QVBoxLayout* mainLayout = new QVBoxLayout();

	entranceSlitPositionEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->entranceSlitGap());
	mainLayout->addWidget(entranceSlitPositionEditor_);

	QHBoxLayout* branchesLayout = new QHBoxLayout();
	mainLayout->addLayout(branchesLayout);

	QGroupBox* branchAGroupBox = new QGroupBox("Branch A");
	QVBoxLayout* branchALayout = new QVBoxLayout();
	branchAGroupBox->setLayout(branchALayout);
	branchesLayout->addWidget(branchAGroupBox);

	QGroupBox* branchBGroupBox = new QGroupBox("Branch B");
	QVBoxLayout* branchBLayout = new QVBoxLayout();
	branchBGroupBox->setLayout(branchBLayout);
	branchesLayout->addWidget(branchBGroupBox);

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

	setLayout(mainLayout);

}
