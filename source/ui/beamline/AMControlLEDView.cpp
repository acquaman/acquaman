#include "AMControlLEDView.h"

AMControlLEDView::AMControlLEDView(AMControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	led_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(led_);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

AMControlLEDView::~AMControlLEDView()
{

}

void AMControlLEDView::refresh()
{
	// Clears the view.

	setAsGreenOff();

	// Updates the view.

	updateLED();
}

void AMControlLEDView::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(updateLED()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateLED()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMControlLEDView::setAsGreenOff()
{
	led_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
}

void AMControlLEDView::setAsGreenOn()
{
	led_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
}

void AMControlLEDView::setAsRedOff()
{
	led_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
}

void AMControlLEDView::setAsRedOn()
{
	led_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
}

void AMControlLEDView::setAsYellowOff()
{
	led_->setPixmap(QPixmap(":/22x22/yellowLEDOff.png"));
}

void AMControlLEDView::setAsYellowOn()
{
	led_->setPixmap(QPixmap(":/22x22/yellowLEDOn.png"));
}

void AMControlLEDView::updateLED()
{
	if (shouldBeGreenOff())
		setAsGreenOff();
	else if (shouldBeGreenOn())
		setAsGreenOn();
	else if (shouldBeRedOff())
		setAsRedOff();
	else if (shouldBeRedOn())
		setAsRedOn();
	else if (shouldBeYellowOff())
		setAsYellowOff();
	else if (shouldBeYellowOn())
		setAsYellowOn();
	else
		setAsGreenOff();
}
