#include "BioXASPersistentView.h"

#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorBasicView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Testing.

	BioXASControlEditor *testEditor = new BioXASControlEditor(BioXASBeamline::bioXAS()->mono()->energy());

	// Create mono view.

	BioXASSSRLMonochromatorBasicView *monoView = new BioXASSSRLMonochromatorBasicView(BioXASBeamline::bioXAS()->mono());

	QVBoxLayout *monoBoxLayout = new QVBoxLayout();
	monoBoxLayout->addWidget(monoView);

	QGroupBox *monoBox = new QGroupBox();
	monoBox->setTitle("Monochromator");
	monoBox->setLayout(monoBoxLayout);

	// Create the scaler channels view.

	BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(BioXASBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsBoxLayout = new QVBoxLayout();
	channelsBoxLayout->addWidget(channelsView);

	QGroupBox *channelsBox = new QGroupBox();
	channelsBox->setTitle("Scaler channels");
	channelsBox->setLayout(channelsBoxLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(testEditor);
	layout->addWidget(monoBox);
	layout->addWidget(channelsBox);

	setLayout(layout);
}

BioXASPersistentView::~BioXASPersistentView()
{

}
