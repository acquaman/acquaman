#include "AMControlValueLabel.h"
#include "beamline/AMControl.h"
#include "beamline/AMEnumeratedControl.h"
#include "ui/beamline/AMEnumeratedControlValueLabel.h"

AMControlValueLabel::AMControlValueLabel(AMControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;
	format_ = 'g';
	precision_ = 2;

	// Create UI elements.

	valueLabel_ = new QLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);

	enumeratedValueLabel_ = new AMEnumeratedControlValueLabel(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(valueLabel_);
	layout->addWidget(enumeratedValueLabel_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

AMControlValueLabel::~AMControlValueLabel()
{

}

void AMControlValueLabel::refresh()
{
	// Clear the view.

	valueLabel_->setText(QString());
	enumeratedValueLabel_->setControl(0);

	// Update the view.
	// The elements that are shown depend on the kind of control given.

	AMEnumeratedControl *enumeratedControl = qobject_cast<AMEnumeratedControl*>(control_);

	if (enumeratedControl) {
		valueLabel_->hide();

		enumeratedValueLabel_->setControl(enumeratedControl);
		enumeratedValueLabel_->show();

	} else {

		QString newText = "[Not Connected]";

		if (control_) {
			if (control_->isEnum())
				newText = control_->enumNameAt(control_->value());
			else
				newText = QString::number(control_->value(), format_.toAscii(), precision_);
		}

		valueLabel_->setText(newText);
		valueLabel_->show();

		enumeratedValueLabel_->hide();
	}
}

void AMControlValueLabel::setFormat(const QChar &newFormat)
{
	if (newFormat == 'g' || newFormat == 'G' || newFormat == 'e' || newFormat == 'E' || newFormat == 'f' || newFormat == 'F') {
		format_ = newFormat;
		refresh();
		emit formatChanged(format_);
	}
}

void AMControlValueLabel::setPrecision(int newPrecision)
{
	if (precision_ != newPrecision) {
		precision_ = newPrecision;
		refresh();
		emit precisionChanged(precision_);
	}
}

void AMControlValueLabel::setStyleSheet(const QString &styleSheet)
{
	valueLabel_->setStyleSheet(styleSheet);
	enumeratedValueLabel_->setStyleSheet(styleSheet);
}

void AMControlValueLabel::setControl(AMControl *newControl)
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

void AMControlValueLabel::setControl(AMControl *newControl, const QChar &newFormat, int newPrecision)
{
	setControl(newControl);
	setFormat(newFormat);
	setPrecision(newPrecision);
}
