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

void AMControlLEDView::clear()
{
	led_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
}

void AMControlLEDView::update()
{
	if (control_) {
		if (control_->isConnected())
			led_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		else
			led_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

	} else {
		clear();
	}
}

void AMControlLEDView::refresh()
{
	// Clears the view.

	clear();

	// Updates the view.

	update();
}

void AMControlLEDView::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_)
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );

		refresh();
	}
}
