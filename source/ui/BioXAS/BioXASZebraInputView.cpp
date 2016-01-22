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
	inputLabel_->setAlignment(Qt::AlignCenter);
	inputLabel_->setMinimumWidth(75);

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
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(inputValueChanged(int)), this, SLOT(updateInputBox()) );
			connect( control_, SIGNAL(inputValueStringChanged(QString)), this, SLOT(updateInputLabel()) );
			connect( control_, SIGNAL(inputStateChanged(double)), this, SLOT(updateInputStatusLabel()) );
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

	inputBox_->blockSignals(true);
	inputBox_->setValue(newValue);
	inputBox_->blockSignals(false);
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
	bool stateHigh = false;

	if (control_)
		stateHigh = control_->isInputStateHigh();

	inputStatusLabel_->setPixmap(QIcon(stateHigh ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

void BioXASZebraInputView::updateControl()
{
	if (control_)
		control_->setInputValue(inputBox_->value());
}
