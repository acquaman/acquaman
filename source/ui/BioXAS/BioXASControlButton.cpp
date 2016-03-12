#include "BioXASControlButton.h"

BioXASControlButton::BioXASControlButton(AMControl *control, QWidget *parent) :
	AMToolButton(parent)
{
	// Initialize class variables.

	control_ = 0;
	greenValue_ = AMNumber(AMNumber::InvalidError);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASControlButton::~BioXASControlButton()
{

}

void BioXASControlButton::refresh()
{
	QColor color = QColor();

	if (control_) {
		if (greenValue_.isValid() && control_->isConnected() && control_->value() == double(greenValue_))
			color = Qt::green;
		else if (greenValue_.isValid() && control_->isConnected())
			color = Qt::red;
		else if (greenValue_.isValid())
			color = Qt::yellow;
	}

	setColor(color);
}

void BioXASControlButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlButton::setGreenValue(const AMNumber &newValue)
{
	if (greenValue_ != newValue) {
		greenValue_ = newValue;
		emit greenValueChanged(greenValue_);
	}
}

