#include "BioXASBeamlineConfigurationView.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASBeamlineConfigurationView::BioXASBeamlineConfigurationView(QWidget *parent) :
    QWidget(parent)
{
	extraChannelDetectorsButtonGroup_ = new QButtonGroup(this);
	connect( extraChannelDetectorsButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)) );

	// Create the optional detectors view.

	QVBoxLayout *extraChannelDetectorsLayout = new QVBoxLayout();

	noneButton_ = new QRadioButton("None");
	extraChannelDetectorsButtonGroup_->addButton(noneButton_, BIOXASBEAMLINECONFIGURATIONVIEW_NONE_BUTTON_INDEX);
	extraChannelDetectorsLayout->addWidget(noneButton_);

	diodeButton_ = new QRadioButton("Diode");
	extraChannelDetectorsButtonGroup_->addButton(diodeButton_, BIOXASBEAMLINECONFIGURATIONVIEW_DIODE_BUTTON_INDEX);
	extraChannelDetectorsLayout->addWidget(diodeButton_);

	pipsButton_ = new QRadioButton("PIPS");
	extraChannelDetectorsButtonGroup_->addButton(pipsButton_, BIOXASBEAMLINECONFIGURATIONVIEW_PIPS_BUTTON_INDEX);
	extraChannelDetectorsLayout->addWidget(pipsButton_);

	lytleButton_ = new QRadioButton("Lytle");
	extraChannelDetectorsButtonGroup_->addButton(lytleButton_, BIOXASBEAMLINECONFIGURATIONVIEW_LYTLE_BUTTON_INDEX);
	extraChannelDetectorsLayout->addWidget(lytleButton_);

	QGroupBox *extraChannelDetectorsBox = new QGroupBox("Extra channel detectors");
	extraChannelDetectorsBox->setLayout(extraChannelDetectorsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(extraChannelDetectorsBox);

	setLayout(layout);

	// Make beamline connections.

	connect( BioXASBeamline::bioXAS(), SIGNAL(diodeDetectorChanged(AMDetector*)), this, SLOT(refresh()) );
	connect( BioXASBeamline::bioXAS(), SIGNAL(pipsDetectorChanged(AMDetector*)), this, SLOT(refresh()) );
	connect( BioXASBeamline::bioXAS(), SIGNAL(lytleDetectorChanged(AMDetector*)), this, SLOT(refresh()) );

	// Current settings.

	refresh();
}

BioXASBeamlineConfigurationView::~BioXASBeamlineConfigurationView()
{

}

void BioXASBeamlineConfigurationView::refresh()
{
	updateNoneButton();
	updateDiodeButton();
	updatePIPSButton();
	updateLytleButton();
}

void BioXASBeamlineConfigurationView::updateNoneButton()
{
	// The 'none' button is always a valid option for the extra channel detectors.
	// It is selected when the beamline has none of the extra channel detectors.

	BioXASBeamline *beamline = BioXASBeamline::bioXAS();

	if (!beamline->diodeDetector() && !beamline->pipsDetector() && !beamline->lytleDetector()) {
		extraChannelDetectorsButtonGroup_->blockSignals(true);
		noneButton_->setChecked(true);
		extraChannelDetectorsButtonGroup_->blockSignals(false);
	}
}

void BioXASBeamlineConfigurationView::updateDiodeButton()
{
	// Uncheck and disable the button.

	extraChannelDetectorsButtonGroup_->blockSignals(true);
	diodeButton_->setChecked(false);
	extraChannelDetectorsButtonGroup_->blockSignals(false);

	diodeButton_->setEnabled(false);

	// Enable the button if the beamline can have a diode, and check it if
	// the beamline has one.

	BioXASBeamline *beamline = BioXASBeamline::bioXAS();

	if (beamline->canHaveDiodeDetector()) {
		diodeButton_->setEnabled(true);

		if (beamline->diodeDetector()) {
			extraChannelDetectorsButtonGroup_->blockSignals(true);
			diodeButton_->setChecked(true);
			extraChannelDetectorsButtonGroup_->blockSignals(false);
		}
	}
}

void BioXASBeamlineConfigurationView::updatePIPSButton()
{
	// Uncheck and disable the button.

	extraChannelDetectorsButtonGroup_->blockSignals(true);
	pipsButton_->setChecked(false);
	extraChannelDetectorsButtonGroup_->blockSignals(false);

	diodeButton_->setEnabled(false);

	// Enable the button if the beamline can have a PIPS detector, and check it if
	// the beamline has one.

	BioXASBeamline *beamline = BioXASBeamline::bioXAS();

	if (beamline->canHavePIPSDetector()) {
		pipsButton_->setEnabled(true);

		if (beamline->pipsDetector()) {
			extraChannelDetectorsButtonGroup_->blockSignals(true);
			pipsButton_->setChecked(true);
			extraChannelDetectorsButtonGroup_->blockSignals(false);
		}
	}
}

void BioXASBeamlineConfigurationView::updateLytleButton()
{
	// Uncheck and disable the button.

	extraChannelDetectorsButtonGroup_->blockSignals(true);
	lytleButton_->setChecked(false);
	extraChannelDetectorsButtonGroup_->blockSignals(false);

	lytleButton_->setEnabled(false);

	// Enable the button if the beamline can have a Lytle detector, and check it if
	// the beamline has one.

	BioXASBeamline *beamline = BioXASBeamline::bioXAS();

	if (beamline->canHaveLytleDetector()) {
		lytleButton_->setEnabled(true);

		if (beamline->lytleDetector()) {
			extraChannelDetectorsButtonGroup_->blockSignals(true);
			lytleButton_->setChecked(true);
			extraChannelDetectorsButtonGroup_->blockSignals(false);
		}
	}
}

void BioXASBeamlineConfigurationView::onButtonClicked(int buttonIndex)
{
	if (buttonIndex == BIOXASBEAMLINECONFIGURATIONVIEW_NONE_BUTTON_INDEX)
		noneButton_->setChecked(true);
	else if (buttonIndex == BIOXASBEAMLINECONFIGURATIONVIEW_DIODE_BUTTON_INDEX)
		diodeButton_->setChecked(true);
	else if (buttonIndex == BIOXASBEAMLINECONFIGURATIONVIEW_PIPS_BUTTON_INDEX)
		pipsButton_->setChecked(true);
	else if (buttonIndex == BIOXASBEAMLINECONFIGURATIONVIEW_LYTLE_BUTTON_INDEX)
		lytleButton_->setChecked(true);
}
