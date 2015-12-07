#include "AMEnumeratedControlComboBox.h"
#include "beamline/AMEnumeratedControl.h"

AMEnumeratedControlComboBox::AMEnumeratedControlComboBox(AMEnumeratedControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	comboBox_ = new QComboBox(this);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(comboBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);
}

AMEnumeratedControlComboBox::~AMEnumeratedControlComboBox()
{

}

void AMEnumeratedControlComboBox::refresh()
{
	// Clears the view.

	clearBox();

	// Updates the view--adds value options and sets current value.

	if (control_) {
		QList<int> indices = control_->indices();

		foreach (int index, indices) {
			QString indexString = control_->indexName(index);

			if (!indexString.isEmpty())
				comboBox_->addItem(indexString, index);
		}
	}

	updateBoxValue();
}

void AMEnumeratedControlComboBox::setControl(AMEnumeratedControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(valueOptionsChanged()), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateBoxValue()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMEnumeratedControlComboBox::clearBox()
{
	comboBox_->clear();
}

void AMEnumeratedControlComboBox::updateBoxValue()
{
	int newIndex = 0;

	if (control_) {
		int comboIndex = comboBox_->findData(int(control_->value()));

		if (comboIndex > -1)
			newIndex = comboIndex;
	}

	comboBox_->setCurrentIndex(newIndex);
}
