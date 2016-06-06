#include "CLSValueSetpointEditorDialog.h"

#include <QPushButton>

#include "ui/CLS/CLSValueSetpointEditor.h"

CLSValueSetpointEditorDialog::CLSValueSetpointEditorDialog(QWidget *parent, Qt::WindowFlags flags) :
	QDialog(parent, flags)
{
	// Create UI elements.

	QLabel *prompt = new QLabel("New value:");

	setpointEditor_ = new CLSValueSetpointEditor(CLSValueSetpointEditor::TypeDouble);
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

CLSValueSetpointEditorDialog::~CLSValueSetpointEditorDialog()
{

}

double CLSValueSetpointEditorDialog::value() const
{
	return setpointEditor_->value();
}

bool CLSValueSetpointEditorDialog::minimumSet() const
{
	return setpointEditor_->minimumSet();
}

double CLSValueSetpointEditorDialog::minimum() const
{
	return setpointEditor_->minimum();
}

bool CLSValueSetpointEditorDialog::maximumSet() const
{
	return setpointEditor_->maximumSet();
}

double CLSValueSetpointEditorDialog::maximum() const
{
	return setpointEditor_->maximum();
}

void CLSValueSetpointEditorDialog::setValue(double newValue)
{
	setpointEditor_->setValue(newValue);
}

void CLSValueSetpointEditorDialog::setValues(const QStringList &newValues)
{
	setpointEditor_->setInputType(CLSValueSetpointEditor::TypeEnum);
	setpointEditor_->setValues(newValues);
}

void CLSValueSetpointEditorDialog::setMinimumValue(double newMin)
{
	setpointEditor_->setMinimumValue(newMin);
}

void CLSValueSetpointEditorDialog::setMaximumValue(double newMax)
{
	setpointEditor_->setMaximumValue(newMax);
}

void CLSValueSetpointEditorDialog::updateButtons()
{
	bool okEnabled = true;

	// Determine the enabled state of the OK button.

	CLSValueSetpointEditor::InputStatus inputStatus = setpointEditor_->inputStatus();

	if (inputStatus == CLSValueSetpointEditor::StatusBad)
		okEnabled = false;

	// Set the new enabled state.

	buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(okEnabled);
}
