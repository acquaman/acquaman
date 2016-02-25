#include "BioXASBeamlineConfigurationView.h"

BioXASBeamlineConfigurationView::BioXASBeamlineConfigurationView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	optionalDetectorsButtonGroup_ = new QButtonGroup(this);

	// Create the optional detectors view.

	QVBoxLayout *optionalDetectorsLayout = new QVBoxLayout();

	QRadioButton *diodeButton = new QRadioButton("Diode");
	optionalDetectorsButtonGroup_->addButton(diodeButton);
	optionalDetectorsLayout->addWidget(diodeButton);

	QRadioButton *pipsButton = new QRadioButton("PIPS");
	optionalDetectorsButtonGroup_->addButton(pipsButton);
	optionalDetectorsLayout->addWidget(pipsButton);

	QRadioButton *lytleButton = new QRadioButton("Lytle");
	optionalDetectorsButtonGroup_->addButton(lytleButton);
	optionalDetectorsLayout->addWidget(lytleButton);

	QGroupBox *optionalDetectorsBox = new QGroupBox("Optional detectors");
	optionalDetectorsBox->setLayout(optionalDetectorsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(optionalDetectorsBox);

	setLayout(layout);
}

BioXASBeamlineConfigurationView::~BioXASBeamlineConfigurationView()
{

}
