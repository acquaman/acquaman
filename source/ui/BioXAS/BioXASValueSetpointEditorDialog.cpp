#include "BioXASValueSetpointEditorDialog.h"
#include "ui/BioXAS/BioXASValueSetpointEditor.h"
#include <QPushButton>

BioXASValueSetpointEditorDialog::BioXASValueSetpointEditorDialog(QWidget *parent, Qt::WindowFlags flags) :
	QDialog(parent, flags)
{
	// Create UI elements.

	QLabel *prompt = new QLabel("New value:");

	setpointEditor_ = new BioXASValueSetpointEditor(BioXASValueSetpointEditor::TypeDouble);
	connect( setpointEditor_, SIGNAL(inputStatusChanged(InputStatus)), this, SLOT(updateButtons()) );

	buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect( buttonBox_, SIGNAL(accepted()), this, SLOT(accept()) );
	connect( buttonBox_, SIGNAL(rejected()), this, SLOT(reject()) );

	// Create and set main layouts.

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->setMargin(0);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(buttonBox_);
	buttonsLayout->addStretch();

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(prompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(setpointEditor_, 0, 1);
	layout->addLayout(buttonsLayout, 1, 1);

	setLayout(layout);
}

BioXASValueSetpointEditorDialog::~BioXASValueSetpointEditorDialog()
{

}

double BioXASValueSetpointEditorDialog::value() const
{
	return setpointEditor_->value();
}

bool BioXASValueSetpointEditorDialog::minimumSet() const
{
	return setpointEditor_->minimumSet();
}

double BioXASValueSetpointEditorDialog::minimum() const
{
	return setpointEditor_->minimum();
}

bool BioXASValueSetpointEditorDialog::maximumSet() const
{
	return setpointEditor_->maximumSet();
}

double BioXASValueSetpointEditorDialog::maximum() const
{
	return setpointEditor_->maximum();
}

void BioXASValueSetpointEditorDialog::setValue(double newValue)
{
	setpointEditor_->setValue(newValue);
}

void BioXASValueSetpointEditorDialog::setValues(const QStringList &newValues)
{
	setpointEditor_->setInputType(BioXASValueSetpointEditor::TypeEnum);
	setpointEditor_->setValues(newValues);
}

void BioXASValueSetpointEditorDialog::setMinimum(double newMin)
{
	setpointEditor_->setMinimum(newMin);
}

void BioXASValueSetpointEditorDialog::setMaximum(double newMax)
{
	setpointEditor_->setMaximum(newMax);
}

void BioXASValueSetpointEditorDialog::updateButtons()
{
	bool okEnabled = true;

	// Determine the enabled state of the OK button.

	BioXASValueSetpointEditor::InputStatus inputStatus = setpointEditor_->inputStatus();

	if (inputStatus == BioXASValueSetpointEditor::StatusBad)
		okEnabled = false;

	// Set the new enabled state.

	buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(okEnabled);
}
