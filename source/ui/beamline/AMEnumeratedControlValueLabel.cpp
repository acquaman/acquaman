#include "AMEnumeratedControlValueLabel.h"
#include "beamline/AMEnumeratedControl.h"

AMEnumeratedControlValueLabel::AMEnumeratedControlValueLabel(AMEnumeratedControl *control, QWidget *parent) : QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	valueLabel_ = new QLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

AMEnumeratedControlValueLabel::~AMEnumeratedControlValueLabel()
{

}

void AMEnumeratedControlValueLabel::refresh()
{
	// Clears the view.

	valueLabel_->setText(QString());

	// Update the view.

	QString newText = "[Not Connected]";

	if (control_ && control_->canMeasure())
		newText = control_->indexName(int(control_->value()));

	valueLabel_->setText(newText);
}

void AMEnumeratedControlValueLabel::setStyleSheet(const QString &styleSheet)
{
	valueLabel_->setStyleSheet(styleSheet);
}

void AMEnumeratedControlValueLabel::setControl(AMEnumeratedControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_)
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );

		refresh();

		emit controlChanged(control_);
	}
}
