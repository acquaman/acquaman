#include "AMPVControlValueLabel.h"
#include "beamline/AMPVControl.h"

AMPVControlValueLabel::AMPVControlValueLabel(AMReadOnlyPVControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;
	format_ = 'g';
	precision_ = 2;

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

AMPVControlValueLabel::~AMPVControlValueLabel()
{

}

void AMPVControlValueLabel::refresh()
{
	// Clears the view.

	valueLabel_->setText(QString());

	// Update the view.

	QString newText = "[Not Connected]";

	if (control_ && control_->canMeasure()) {
		if (control_->isEnum())
			newText = control_->enumNameAt(control_->value());
		else
			newText = QString::number(control_->value(), format_.toAscii(), precision_);
	}

	valueLabel_->setText(newText);
}

void AMPVControlValueLabel::setFormat(const QChar &newFormat)
{
	if (newFormat == 'g' || newFormat == 'G' || newFormat == 'e' || newFormat == 'E' || newFormat == 'f' || newFormat == 'F') {
		format_ = newFormat;
		refresh();
		emit formatChanged(format_);
	}
}

void AMPVControlValueLabel::setPrecision(int newPrecision)
{
	if (precision_ != newPrecision) {
		precision_ = newPrecision;
		refresh();
		emit precisionChanged(precision_);
	}
}

void AMPVControlValueLabel::setControl(AMReadOnlyPVControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMPVControlValueLabel::setControl(AMReadOnlyPVControl *newControl, const QChar &newFormat, int newPrecision)
{
	setControl(newControl);
	setFormat(newFormat);
	setPrecision(newPrecision);
}
