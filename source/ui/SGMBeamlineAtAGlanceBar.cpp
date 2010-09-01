#include "SGMBeamlineAtAGlanceBar.h"

SGMBeamlineAtAGlanceBar::SGMBeamlineAtAGlanceBar(QWidget *parent) :
	QFrame(parent)
{
	hl_ = new QHBoxLayout();

	energyNC_ = new AMControlEdit(SGMBeamline::sgm()->energy());
	gratingNC_ = new AMControlEdit(SGMBeamline::sgm()->grating());
	readyLabel_ = new AMControlEdit(SGMBeamline::sgm()->beamlineReady());
	moveStatusLabel_ = new AMControlEdit(SGMBeamline::sgm()->energyMovingStatus());
	hl_->addWidget(energyNC_);
	hl_->addWidget(gratingNC_);
	hl_->addSpacing(40);
	hl_->addWidget(readyLabel_);
	hl_->addWidget(moveStatusLabel_);
	hl_->addStretch(1);

	setLayout(hl_);

	setObjectName("SGMBeamlineAtAGlanceBar");
	// setStyleSheet("QFrame#SGMBeamlineAtAGlanceBar {	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255)); border-bottom: 1px solid black; }");
}
