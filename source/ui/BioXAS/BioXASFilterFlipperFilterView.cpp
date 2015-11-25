#include "BioXASFilterFlipperFilterView.h"
#include "ui/util/AMPeriodicTableDialog.h"

BioXASFilterFlipperFilterView::BioXASFilterFlipperFilterView(BioXASFilterFlipperFilter *filter, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filter_ = 0;

	// Create UI elements.

	elementLineEdit_ = new QLineEdit();
	thicknessBox_ = new QSpinBox();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(elementLineEdit_);
	layout->addWidget(thicknessBox_);

	setLayout(layout);

	// Make connections.

	connect( elementLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onElementButtonClicked()) );
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
	elementLineEdit_->setText("");
	thicknessBox_->clear();
}

void BioXASFilterFlipperFilterView::update()
{
	updateElementLineEdit();
	updateThicknessBox();
}

void BioXASFilterFlipperFilterView::refresh()
{
	// Clear the view.

	clear();

	// Set up view elements.

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
			connect( filter_, SIGNAL(elementSymbolChanged(QString)), this, SLOT(updateElementLineEdit()) );
			connect( filter_, SIGNAL(thicknessChanged(double)), this, SLOT(updateThicknessBox()) );
		}

		refresh();

		emit filterChanged(filter_);
	}
}

void BioXASFilterFlipperFilterView::updateElementLineEdit()
{
	if (filter_ && filter_->isValid())
		elementLineEdit->setText(filter_->elementSymbol());
	else
		elementLineEdit_->setText("");
}

void BioXASFilterFlipperFilterView::updateThicknessBox()
{
	if (filter_ && filter_->isValid())
		thicknessBox_->setValue(int(filter_->thickness()));

	else
		thicknessBox_->setValue(0);
}

void BioXASFilterFlipperFilterView::onElementLineEditChanged()
{
	if (filter_)
		filter_->setElementSymbol(elementLineEdit_->text());
}

void BioXASFilterFlipperFilterView::onThicknessBoxValueChanged()
{
	if (filter_)
		filter_->setThickness(thicknessBox_->value());
}
