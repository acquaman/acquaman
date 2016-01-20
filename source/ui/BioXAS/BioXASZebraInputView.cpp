#include "BioXASZebraInputView.h"

BioXASZebraInputView::BioXASZebraInputView(BioXASZebraInput *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	inputBox_ = new QSpinBox(this);
	inputBox_->setRange(0, 63);
	connect( inputBox_, SIGNAL(editingFinished()), this, SLOT(updateControl()) );

	inputLabel_ = new QLabel(this);

	inputStatusLabel_ = new QLabel(this);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(inputBox_);
	layout->addWidget(inputLabel_);
	layout->addWidget(inputStatusLabel_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraInputView::~BioXASZebraInputView()
{

}

void BioXASZebraInputView::refresh()
{
	updateInputBox();
	updateInputLabel();
	updateInputStatusLabel();
}

void BioXASZebraInputView::setControl(BioXASZebraInput *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(inputValueChanged(int)), this, SLOT(updateInputBox()) );
			connect( control_, SIGNAL(inputValueStringChanged(QString)), this, SLOT(updateInputStatusLabel()) );
			connect( control_, SIGNAL(inputStatusChanged(bool)), this, SLOT(updateInputStatusLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraInputView::updateInputBox()
{
	int newValue = 0;

	if (control_)
		newValue = control_->inputValue();

	inputBox_->setValue(newValue);
}

void BioXASZebraInputView::updateInputLabel()
{
	QString newText;

	if (control_)
		newText = control_->inputValueString();

	inputLabel_->setText(newText);
}

void BioXASZebraInputView::updateInputStatusLabel()
{
	bool status = false;

	if (control_)
		status = control_->inputStatus();

	inputStatusLabel_->setPixmap(QIcon(status ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

void BioXASZebraInputView::updateControl()
{
	if (control_)
		control_->setInputValue(inputBox_->value());
}
