#include "BioXASBeamStatusView.h"
#include "beamline/CLS/CLSShutters.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	beamStatus_ = 0;

	selectedComponentView_ = 0;

	// Create beam status editor.

	beamStatusEditor_ = new CLSControlEditor(0);
	beamStatusEditor_->setObjectName("BioXASBeamStatusEditor");

	// Create the button bar.

	buttonBar_ = new BioXASBeamStatusButtonBar(0);

	connect( buttonBar_, SIGNAL(selectedComponentChanged(AMControl*)), this, SIGNAL(selectedComponentChanged(AMControl*)) );
	connect( buttonBar_, SIGNAL(selectedComponentChanged(AMControl*)), this, SLOT(updateSelectedComponentView()) );

	QHBoxLayout *buttonBarLayout = new QHBoxLayout();
	buttonBarLayout->addStretch();
	buttonBarLayout->addWidget(buttonBar_);
	buttonBarLayout->addStretch();

	// Create selected component view.

	selectedComponentBoxLayout_ = new QVBoxLayout();

	QVBoxLayout *componentsBoxLayout = new QVBoxLayout();
	componentsBoxLayout->addLayout(buttonBarLayout);
	componentsBoxLayout->addLayout(selectedComponentBoxLayout_);

	QGroupBox *componentsBox = new QGroupBox();
	componentsBox->setObjectName("BioXASBeamStatusComponentsBox");
	componentsBox->setLayout(componentsBoxLayout);

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(componentsBox);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::setBeamStatus(BioXASBeamStatus *newBeamStatus)
{
	if (beamStatus_ != newBeamStatus) {
		beamStatus_ = newBeamStatus;

		emit beamStatusChanged(beamStatus_);
	}

	updateBeamStatusView();
}

void BioXASBeamStatusView::setSelectedComponent(AMControl *newSelection)
{
	buttonBar_->setSelectedComponent(newSelection);
	updateSelectedComponentView();
}

void BioXASBeamStatusView::updateBeamStatusView()
{
	updateBeamStatusEditor();
	updateButtonBar();
	updateSelectedComponentView();
}

void BioXASBeamStatusView::updateBeamStatusEditor()
{
	beamStatusEditor_->setControl(beamStatus_);
}

void BioXASBeamStatusView::updateButtonBar()
{
	buttonBar_->setBeamStatus(beamStatus_);
}

void BioXASBeamStatusView::updateSelectedComponentView()
{
	// Clear the selected component view.

	if (selectedComponentView_) {
		selectedComponentBoxLayout_->removeWidget(selectedComponentView_);
		selectedComponentView_->disconnect();
		selectedComponentView_->deleteLater();
	}

	// Update the current component view.

	selectedComponentView_ = createComponentView( getSelectedComponent() );

	if (selectedComponentView_)
		selectedComponentBoxLayout_->addWidget(selectedComponentView_);
}

QWidget* BioXASBeamStatusView::createComponentView(AMControl *component)
{
	QWidget *result = 0;

	if (component) {

		// Shutters view.

		CLSShutters *shutters = qobject_cast<CLSShutters*>(component);
		if (!result && shutters) {

			QVBoxLayout *shuttersViewLayout = new QVBoxLayout();

			foreach (AMControl *shutter, shutters->shuttersList())
				shuttersViewLayout->addWidget(new CLSControlEditor(shutter));

			QWidget *shuttersView = new QWidget();
			shuttersView->setLayout(shuttersViewLayout);

			result = shuttersView;
		}

		// Anything else, for now.

		if (!result)
			result = new CLSControlEditor(component);
	}

	return result;
}

AMControl* BioXASBeamStatusView::getSelectedComponent() const
{
	return buttonBar_->selectedComponent();
}
