#include "BioXASSidePOEBeamStatusView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"

BioXASSidePOEBeamStatusView::BioXASSidePOEBeamStatusView(BioXASSidePOEBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setTitle("POE beam status");
	beamStatusEditor_->setNoUnitsBox(true);

	mirrorMaskEditor_ = new AMExtendedControlEditor(0);
	mirrorMaskEditor_->setTitle("Mirror mask");
	mirrorMaskEditor_->setNoUnitsBox(true);

	monoMaskEditor_ = new AMExtendedControlEditor(0);
	monoMaskEditor_->setTitle("Mono mask");
	monoMaskEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *subControlsLayout = new QHBoxLayout();
	subControlsLayout->addWidget(mirrorMaskEditor_);
	subControlsLayout->addWidget(monoMaskEditor_);

	QGroupBox *subControlsBox = new QGroupBox();
	subControlsBox->setFlat(true);
	subControlsBox->setLayout(subControlsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(subControlsBox);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASSidePOEBeamStatusView::~BioXASSidePOEBeamStatusView()
{

}

void BioXASSidePOEBeamStatusView::refresh()
{
	// Clear the view.

	beamStatusEditor_->setControl(0);
	mirrorMaskEditor_->setControl(0);
	monoMaskEditor_->setControl(0);

	// Update view elements.

	beamStatusEditor_->setControl(beamStatus_);

	updateMirrorMaskEditor();
	updateMonoMaskEditor();
}

void BioXASSidePOEBeamStatusView::setBeamStatus(BioXASSidePOEBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(updateMirrorMaskEditor()) );
			connect( beamStatus_, SIGNAL(monoMaskStateChanged(AMControl*)), this, SLOT(updateMonoMaskEditor()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSidePOEBeamStatusView::updateMirrorMaskEditor()
{
	AMControl *mirrorMaskControl = 0;

	if (beamStatus_)
		mirrorMaskControl = beamStatus_->mirrorMaskState();

	mirrorMaskEditor_->setControl(mirrorMaskControl);
}

void BioXASSidePOEBeamStatusView::updateMonoMaskEditor()
{
	AMControl *monoMaskControl = 0;

	if (beamStatus_)
		monoMaskControl = beamStatus_->monoMaskState();

	monoMaskEditor_->setControl(monoMaskControl);
}
