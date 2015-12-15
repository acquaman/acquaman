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
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(0)), 0, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(1)), 0, 1);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(2)), 1, 0);
	pulseLayout->addWidget(new BioXASZebraPulseControlView(pulseControls.at(3)), 1, 1);

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

