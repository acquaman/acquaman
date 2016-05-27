#include "CLSBeamlineStatusView.h"

#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSBeamlineStatus.h"

#include "ui/CLS/CLSControlEditor.h"

CLSBeamlineStatusView::CLSBeamlineStatusView(CLSBeamlineStatus *beamlineStatus, bool compactView, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.
	beamlineStatus_ = 0;
	selectedComponent_ = 0;

	beamlineStatusComponent_ = 0;
	selectedComponentView_ = 0;

	compactView_ = compactView;

	// create / layout the ui components
	QVBoxLayout *contentLayout = new QVBoxLayout();
	setLayout(contentLayout);

	QWidget* beamlineStatusWidget;
	if (compactView_) {
		beamlineStatusWidget = createCompactBeamlineStatusView();
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
	if (beamlineStatusComponent_)
		beamlineStatusComponent_->setControl(beamlineStatus_);

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
			connect( beamlineStatus_, SIGNAL(beamStatusChanged(bool)), this, SLOT(onBeamStatusChanged(bool)) );
//			connect( beamlineStatus_, SIGNAL(safetyShutterClosed(bool)), this, SLOT(onSafetyShutterClosed(bool)) );
		}

		refresh();

		emit beamlineStatusChanged(beamlineStatus_);
	}
}

void CLSBeamlineStatusView::setSelectedComponent(AMControl *newControl)
{
	if (selectedComponent_ != newControl && beamlineStatus_->components().contains(newControl)) {
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

void CLSBeamlineStatusView::onBeamStatusChanged(bool isOn)
{
	if(isOn) {
		beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		beamOnButton_->setChecked(true);
	}
	else {
		beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		beamOffButton_->setChecked(true);
	}
}

QWidget* CLSBeamlineStatusView::createCompactBeamlineStatusView()
{
	QLayout *componentBarLayout = createBeamlineStatusButtonBarLayout();
	QLayout* beamOnOffButtonLayout = createBeamOnOffButtons();

	QVBoxLayout *beamlineStatusLayout = new QVBoxLayout();
	beamlineStatusLayout->addLayout(componentBarLayout);
	beamlineStatusLayout->addLayout(beamOnOffButtonLayout);

	QGroupBox* beamlineStatusWidget = new QGroupBox("Beamline status");
	beamlineStatusWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	beamlineStatusWidget->setLayout(beamlineStatusLayout);

	return beamlineStatusWidget;
}

QWidget* CLSBeamlineStatusView::createFullBeamlineStatusView()
{

	// Create beam status editor.
	QLayout* beamOnOffButtonLayout = createBeamOnOffButtons();

	beamlineStatusComponent_ = new CLSControlEditor(0);
	beamlineStatusComponent_->setTitle("Beamline status");
	beamlineStatusComponent_->addLayout(beamOnOffButtonLayout);

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
	contentLayout->addWidget(beamlineStatusComponent_);
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

QLayout* CLSBeamlineStatusView::createBeamOnOffButtons()
{
	beamlineStatusLED_ = new QLabel();
	beamlineStatusLED_->setObjectName("beamlineStatusLED");
	beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	beamOnButton_ = createPushButton("Beam On");
	beamOffButton_ = createPushButton("Beam Off");

	QHBoxLayout *beamOnOffButtonLayout = new QHBoxLayout();
	beamOnOffButtonLayout->addStretch();
	beamOnOffButtonLayout->addWidget(beamlineStatusLED_);
	beamOnOffButtonLayout->addWidget(beamOnButton_);
	beamOnOffButtonLayout->addWidget(beamOffButton_);
	beamOnOffButtonLayout->addStretch();

	connect(beamOnButton_, SIGNAL(clicked()), this, SIGNAL(beamOnRequested()));
	connect(beamOffButton_, SIGNAL(clicked()), this, SIGNAL(beamOffRequested()));

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
	pushButton->setMaximumSize(QSize(80, 16777215));
	pushButton->setCheckable(true);
	pushButton->setAutoExclusive(true);
	pushButton->setText(text);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
	pushButton->setSizePolicy(sizePolicy);

	return pushButton;
}
