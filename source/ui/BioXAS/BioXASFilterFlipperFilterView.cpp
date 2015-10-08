#include "BioXASFilterFlipperFilterView.h"
#include "ui/util/AMPeriodicTableDialog.h"

BioXASFilterFlipperFilterView::BioXASFilterFlipperFilterView(BioXASFilterFlipperFilter *filter, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filter_ = 0;

	// Create UI elements.

	defaultLabel_ = new QLabel("None");
	elementButton_ = new QToolButton();
	thicknessBox_ = new QSpinBox();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(defaultLabel_);
	layout->addWidget(elementButton_);
	layout->addWidget(thicknessBox_);

	setLayout(layout);

	// Make connections.

	connect( elementButton_, SIGNAL(clicked()), this, SLOT(onElementButtonClicked()) );
	connect( thicknessBox_, SIGNAL(valueChanged(int)), this, SLOT(onThicknessBoxValueChanged()) );

	// Current settings.

	setFilter(filter);
	refresh();
}

BioXASFilterFlipperFilterView::~BioXASFilterFlipperFilterView()
{

}

void BioXASFilterFlipperFilterView::clear()
{
	defaultLabel_->clear();
	elementButton_->setText("");
	thicknessBox_->clear();
}

void BioXASFilterFlipperFilterView::update()
{
	updateDefaultLabel();
	updateElementButton();
	updateThicknessBox();
}

void BioXASFilterFlipperFilterView::refresh()
{
	// Clear the view.

	clear();

	// Set up view elements.

	defaultLabel_->setText("None");

	thicknessBox_->setMinimum(0);
	thicknessBox_->setMaximum(10);
	thicknessBox_->setValue(0);

	// Update the view.

	update();
}

void BioXASFilterFlipperFilterView::setFilter(BioXASFilterFlipperFilter *newFilter)
{
	if (filter_ != newFilter) {

		if (filter_) {
			disconnect( filter_, 0, this, 0 );
		}

		filter_ = newFilter;

		if (filter_) {
			connect( filter_, SIGNAL(elementChanged(AMElement*)), this, SLOT(updateElementButton()) );
			connect( filter_, SIGNAL(thicknessChanged(double)), this, SLOT(updateThicknessBox()) );
		}

		refresh();

		emit filterChanged(filter_);
	}
}

void BioXASFilterFlipperFilterView::updateDefaultLabel()
{
	if (filter_)
		defaultLabel_->hide();
	else
		defaultLabel_->show();
}

void BioXASFilterFlipperFilterView::updateElementButton()
{
	if (filter_) {

		// If there is a filter, the button text should match the element symbol.

		if (filter_->element())
			elementButton_->setText(filter_->element()->symbol());
		else
			elementButton_->setText("");

		elementButton_->show();

	} else {

		// If there is no filter, the button should be hidden.

		elementButton_->hide();
	}

}

void BioXASFilterFlipperFilterView::updateThicknessBox()
{
	if (filter_) {

		// If there is a filter, the box value should match the filter's thickness.

		thicknessBox_->setValue(int(filter_->thickness()));
		thicknessBox_->show();

	} else {

		// If there is no filter, the box should be hidden.

		thicknessBox_->hide();
	}
}

void BioXASFilterFlipperFilterView::updateEnabledBox()
{
	if (filter_) {

		// If there is a filter, the box should be checked.

		enabledBox_->setChecked(true);

	} else {

		// If there is no filter, the box should be unchecked.

		enabledBox_->setChecked(false);
	}
}

void BioXASFilterFlipperFilterView::onElementButtonClicked()
{
	if (filter_) {
		// Identify the user's new element selection.

		AMElement *newElement = AMPeriodicTableDialog::getElement(this);

		// Set the filter's element.

		filter_->setElement(newElement);
	}
}

void BioXASFilterFlipperFilterView::onThicknessBoxValueChanged()
{
	if (filter_) {
		filter_->setThickness(thicknessBox_->value());
	}
}
