#include "AMSlitsView.h"
#include "beamline/AMSlits.h"
#include "ui/beamline/AMSlitView.h"

AMSlitsView::AMSlitsView(AMSlits *slits, bool showBladeEditors, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	slits_ = 0;

	// Create vertical slit view.

	verticalSlitView_ = new AMSlitView(0);

	QVBoxLayout *verticalBoxLayout = new QVBoxLayout();
	verticalBoxLayout->addWidget(verticalSlitView_);

	QGroupBox *verticalBox = new QGroupBox("Vertical slit");
	verticalBox->setLayout(verticalBoxLayout);

	// Create horizontal slit view.

	horizontalSlitView_ = new AMSlitView(0);

	QVBoxLayout *horizontalBoxLayout = new QVBoxLayout();
	horizontalBoxLayout->addWidget(horizontalSlitView_);

	QGroupBox *horizontalBox = new QGroupBox("Horizontal slit");
	horizontalBox->setLayout(horizontalBoxLayout);

	// Create and set main layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(verticalBox);
	layout->addWidget(horizontalBox);

	setLayout(layout);

	// Current settings.

	setControl(slits);
	setBladeEditorsShown(showBladeEditors);
	refresh();
}

AMSlitsView::~AMSlitsView()
{

}

void AMSlitsView::refresh()
{
	if (slits_) {
		verticalSlitView_->setControl(slits_->verticalSlit());
		horizontalSlitView_->setControl(slits_->horizontalSlit());
	} else {
		verticalSlitView_->setControl(0);
		horizontalSlitView_->setControl(0);
	}
}

void AMSlitsView::setControl(AMSlits *newControl)
{
	if (slits_ != newControl) {
		slits_ = newControl;
		refresh();

		emit controlChanged(slits_);
	}
}

void AMSlitsView::setBladeEditorsShown(bool showEditors)
{
	verticalSlitView_->setBladeEditorsShown(showEditors);
	horizontalSlitView_->setBladeEditorsShown(showEditors);
}


