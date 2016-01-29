#include "BioXASFilterFlipperFilterView.h"
#include "ui/util/AMPeriodicTableDialog.h"

BioXASFilterFlipperFilterView::BioXASFilterFlipperFilterView(BioXASFilterFlipperFilter *filter, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filter_ = 0;

	// Create UI elements.

	elementLineEdit_ = new QLineEdit();
	elementLineEdit_->setAlignment(Qt::AlignCenter);

	thicknessBox_ = new QSpinBox();
	thicknessBox_->setAlignment(Qt::AlignCenter);
	thicknessBox_->setValue(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(elementLineEdit_);
	layout->addWidget(thicknessBox_);

	setLayout(layout);

	// Make connections.

	connect( elementLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onElementLineEditChanged()) );
	connect( thicknessBox_, SIGNAL(valueChanged(int)), this, SLOT(onThicknessBoxValueChanged()) );

	// Current settings.

	setFilter(filter);
	refresh();
}

BioXASFilterFlipperFilterView::~BioXASFilterFlipperFilterView()
{

}

void BioXASFilterFlipperFilterView::refresh()
{
	// Clear the view.

	clear();

	// Set up view elements.

	thicknessBox_->blockSignals(true);
	thicknessBox_->setMinimum(0);
	thicknessBox_->setMaximum(10);
	thicknessBox_->setValue(0);
	thicknessBox_->blockSignals(false);

	// Update view elements.

	updateElementLineEdit();
	updateThicknessBox();
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

void BioXASFilterFlipperFilterView::clear()
{
	elementLineEdit_->setText("");
	thicknessBox_->clear();
}

void BioXASFilterFlipperFilterView::updateElementLineEdit()
{
	QString text;

	if (filter_)
		text = filter_->elementSymbol();

	elementLineEdit_->setText(text);
}

void BioXASFilterFlipperFilterView::updateThicknessBox()
{
	double thickness = 0;

	if (filter_)
		thickness = filter_->thickness();

	thicknessBox_->setValue(int(thickness));
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
