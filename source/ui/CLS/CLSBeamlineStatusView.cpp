#include "CLSBeamlineStatusView.h"

#include "beamline/CLS/CLSBeamlineStatus.h"

#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"
//#include "beamline/CLS/CLSMirrorMaskState.h"
//#include "beamline/CLS/CLSSSRLMonochromatorMaskState.h"
#include "ui/CLS/CLSControlEditor.h"

CLSBeamlineStatusView::CLSBeamlineStatusView(CLSBeamlineStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamlineStatus_ = 0;
	selectedComponent_ = 0;
	selectedComponentView_ = 0;

	// Create beam status editor.
	editor_ = new CLSControlEditor(0);
	editor_->setTitle("Beam status");

	// Create components view.

	buttonBar_ = new CLSBeamlineStatusButtonBar(0);
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

CLSBeamlineStatusView::~CLSBeamlineStatusView()
{

}

void CLSBeamlineStatusView::refresh()
{
	// Update the beam status editor.

	editor_->setControl(beamlineStatus_);

	// Update the beam status button bar.

	buttonBar_->setBeamlineStatus(beamlineStatus_);

	// Update the selected component view.

	updateSelectedComponentView();
}

void CLSBeamlineStatusView::setBeamStatus(CLSBeamlineStatus *newStatus)
{
	if (beamlineStatus_ != newStatus) {

		if (beamlineStatus_)
			disconnect( beamlineStatus_, 0, this, 0 );

		beamlineStatus_ = newStatus;

		if (beamlineStatus_)
			connect( beamlineStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamlineStatus_);
	}
}

void CLSBeamlineStatusView::setSelectedComponent(AMControl *newControl)
{
	if (selectedComponent_ != newControl && beamlineStatus_->components().contains(newControl)) {
		selectedComponent_ = newControl;

		buttonBar_->setSelectedControl(selectedComponent_);

		refresh();

		emit selectedComponentChanged(selectedComponent_);
	}
}

void CLSBeamlineStatusView::updateSelectedComponentView()
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

QWidget* CLSBeamlineStatusView::createComponentView(AMControl *control)
{
	QWidget *view = 0;

	if (control) {
		if (beamlineStatus_->isShutterControl(control)) {
			CLSShutters *shutters = qobject_cast<CLSShutters*>(control);
			if (shutters) {
				QVBoxLayout *shuttersViewLayout = new QVBoxLayout();

				foreach (AMControl *shutter, shutters->shuttersList())
					shuttersViewLayout->addWidget(new CLSControlEditor(shutter));

				view = new QWidget();
				view->setLayout(shuttersViewLayout);
			}
		} else {
			QString title = "";
			if ( beamlineStatus_->isValveControl(control) ) {
				title = "Valves";
			} else if ( beamlineStatus_->isMirrorMaskControl(control) ) {
				title = "Mirror Mask";
			} else if ( beamlineStatus_->isMonoMaskControl(control) ) {
				title = "Monochromator Mask";
			}

			if (title.length() > 0) {
				CLSControlEditor *editor = new CLSControlEditor(control);
				editor->setTitle(title);
				view = editor;
			}
		}
	}

	return view;
}
