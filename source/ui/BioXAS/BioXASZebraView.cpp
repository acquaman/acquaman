#include "BioXASZebraView.h"

#include "ui/BioXAS/BioXASZebraPulseControlView.h"

#include <QGridLayout>

BioXASZebraView::BioXASZebraView(BioXASZebra *zebra, QWidget *parent)
	: QWidget(parent)
{
	zebra_ = zebra;

	QList<BioXASZebraPulseControl *> pulseControls = zebra_->pulseControls();

	QGridLayout *pulseLayout = new QGridLayout;
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(0)), 0, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(1)), 0, 1);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(2)), 1, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(3)), 1, 1);

	setLayout(pulseLayout);
}

BioXASZebraView::~BioXASZebraView()
{

}

