#include "BioXASMainMonochromatorRegionControlView.h"

BioXASMainMonochromatorRegionControlView::BioXASMainMonochromatorRegionControlView(BioXASMainMonochromatorRegionControl *regionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize variables.

	regionControl_ = 0;

	// Create UI elements.

	initializeView_ = new QWidget();
	QLabel *initializeViewLabel = new QLabel("Begin crystal change procedure?");
	QDialogButtonBox *initializeViewButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
																   QDialogButtonBox::Cancel);

	// Create and set layouts.

	QVBoxLayout *initializeViewLayout = new QVBoxLayout();
	initializeViewLayout->setMargin(0);
	initializeViewLayout->addWidget(initializeViewLabel);
	initializeViewLayout->addWidget(initializeViewButtons);

	initializeView_->setLayout(initializeViewLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(initializeView_);

	setLayout(layout);

	// Make connections.

	connect( initializeViewButtons, SIGNAL(accepted()), this, SLOT(onInitializeViewAccepted()) );
	connect( initializeViewButtons, SIGNAL(rejected()), this, SLOT(close()) );

	// Current settings.

	setRegionControl(regionControl);
}

BioXASMainMonochromatorRegionControlView::~BioXASMainMonochromatorRegionControlView()
{

}

void BioXASMainMonochromatorRegionControlView::setRegionControl(BioXASMainMonochromatorRegionControl *newControl)
{
	if (regionControl_ != newControl) {
		regionControl_ = newControl;
		emit regionControlChanged(regionControl_);
	}
}

void BioXASMainMonochromatorRegionControlView::onInitializeViewAccepted()
{
	close();
}
