#include "BioXASZebraView.h"

#include "ui/BioXAS/BioXASZebraPulseControlView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASZebraLogicBlockView.h"

#include <QGridLayout>

BioXASZebraView::BioXASZebraView(BioXASZebra *zebra, QWidget *parent)
	: QTabWidget(parent)
{
	zebra_ = zebra;

	// The pulse views.

	QGridLayout *pulseLayout = new QGridLayout;
	pulseLayout->addWidget(new BioXASZebraPulseControlView(zebra_->pulseControlAt(0)), 0, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(zebra_->pulseControlAt(1)), 0, 1);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(zebra_->pulseControlAt(2)), 1, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(zebra_->pulseControlAt(3)), 1, 1);

	AMControlSet *softInputControls = zebra_->softInputControls();

	softInput1Button_ = new QPushButton("Soft In 1");
	softInput1Button_->setCheckable(true);
	connect(softInput1Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput1Toggled(bool)));
	connect(softInputControls->at(0), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput1()));

	softInput2Button_ = new QPushButton("Soft In 2");
	softInput2Button_->setCheckable(true);
	connect(softInput2Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput2Toggled(bool)));
	connect(softInputControls->at(1), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput2()));

	softInput3Button_ = new QPushButton("Soft In 3");
	softInput3Button_->setCheckable(true);
	connect(softInput3Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput3Toggled(bool)));
	connect(softInputControls->at(2), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput3()));

	softInput4Button_ = new QPushButton("Soft In 4");
	softInput4Button_->setCheckable(true);
	connect(softInput4Button_, SIGNAL(toggled(bool)), this, SLOT(onSoftInput4Toggled(bool)));
	connect(softInputControls->at(3), SIGNAL(valueChanged(double)), this, SLOT(updateSoftInput4()));

	QHBoxLayout *softInputsLayout = new QHBoxLayout();
	softInputsLayout->addStretch();
	softInputsLayout->addWidget(softInput1Button_);
	softInputsLayout->addWidget(softInput2Button_);
	softInputsLayout->addWidget(softInput3Button_);
	softInputsLayout->addWidget(softInput4Button_);
	softInputsLayout->addStretch();

	QVBoxLayout *pulseViewLayout = new QVBoxLayout();
	pulseViewLayout->addLayout(pulseLayout);
	pulseViewLayout->addLayout(softInputsLayout);

	QWidget *pulseView = new QWidget();
	pulseView->setLayout(pulseViewLayout);

	addTab(pulseView, "Pulse blocks");

	// The AND blocks.

	QGridLayout *andBlocksLayout = new QGridLayout;
	andBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->andBlockAt(0), "AND 1"), 0, 0);
	andBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->andBlockAt(1), "AND 2"), 0, 1);
	andBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->andBlockAt(2), "AND 3"), 1, 0);
	andBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->andBlockAt(3), "AND 4"), 1, 1);

	QWidget *andBlocksView = new QWidget();
	andBlocksView->setLayout(andBlocksLayout);

	addTab(andBlocksView, "AND blocks");

	// The OR blocks.

	QGridLayout *orBlocksLayout = new QGridLayout;
	orBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->orBlockAt(0), "OR 1"), 0, 0);
	orBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->orBlockAt(1), "OR 2"), 0, 1);
	orBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->orBlockAt(2), "OR 3"), 1, 0);
	orBlocksLayout->addWidget(new BioXASZebraLogicBlockView(zebra_->orBlockAt(3), "OR 4"), 1, 1);

	QWidget *orBlocksBox = new QWidget();
	orBlocksBox->setLayout(orBlocksLayout);

	addTab(orBlocksBox, "OR blocks");
}

BioXASZebraView::~BioXASZebraView()
{

}

void BioXASZebraView::onSoftInput1Toggled(bool state)
{
	zebra_->softInputControlAt(0)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput1()
{
	softInput1Button_->blockSignals(true);
	softInput1Button_->setChecked(int(zebra_->softInputControlAt(0)->value()) == 1);
	softInput1Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput2Toggled(bool state)
{
	zebra_->softInputControlAt(1)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput2()
{
	softInput2Button_->blockSignals(true);
	softInput2Button_->setChecked(int(zebra_->softInputControlAt(1)->value()) == 1);
	softInput2Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput3Toggled(bool state)
{
	zebra_->softInputControlAt(2)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput3()
{
	softInput3Button_->blockSignals(true);
	softInput3Button_->setChecked(int(zebra_->softInputControlAt(2)->value()) == 1);
	softInput3Button_->blockSignals(false);
}

void BioXASZebraView::onSoftInput4Toggled(bool state)
{
	zebra_->softInputControlAt(3)->move(state ? 1 : 0);
}

void BioXASZebraView::updateSoftInput4()
{
	softInput4Button_->blockSignals(true);
	softInput4Button_->setChecked(int(zebra_->softInputControlAt(3)->value()) == 1);
	softInput4Button_->blockSignals(false);
}

