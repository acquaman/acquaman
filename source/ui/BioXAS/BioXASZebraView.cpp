#include "BioXASZebraView.h"

#include "ui/BioXAS/BioXASZebraPulseControlView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QGridLayout>

BioXASZebraView::BioXASZebraView(BioXASZebra *zebra, QWidget *parent)
	: QWidget(parent)
{
	zebra_ = zebra;

	QList<BioXASZebraPulseControl *> pulseControls = zebra_->pulseControls();

	QGridLayout *pulseLayout = new QGridLayout;
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(0)), 0, 0, 3, 3);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(1)), 0, 3, 3, 3);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(2)), 3, 0, 3, 3);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(3)), 3, 3, 3, 3);

	QList<AMPVControl *> softInputControls = zebra_->softInputControls();

	softInput1Button_ = new QPushButton("Soft In 1");
	softInput1Button_->setCheckable(true);
	connect(softInput1Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput1Toggled(bool)));
	connect(softInputControls.at(0), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput1()));

	softInput2Button_ = new QPushButton("Soft In 2");
	softInput2Button_->setCheckable(true);
	connect(softInput2Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput2Toggled(bool)));
	connect(softInputControls.at(1), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput2()));

	softInput3Button_ = new QPushButton("Soft In 3");
	softInput3Button_->setCheckable(true);
	connect(softInput3Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput3Toggled(bool)));
	connect(softInputControls.at(2), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput3()));

	softInput4Button_ = new QPushButton("Soft In 4");
	softInput4Button_->setCheckable(true);
	connect(softInput4Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput4Toggled(bool)));
	connect(softInputControls.at(3), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput4()));

	pulseLayout->addWidget(softInput1Button_, 6, 0, 1, 1);
	pulseLayout->addWidget(softInput2Button_, 6, 1, 1, 1);
	pulseLayout->addWidget(softInput3Button_, 6, 2, 1, 1);
	pulseLayout->addWidget(softInput4Button_, 6, 3, 1, 1);

//	setLayout(pulseLayout);

	AMExtendedControlEditor *softIn1ControlEditor = new AMExtendedControlEditor(zebra_->softIn1Control());

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(pulseLayout);
	mainVL->addWidget(softIn1ControlEditor);
	setLayout(mainVL);
}

BioXASZebraView::~BioXASZebraView()
{

}

void BioXASZebraView::onSoftInput1Toggled(bool state)
{
	zebra_->softInputControls().at(0)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput1()
{
	softInput1Button_->blockSignals(true);
	softInput1Button_->setChecked(int(zebra_->softInputControls().at(0)->value()) == 1);
	softInput1Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput2Toggled(bool state)
{
	zebra_->softInputControls().at(1)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput2()
{
	softInput2Button_->blockSignals(true);
	softInput2Button_->setChecked(int(zebra_->softInputControls().at(1)->value()) == 1);
	softInput2Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput3Toggled(bool state)
{
	zebra_->softInputControls().at(2)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput3()
{
	softInput3Button_->blockSignals(true);
	softInput3Button_->setChecked(int(zebra_->softInputControls().at(2)->value()) == 1);
	softInput3Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput4Toggled(bool state)
{
	zebra_->softInputControls().at(3)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput4()
{
	softInput4Button_->blockSignals(true);
	softInput4Button_->setChecked(int(zebra_->softInputControls().at(3)->value()) == 1);
	softInput4Button_->blockSignals(false);
}

