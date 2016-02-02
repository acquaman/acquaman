#include "BioXASSSRLMonochromatorView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/beamline/AMControlStopButton.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"

BioXASSSRLMonochromatorView::BioXASSSRLMonochromatorView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create UI elements.

	stopButton_ = new AMControlStopButton(0);

	configurationView_ = new BioXASSSRLMonochromatorConfigurationView(0);

	// Create and set layouts.

	QHBoxLayout *stopButtonLayout = new QHBoxLayout();
	stopButtonLayout->addStretch();
	stopButtonLayout->addWidget(stopButton_);
	stopButtonLayout->addStretch();

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(stopButtonLayout);
	layout->addWidget(configurationView_);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorView::~BioXASSSRLMonochromatorView()
{

}

void BioXASSSRLMonochromatorView::refresh()
{
	// Clear the view.

	stopButton_->setControl(0);
	configurationView_->setMono(0);

	// Update view elements.

	stopButton_->setControl(mono_);
	configurationView_->setMono(mono_);
}

void BioXASSSRLMonochromatorView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {
		mono_ = newMono;
		refresh();

		emit monoChanged(mono_);
	}
}
