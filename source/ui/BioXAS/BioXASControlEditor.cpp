#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

#include "ui/BioXAS/BioXASControlUnitsLabel.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
    QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	valueLabel_ = new QLabel();

	unitsBox_ = new BioXASControlUnitsLabel(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(valueLabel_);
	layout->addWidget(unitsBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASControlEditor::~BioXASControlEditor()
{

}

void BioXASControlEditor::refresh()
{
	updateValueLabel();
	unitsBox_->setControl(control_);
}

void BioXASControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValueLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlEditor::updateValueLabel()
{
	QString newValue = "[Invalid]";

	if (control_) {

		newValue = "[Not measurable]";

		if (control_->canMeasure())
			newValue = QString("%1").arg(control_->value());
	}

	valueLabel_->setText(newValue);
}
