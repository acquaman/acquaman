#include "AMSlitView.h"
#include "beamline/AMSlit.h"
#include "ui/beamline/AMExtendedControlEditor.h"

AMSlitView::AMSlitView(AMSlit *slit, bool showBladeEditors, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	slit_ = 0;
	showBladeEditors_ = false;

	// Create UI elements.

	gapEditor_ = new AMExtendedControlEditor(0);
	gapEditor_->setTitle("Gap");

	centerEditor_ = new AMExtendedControlEditor(0);
	centerEditor_->setTitle("Center");

	openButton_ = new QPushButton("Open");
	connect( openButton_, SIGNAL(clicked(bool)), this, SLOT(onOpenButtonClicked()) );

	closeButton_ = new QPushButton("Close");
	connect( closeButton_, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()) );

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(openButton_);
	buttonsLayout->addWidget(closeButton_);

	firstBladeEditor_ = new AMExtendedControlEditor(0);
	firstBladeEditor_->setReadOnlyPreference(true);

	secondBladeEditor_ = new AMExtendedControlEditor(0);
	secondBladeEditor_->setReadOnlyPreference(true);

	QHBoxLayout *bladesLayout = new QHBoxLayout();
	bladesLayout->addWidget(firstBladeEditor_);
	bladesLayout->addWidget(secondBladeEditor_);

	bladesBox_ = new QGroupBox();
	bladesBox_->setFlat(true);
	bladesBox_->setLayout(bladesLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(gapEditor_);
	layout->addWidget(centerEditor_);
	layout->addLayout(buttonsLayout);
	layout->addWidget(bladesBox_);

	setLayout(layout);

	// Current settings.

	setControl(slit);
	setBladeEditorsShown(showBladeEditors);

	refresh();
}

AMSlitView::~AMSlitView()
{

}

void AMSlitView::refresh()
{
	updateGapEditor();
	updateCenterEditor();
	updateOpenButton();
	updateClosedButton();
	updateBladeEditors();
}

void AMSlitView::setControl(AMSlit *newControl)
{
	if (slit_ != newControl) {

		if (slit_)
			disconnect( slit_, 0, this, 0 );

		slit_ = newControl;

		if (slit_) {
			connect( slit_, SIGNAL(firstBladeChanged(AMControl*)), this, SLOT(refresh()) );
			connect( slit_, SIGNAL(secondBladeChanged(AMControl*)), this, SLOT(refresh()) );
			connect( slit_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(slit_);
	}
}

void AMSlitView::setBladeEditorsShown(bool showEditors)
{
	if (showBladeEditors_ != showEditors) {
		showBladeEditors_ = showEditors;
		refresh();
	}
}

void AMSlitView::updateGapEditor()
{
	if (slit_)
		gapEditor_->setControl(slit_->gap());
	else
		gapEditor_->setControl(0);
}

void AMSlitView::updateCenterEditor()
{
	if (slit_)
		centerEditor_->setControl(slit_->center());
	else
		centerEditor_->setControl(0);
}

void AMSlitView::updateOpenButton()
{
	if (slit_ && slit_->canOpen())
		openButton_->show();
	else
		openButton_->hide();
}

void AMSlitView::updateClosedButton()
{
	if (slit_ && slit_->canClose())
		closeButton_->show();
	else
		closeButton_->hide();
}

void AMSlitView::updateBladeEditors()
{
	// Update the controls for each editor.

	if (slit_) {
		firstBladeEditor_->setControl(slit_->firstBlade());
		firstBladeEditor_->setTitle(slit_->firstBlade()->name());
		secondBladeEditor_->setControl(slit_->secondBlade());
		secondBladeEditor_->setTitle(slit_->secondBlade()->name());

	} else {
		firstBladeEditor_->setControl(0);
		firstBladeEditor_->setTitle("");
		secondBladeEditor_->setControl(0);
		secondBladeEditor_->setTitle("");
	}

	// Updates the editors visibility.

	if (showBladeEditors_)
		bladesBox_->show();
	else
		bladesBox_->hide();
}

void AMSlitView::onOpenButtonClicked()
{
	if (slit_)
		slit_->open();
}

void AMSlitView::onCloseButtonClicked()
{
	if (slit_)
		slit_->close();
}
