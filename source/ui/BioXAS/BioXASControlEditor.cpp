#include "BioXASControlEditor.h"
#include "beamline/AMControl.h"

BioXASControlEditor::BioXASControlEditor(AMControl *control, QWidget *parent) :
    QGroupBox(parent)
{
	// Initialize class variables.

	control_ = 0;

	displayControlUnits_ = true;

	// Create UI elements.

	valueLabel_ = new QLabel();

	unitsLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(valueLabel_);
	layout->addWidget(unitsLabel_);

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
	updateUnitsLabel();
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
			connect( control_, SIGNAL(unitsChanged(QString)), this, SLOT(updateUnitsLabel()) );
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

void BioXASControlEditor::updateUnitsLabel()
{
	QString newUnits;

	if (control_)
		newUnits = control_->units();

	unitsLabel_->setText(newUnits);
}
