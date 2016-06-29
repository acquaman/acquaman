#include "CLSBeamlineStatusView.h"

#include "beamline/CLS/CLSBeamline.h"
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

	beamOnButton_ = 0;
	beamOffButton_ = 0;

	compactView_ = compactView;

	// create / layout the ui components
	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setSpacing(1);
	contentLayout->setMargin(2);
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

void CLSBeamlineStatusView::enableBeamOnOffActions(CLSBeamline *beamline)
{
	QLayout * beamOnOffButtonLayout = createBeamOnOffButtons();
	if (beamStatusContentLayout_) {
		beamStatusContentLayout_->addLayout(beamOnOffButtonLayout);

		onBeamStatusChanged(beamlineStatus_->isOn());

		connect(this, SIGNAL(beamOnRequested()), beamline, SLOT(onTurningBeamOnRequested()) );
		connect(this, SIGNAL(beamOffRequested()), beamline, SLOT(onTurningBeamOffRequest()) );
	}
}

void CLSBeamlineStatusView::refresh()
{
	// Update the beam status editor.
	if (beamStatusEditor_) {
		beamStatusEditor_->setControl(beamlineStatus_->beamlineStatusPVControl());
	}

	// Update the beam status button bar.
	if (beamlineStatusButtonBar_)
		beamlineStatusButtonBar_->setBeamlineStatus(beamlineStatus_);

	// Update the selected component view.
	updateSelectedComponentView();
	onBeamStatusChanged(beamlineStatus_->isOn());
}

void CLSBeamlineStatusView::onBeamStatusChanged(bool beamOn)
{
	if (beamOnButton_)
		beamOnButton_->setEnabled(!beamOn);

	if (beamOffButton_)
		beamOffButton_->setEnabled(beamOn);
}

void CLSBeamlineStatusView::setBeamlineStatus(CLSBeamlineStatus *newStatus)
{
	if (beamlineStatus_ != newStatus) {

		if (beamlineStatus_)
			disconnect( beamlineStatus_, 0, this, 0 );

		beamlineStatus_ = newStatus;

		if (beamlineStatus_) {
			connect( beamlineStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );
			connect( beamlineStatus_, SIGNAL(beamStatusChanged(bool)), this, SLOT(onBeamStatusChanged(bool)));
		}

		refresh();

		emit beamlineStatusComponentChanged(beamlineStatus_);
	}
}

void CLSBeamlineStatusView::setSelectedComponent(AMControl *newControl)
{
	if (selectedComponent_ != newControl && (beamlineStatus_->components().contains(newControl) || beamlineStatus_ == newControl)) {
		selectedComponent_ = newControl;

		beamlineStatusButtonBar_->setSelectedControl(selectedComponent_);

		refresh();
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
	beamStatusContentLayout_ = new QVBoxLayout;
	beamStatusContentLayout_->addLayout(createBeamlineStatusButtonBarLayout());

	if (showBeamStatus) {
		beamStatusEditor_ = new CLSControlEditor(0);
		beamStatusEditor_->hideBorder();
		beamStatusContentLayout_->addWidget(beamStatusEditor_);
	}

	QGroupBox* beamlineStatusWidget = new QGroupBox("Beamline status");
	beamlineStatusWidget->setLayout(beamStatusContentLayout_);

	return beamlineStatusWidget;
}

QWidget* CLSBeamlineStatusView::createFullBeamlineStatusView()
{

	// Create beam status editor.
	beamStatusEditor_ = new CLSControlEditor(0);
	beamStatusEditor_->hideBorder();

	beamStatusContentLayout_ = new QVBoxLayout;
	beamStatusContentLayout_->addWidget(beamStatusEditor_);

	QGroupBox* beamStatusWidget = new QGroupBox("Beam status");
	beamStatusWidget->setLayout(beamStatusContentLayout_);


	// Create components view.
	QLayout *componentBarLayout = createBeamlineStatusButtonBarLayout();

	selectedComponentBox_ = new QGroupBox();
	selectedComponentBox_->setFlat(true);
	selectedComponentBox_->setLayout(new QVBoxLayout());
	selectedComponentBox_->hide();

	QVBoxLayout *componentsBoxLayout = new QVBoxLayout();
	componentsBoxLayout->addLayout(componentBarLayout);
	componentsBoxLayout->addWidget(selectedComponentBox_);

	QGroupBox *componentsBox = new QGroupBox("Components");
	componentsBox->setLayout(componentsBoxLayout);

	// layout the components
	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->addWidget(beamStatusWidget);
	contentLayout->addWidget(componentsBox);

	// the beamline status widget
	QFrame* beamlineStatusWidget = new QFrame();
	beamlineStatusWidget->setLayout(contentLayout);

	return beamlineStatusWidget;
}

QLayout* CLSBeamlineStatusView::createBeamlineStatusButtonBarLayout()
{
	beamlineStatusButtonBar_ = new CLSBeamlineStatusButtonBar(beamlineStatus_);
	QHBoxLayout *componentBarLayout = new QHBoxLayout();
	componentBarLayout->addStretch();
	componentBarLayout->addWidget(beamlineStatusButtonBar_);
	componentBarLayout->addStretch();

	connect( beamlineStatusButtonBar_, SIGNAL(selectedControlChanged(AMControl*)), this, SLOT(setSelectedComponent(AMControl*)) );
	connect( beamlineStatusButtonBar_, SIGNAL(controlClicked(AMControl*)), this, SIGNAL(controlClicked(AMControl*)) );

	return componentBarLayout;
}

QLayout* CLSBeamlineStatusView::createBeamOnOffButtons()
{
	if (beamOnButton_ || beamOffButton_)
		return 0;

	beamOnButton_ = createPushButton("Beam On");
	beamOffButton_ = createPushButton("Beam Off");

	connect(beamOnButton_, SIGNAL(clicked()), this, SIGNAL(beamOnRequested()));
	connect(beamOffButton_, SIGNAL(clicked()), this, SIGNAL(beamOffRequested()));

	QHBoxLayout *beamOnOffButtonLayout = new QHBoxLayout();
	beamOnOffButtonLayout->addWidget(beamOnButton_);
	beamOnOffButtonLayout->addWidget(beamOffButton_);

	return beamOnOffButtonLayout;
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

QPushButton *CLSBeamlineStatusView::createPushButton(QString text)
{
	QPushButton *pushButton = new QPushButton();
	pushButton->setCheckable(false);
	pushButton->setAutoExclusive(true);
	pushButton->setText(text);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
	pushButton->setSizePolicy(sizePolicy);

	return pushButton;
}
