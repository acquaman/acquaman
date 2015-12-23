#include "BioXASBeamlineStatusButtonsView.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"

BioXASBeamlineStatusButtonsView::BioXASBeamlineStatusButtonsView(QWidget *parent) :
    QWidget(parent)
{
	// Create button group.

	buttons_ = new QButtonGroup(this);
	buttons_->setExclusive(true);

	connect( buttons_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)) );

	// Create front-end shutters views.

	BioXASShuttersButton *frontEndShuttersButton = new BioXASShuttersButton(BioXASBeamline::bioXAS()->shutters());
	frontEndShuttersButton->setToolTip("Front-end shutters");
	buttons_->addButton(frontEndShuttersButton, 0);

	AMExtendedControlEditor *frontEndShuttersEditor = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->shutters());
	frontEndShuttersEditor->setTitle("Front-end shutters");
	frontEndShuttersEditor->setNoUnitsBox(true);

	buttonEditorMap_.insert(frontEndShuttersButton, frontEndShuttersEditor);

	// Create valves views.

	BioXASValvesButton *valvesButton = new BioXASValvesButton(BioXASBeamline::bioXAS()->valves());
	valvesButton->setToolTip("Valves");
	buttons_->addButton(valvesButton, 1);

	AMExtendedControlEditor *valvesEditor = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->valves());
	valvesEditor->setTitle("Valves");
	valvesEditor->setNoUnitsBox(true);

	buttonEditorMap_.insert(valvesButton, valvesEditor);

	// Create buttons box.

	QHBoxLayout *buttonsBoxLayout = new QHBoxLayout();
	buttonsBoxLayout->setMargin(0);
	buttonsBoxLayout->addWidget(frontEndShuttersButton);
	buttonsBoxLayout->addWidget(valvesButton);
	buttonsBoxLayout->addStretch();

	QWidget *buttonsBox = new QWidget();
	buttonsBox->setLayout(buttonsBoxLayout);

	// Create editors box.

	QVBoxLayout *editorsBoxLayout = new QVBoxLayout();
	editorsBoxLayout->addWidget(frontEndShuttersEditor);
	editorsBoxLayout->addWidget(valvesEditor);

	editorsBox_ = new QGroupBox();
	editorsBox_->setFlat(true);
	editorsBox_->setLayout(editorsBoxLayout);

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(buttonsBox);
	layout->addWidget(editorsBox_);

	setLayout(layout);

	// Initial settings.

	onButtonClicked(0);
}

BioXASBeamlineStatusButtonsView::~BioXASBeamlineStatusButtonsView()
{

}

void BioXASBeamlineStatusButtonsView::hideEditors()
{
	foreach (QWidget *editor, buttonEditorMap_.values()) {
		if (editor)
			editor->hide();
	}
}

void BioXASBeamlineStatusButtonsView::onButtonClicked(QAbstractButton *button)
{
	// Hide all editors.

	hideEditors();

	editorsBox_->hide();

	if (button) {

		// Identify the button's check state.

		bool buttonChecked = button->isChecked();

		if (buttonChecked) {

			// If the button is checked, the editors must be shown and the
			// shown editor should be the one corresponding to the clicked
			// button.

			QWidget *editor = buttonEditorMap_.value(button, 0);

			if (editor)
				editor->show();

			editorsBox_->show();
		}
	}
}
