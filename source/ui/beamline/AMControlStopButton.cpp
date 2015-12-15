#include "AMControlStopButton.h"
#include "beamline/AMControl.h"

AMControlStopButton::AMControlStopButton(AMControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	stopButton_ = new QPushButton();
	stopButton_->setText("Stop");

	connect( stopButton_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()) );

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(stopButton_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

AMControlStopButton::~AMControlStopButton()
{

}

void AMControlStopButton::refresh()
{
	bool enabled = false;

	if (control_)
		enabled = true;

	stopButton_->setEnabled(enabled);
}

void AMControlStopButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {
		control_ = newControl;
		refresh();

		emit controlChanged(control_);
	}
}

void AMControlStopButton::setStyleSheet(const QString &styleSheet)
{
	stopButton_->setStyleSheet(styleSheet);
}

void AMControlStopButton::onStopButtonClicked()
{
	if (control_ && control_->canStop())
		control_->stop();
}
