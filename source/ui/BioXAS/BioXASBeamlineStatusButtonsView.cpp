#include "BioXASBeamlineStatusButtonsView.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"

BioXASBeamlineStatusButtonsView::BioXASBeamlineStatusButtonsView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	selectedButton_ = 0;

	buttons_ = new QButtonGroup(this);
	buttons_->setExclusive(false);

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

	// Current settings.

	refresh();
}

BioXASBeamlineStatusButtonsView::~BioXASBeamlineStatusButtonsView()
{

}

void BioXASBeamlineStatusButtonsView::refresh()
{
	// Clear the view.

	hideEditors();

	// Show the editor corresponding to the selected button.

	showEditorForButton(selectedButton_);
}

void BioXASBeamlineStatusButtonsView::setSelectedButton(QAbstractButton *newButton)
{
	if (selectedButton_ != newButton) {

		if (selectedButton_)
			selectedButton_->setChecked(false);

		selectedButton_ = newButton;

		if (selectedButton_)
			selectedButton_->setChecked(true);

		refresh();

		emit selectedButtonChanged();
	}
}

void BioXASBeamlineStatusButtonsView::showEditorForButton(QAbstractButton *button)
{
	// Show the editor corresponding to the given button.

	if (button) {
		QWidget *editor = buttonEditorMap_.value(button, 0);

		if (editor) {
			editor->show();
			editorsBox_->show();
		}
	}
}

void BioXASBeamlineStatusButtonsView::hideEditors()
{
	// Hide each individual editor widget.

	foreach (QWidget *editor, buttonEditorMap_.values()) {
		if (editor)
			editor->hide();
	}

	// Hide the editors group.

	editorsBox_->hide();
}

void BioXASBeamlineStatusButtonsView::onButtonClicked(QAbstractButton *clickedButton)
{
	// If the group's selection is identical to the current
	// selection, the selected button should be unchecked.
	// Otherwise, the clicked button should become the new
	// selection.

	if (selectedButton_ == clickedButton)
		setSelectedButton(0);
	else
		setSelectedButton(clickedButton);
}
