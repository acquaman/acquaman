#include "BioXASBeamlineConfigurationView.h"

BioXASBeamlineConfigurationView::BioXASBeamlineConfigurationView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	extraChannelDetectorsButtonGroup_ = new QButtonGroup(this);

	// Create the optional detectors view.

	QVBoxLayout *extraChannelDetectorsLayout = new QVBoxLayout();

	QRadioButton *diodeButton = new QRadioButton("Diode");
	extraChannelDetectorsButtonGroup_->addButton(diodeButton);
	extraChannelDetectorsLayout->addWidget(diodeButton);

	QRadioButton *pipsButton = new QRadioButton("PIPS");
	extraChannelDetectorsButtonGroup_->addButton(pipsButton);
	extraChannelDetectorsLayout->addWidget(pipsButton);

	QRadioButton *lytleButton = new QRadioButton("Lytle");
	extraChannelDetectorsButtonGroup_->addButton(lytleButton);
	extraChannelDetectorsLayout->addWidget(lytleButton);

	QGroupBox *extraChannelDetectorsBox = new QGroupBox("Extra channel detectors");
	extraChannelDetectorsBox->setLayout(extraChannelDetectorsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(extraChannelDetectorsBox);

	setLayout(layout);
}

BioXASBeamlineConfigurationView::~BioXASBeamlineConfigurationView()
{

}
