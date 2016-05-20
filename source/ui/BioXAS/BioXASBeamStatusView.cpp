#include "BioXASBeamStatusView.h"

#include "beamline/CLS/CLSBeamlineStatus.h"

#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASBeamStatusButtonBar.h"

BioXASBeamStatusView::BioXASBeamStatusView(CLSBeamlineStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;
	selectedComponent_ = 0;
	selectedComponentView_ = 0;

	// Create beam status editor.

	editor_ = new BioXASControlEditor(0);
	editor_->setTitle("Beam status");

	// Create components view.

	buttonBar_ = new BioXASBeamStatusButtonBar(0);
	connect( buttonBar_, SIGNAL(selectedControlChanged(AMControl*)), this, SLOT(setSelectedComponent(AMControl*)) );

	QHBoxLayout *buttonBarLayout = new QHBoxLayout();
	buttonBarLayout->addStretch();
	buttonBarLayout->addWidget(buttonBar_);
	buttonBarLayout->addStretch();

	selectedComponentBoxLayout_ = new QVBoxLayout();

	selectedComponentBox_ = new QGroupBox();
	selectedComponentBox_->setFlat(true);
	selectedComponentBox_->setLayout(selectedComponentBoxLayout_);
	selectedComponentBox_->hide();

	QVBoxLayout *componentsBoxLayout = new QVBoxLayout();
	componentsBoxLayout->addLayout(buttonBarLayout);
	componentsBoxLayout->addWidget(selectedComponentBox_);

	QGroupBox *componentsBox = new QGroupBox();
	componentsBox->setTitle("Components");
	componentsBox->setLayout(componentsBoxLayout);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(editor_);
	layout->addWidget(componentsBox);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::refresh()
{
	// Update the beam status editor.

	editor_->setControl(beamStatus_);

	// Update the beam status button bar.

	buttonBar_->setBeamlineStatus(beamStatus_);

	// Update the selected component view.

	updateSelectedComponentView();
}

void BioXASBeamStatusView::setBeamStatus(CLSBeamlineStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASBeamStatusView::setSelectedComponent(AMControl *newControl)
{
	if (selectedComponent_ != newControl && beamStatus_->components().contains(newControl)) {
		selectedComponent_ = newControl;

		buttonBar_->setSelectedControl(selectedComponent_);

		refresh();

		emit selectedComponentChanged(selectedComponent_);
	}
}

void BioXASBeamStatusView::updateSelectedComponentView()
{
	// If there is an existing component view, remove it from the
	// component box layout, delete it, and hide the component box.

	if (selectedComponentView_) {
		selectedComponentBoxLayout_->removeWidget(selectedComponentView_);
		selectedComponentView_->deleteLater();
		selectedComponentView_ = 0;
		selectedComponentBox_->hide();
	}

	// Create a new view for the currently selected component. If a
	// valid view was created, add it to the component box layout
	// and show the component box.

	selectedComponentView_ = createComponentView(selectedComponent_);

	if (selectedComponentView_) {
		selectedComponentBoxLayout_->addWidget(selectedComponentView_);
		selectedComponentBox_->show();
	}
}

QWidget* BioXASBeamStatusView::createComponentView(AMControl *control)
{
	QWidget *view = 0;

	if (control) {

		bool controlFound = false;

		BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
		if (!controlFound && shutters) {

			QVBoxLayout *shuttersViewLayout = new QVBoxLayout();

			foreach (AMControl *shutter, shutters->shuttersList())
				shuttersViewLayout->addWidget(new BioXASControlEditor(shutter));

			view = new QWidget();
			view->setLayout(shuttersViewLayout);

			controlFound = true;
		}

		BioXASValves *valves = qobject_cast<BioXASValves*>(control);
		if (!controlFound && valves) {
			BioXASControlEditor *editor = new BioXASControlEditor(valves);
			editor->setTitle("Valves");

			view = editor;
			controlFound = true;
		}

		BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
		if (!controlFound && mirrorMask) {
			BioXASControlEditor *editor = new BioXASControlEditor(mirrorMask);
			editor->setTitle("Mirror Mask");

			view = editor;
			controlFound = true;
		}

		BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
		if (!controlFound && monoMask) {
			BioXASControlEditor *editor = new BioXASControlEditor(monoMask);
			editor->setTitle("Monochromator Mask");

			view = editor;
			controlFound = true;
		}
	}

	return view;
}
