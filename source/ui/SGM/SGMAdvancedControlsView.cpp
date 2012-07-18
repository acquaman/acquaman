#include "SGMAdvancedControlsView.h"

#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ui/beamline/AMExtendedControlEditor.h"

SGMAdvancedControlsView::SGMAdvancedControlsView(QWidget *parent) :
	QWidget(parent)
{
	undulatorOffsetEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->undulatorOffset());
	undulatorOffsetEditor_->setControlFormat('f', 3);
	undulatorOffsetEditor_->overrideTitle("Undulator Offset");
	undulatorOffsetEditor_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	mirrorStripeFeedbackEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->mirrorStripeSelection());
	mirrorStripeFeedbackEditor_->setControlFormat('f', 1);
	mirrorStripeFeedbackEditor_->overrideTitle("Mirror Stripe Feedback");
	mirrorStripeFeedbackEditor_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	mirrorStripeSelectionGroup_ = new QButtonGroup();
	selectCarbonButton_ = new QRadioButton(SGMBeamline::sgm()->sgmMirrorStripeName(SGMBeamline::carbonStripe));
	selectSiliconButton_ = new QRadioButton(SGMBeamline::sgm()->sgmMirrorStripeName(SGMBeamline::siliconStripe));
	mirrorStripeSelectionGroup_->addButton(selectCarbonButton_, 0);
	mirrorStripeSelectionGroup_->addButton(selectSiliconButton_, 1);
	QGroupBox *mirrorStripSelectionBox = new QGroupBox("Mirror Stripe Selection");
	QVBoxLayout *msvl = new QVBoxLayout();
	msvl->addWidget(selectCarbonButton_);
	msvl->addWidget(selectSiliconButton_);
	msvl->setSpacing(0);
	msvl->setContentsMargins(2, 2, 2, 2);
	mirrorStripSelectionBox->setLayout(msvl);
	connect(SGMBeamline::sgm(), SIGNAL(currentMirrorStripeChanged(SGMBeamline::sgmMirrorStripe)), this, SLOT(onCurrentMirrorStripeChanged(SGMBeamline::sgmMirrorStripe)));
	connect(mirrorStripeSelectionGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMirrorStripeButtonsClicked(int)));

	QGroupBox *masterMirrorStripBox = new QGroupBox("Mirror Stripes");
	QVBoxLayout *mmvl = new QVBoxLayout();
	mmvl->addWidget(mirrorStripeFeedbackEditor_);
	mmvl->addWidget(mirrorStripSelectionBox);
	mmvl->addStretch(10);
	masterMirrorStripBox->setLayout(mmvl);

	entranceSlitEditor_ = new AMExtendedControlEditor(SGMBeamline::sgm()->entranceSlitGap());
	entranceSlitEditor_->setControlFormat('f', 1);
	entranceSlitEditor_->overrideTitle("Entrance Slit");
	entranceSlitEditor_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	endstationsAvailable_ = new QButtonGroup();
	scientaButton_ = new QRadioButton(SGMBeamline::sgm()->sgmEndstationName(SGMBeamline::scienta));
	ssaButton_ = new QRadioButton(SGMBeamline::sgm()->sgmEndstationName(SGMBeamline::ssa));
	endstationsAvailable_->addButton(scientaButton_, 0);
	endstationsAvailable_->addButton(ssaButton_, 1);
	QGroupBox *endstationsBox = new QGroupBox("Endstations");
	QVBoxLayout *sl = new QVBoxLayout();
	sl->addWidget(scientaButton_);
	sl->addWidget(ssaButton_);
	sl->setSpacing(0);
	sl->setContentsMargins(2, 2, 2, 2);
	sl->addStretch(10);
	endstationsBox->setLayout(sl);
	connect(SGMBeamline::sgm(), SIGNAL(currentEndstationChanged(SGMBeamline::sgmEndstation)), this, SLOT(onCurrentEndstationChanged(SGMBeamline::sgmEndstation)));
	connect(endstationsAvailable_, SIGNAL(buttonClicked(int)), this, SLOT(onEndstationButtonsClicked(int)));

	mainVL_ = new QVBoxLayout();
	QHBoxLayout *tmpHL = new QHBoxLayout();
	tmpHL->addWidget(undulatorOffsetEditor_);
	tmpHL->addStretch(10);
	mainVL_->addLayout(tmpHL);
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(masterMirrorStripBox);
	tmpHL->addStretch(10);
	mainVL_->addLayout(tmpHL);
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(entranceSlitEditor_);
	tmpHL->addStretch(10);
	mainVL_->addLayout(tmpHL);
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(endstationsBox);
	tmpHL->addStretch(10);
	mainVL_->addLayout(tmpHL);
	mainVL_->addStretch(10);

	setLayout(mainVL_);

	if(SGMBeamline::sgm()->isConnected())
		onCurrentMirrorStripeChanged((SGMBeamline::sgmMirrorStripe)SGMBeamline::sgm()->mirrorStripeSelection()->value());
}

void SGMAdvancedControlsView::onCurrentMirrorStripeChanged(SGMBeamline::sgmMirrorStripe newMirrorStripe){
	if(newMirrorStripe == SGMBeamline::carbonStripe)
		selectCarbonButton_->setChecked(true);
	else if(newMirrorStripe == SGMBeamline::siliconStripe)
		selectSiliconButton_->setChecked(true);
}

void SGMAdvancedControlsView::onMirrorStripeButtonsClicked(int buttonIndex){
	SGMBeamline::sgm()->setCurrentMirrorStripe((SGMBeamline::sgmMirrorStripe)buttonIndex);
}

void SGMAdvancedControlsView::onCurrentEndstationChanged(SGMBeamline::sgmEndstation newEndstation){
	if(newEndstation == SGMBeamline::scienta)
		scientaButton_->setChecked(true);
	else if(newEndstation == SGMBeamline::ssa)
		ssaButton_->setChecked(true);
}

void SGMAdvancedControlsView::onEndstationButtonsClicked(int buttonIndex){
	SGMBeamline::sgm()->setCurrentEndstation((SGMBeamline::sgmEndstation)buttonIndex);
}
