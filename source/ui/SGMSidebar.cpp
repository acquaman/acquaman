#include "SGMSidebar.h"

SGMSidebar::SGMSidebar(QWidget *parent) :
    QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	vl_ = new QVBoxLayout();
	mainBox_->setLayout(vl_);

	energyNC_ = new AMControlEdit(SGMBeamline::sgm()->energy());
	gratingNC_ = new AMControlEdit(SGMBeamline::sgm()->grating());
	readyLabel_ = new AMControlEdit(SGMBeamline::sgm()->beamlineReady());
	moveStatusLabel_ = new AMControlEdit(SGMBeamline::sgm()->energyMovingStatus());
	vl_->addWidget(energyNC_);
	vl_->addWidget(gratingNC_);
	vl_->addWidget(readyLabel_);
	vl_->addWidget(moveStatusLabel_);

	setLayout(mainLayout_);
}
