#include "BioXASControlUnitsLabel.h"
#include "beamline/AMControl.h"

BioXASControlUnitsLabel::BioXASControlUnitsLabel(AMControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	unitsLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(unitsLabel_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASControlUnitsLabel::~BioXASControlUnitsLabel()
{

}

void BioXASControlUnitsLabel::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASControlUnitsLabel::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(update()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlUnitsLabel::clear()
{
	unitsLabel_->setText("");
}

void BioXASControlUnitsLabel::update()
{
	QString newUnits;

	if (control_)
		newUnits = control_->units();

	unitsLabel_->setText(newUnits);
}
