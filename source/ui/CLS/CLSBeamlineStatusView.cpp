#include "CLSBeamlineStatusView.h"

#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"
#include "beamline/CLS/CLSBeamlineStatus.h"

#include "ui/CLS/CLSControlEditor.h"

CLSBeamlineStatusView::CLSBeamlineStatusView(CLSBeamlineStatus *beamlineStatus, bool compactView, bool showBeamStatusInCompactView, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.
	beamlineStatus_ = 0;
	selectedComponent_ = 0;

	beamStatusEditor_ = 0;
	selectedComponentView_ = 0;

	compactView_ = compactView;

	// create / layout the ui components
	QVBoxLayout *contentLayout = new QVBoxLayout();
	setLayout(contentLayout);

	QWidget* beamlineStatusWidget;
	if (compactView_) {
		beamlineStatusWidget = createCompactBeamlineStatusView(showBeamStatusInCompactView);
	} else {
		beamlineStatusWidget = createFullBeamlineStatusView();
	}
	contentLayout->addWidget(beamlineStatusWidget);

	// Current settings.
	setBeamlineStatus(beamlineStatus);
}

CLSBeamlineStatusView::~CLSBeamlineStatusView()
{

}

void CLSBeamlineStatusView::refresh()
{
	// Update the beam status editor.
	if (beamStatusEditor_) {
		beamStatusEditor_->setControl(beamlineStatus_->beamlineStatusControl());
	}

	// Update the beam status button bar.
	if (componentButtonBar_)
		componentButtonBar_->setBeamlineStatus(beamlineStatus_);

	// Update the selected component view.
	updateSelectedComponentView();
}

void CLSBeamlineStatusView::setBeamlineStatus(CLSBeamlineStatus *newStatus)
{
	if (beamlineStatus_ != newStatus) {

		if (beamlineStatus_)
			disconnect( beamlineStatus_, 0, this, 0 );

		beamlineStatus_ = newStatus;

		if (beamlineStatus_) {
			connect( beamlineStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );
		}

		refresh();

		emit beamStatusChanged(beamlineStatus_);
	}
}

void CLSBeamlineStatusView::setSelectedComponent(AMControl *newControl)
{
	if (selectedComponent_ != newControl && (beamlineStatus_->components().contains(newControl) || beamlineStatus_ == newControl)) {
		selectedComponent_ = newControl;

		componentButtonBar_->setSelectedControl(selectedComponent_);

		refresh();

		emit selectedComponentChanged(selectedComponent_);
	}
}

void CLSBeamlineStatusView::updateSelectedComponentView()
{
	if (compactView_)
		return;

	// If there is an existing component view, remove it from the
	// component box layout, delete it, and hide the component box.

	if (selectedComponentView_) {
		selectedComponentBox_->layout()->removeWidget(selectedComponentView_);
		selectedComponentView_->deleteLater();
		selectedComponentView_ = 0;
		selectedComponentBox_->hide();
	}

	// Create a new view for the currently selected component. If a
	// valid view was created, add it to the component box layout
	// and show the component box.

	selectedComponentView_ = createComponentView(selectedComponent_);
	if (selectedComponentView_) {
		selectedComponentBox_->layout()->addWidget(selectedComponentView_);
		selectedComponentBox_->show();
	}
}

QWidget* CLSBeamlineStatusView::createCompactBeamlineStatusView(bool showBeamStatus)
{
	QVBoxLayout *contentLayout = new QVBoxLayout;
	contentLayout->addLayout(createBeamlineStatusButtonBarLayout());

	if (showBeamStatus) {
		beamStatusEditor_ = new CLSControlEditor(0);
		beamStatusEditor_->hideBorder();
		contentLayout->addWidget(beamStatusEditor_);
	}

	QGroupBox* beamlineStatusWidget = new QGroupBox("Beamline status");
	beamlineStatusWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	beamlineStatusWidget->setLayout(contentLayout);

	return beamlineStatusWidget;
}

QWidget* CLSBeamlineStatusView::createFullBeamlineStatusView()
{

	// Create beam status editor.
	beamStatusEditor_ = new CLSControlEditor(0);
	beamStatusEditor_->setTitle("Beam status");

	// Create components view.
	QLayout *componentBarLayout = createBeamlineStatusButtonBarLayout();

	//    --- create the selected components -----
	QLayout *selectedComponentLayout = new QVBoxLayout();

	selectedComponentBox_ = new QGroupBox();
	selectedComponentBox_->setFlat(true);
	selectedComponentBox_->setLayout(selectedComponentLayout);
	selectedComponentBox_->hide();

	QVBoxLayout *componentsBoxLayout = new QVBoxLayout();
	componentsBoxLayout->addLayout(componentBarLayout);
	componentsBoxLayout->addWidget(selectedComponentBox_);

	QGroupBox *componentsBox = new QGroupBox();
	componentsBox->setTitle("Components");
	componentsBox->setLayout(componentsBoxLayout);

	// layout the components
	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->addWidget(beamStatusEditor_);
	contentLayout->addWidget(componentsBox);

	// the beamline status widget
	QFrame* beamlineStatusWidget = new QFrame();
	beamlineStatusWidget->setLayout(contentLayout);

	return beamlineStatusWidget;
}

QLayout* CLSBeamlineStatusView::createBeamlineStatusButtonBarLayout()
{
	componentButtonBar_ = new CLSBeamlineStatusButtonBar(beamlineStatus_);
	QHBoxLayout *componentBarLayout = new QHBoxLayout();
	componentBarLayout->addStretch();
	componentBarLayout->addWidget(componentButtonBar_);
	componentBarLayout->addStretch();

	connect( componentButtonBar_, SIGNAL(selectedControlChanged(AMControl*)), this, SLOT(setSelectedComponent(AMControl*)) );

	return componentBarLayout;
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
