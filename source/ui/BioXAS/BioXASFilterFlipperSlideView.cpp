#include "BioXASFilterFlipperSlideView.h"

BioXASFilterFlipperSlideView::BioXASFilterFlipperSlideView(BioXASFilterFlipperSlide *slide, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	slide_ = 0;

	// Create UI elements.

	filterView_ = new BioXASFilterFlipperFilterView(0, this);
	enabledBox_ = new QCheckBox(this);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(filterView_);
	layout->addWidget(enabledBox_);

	setLayout(layout);

	// Make connections.

	connect( enabledBox_, SIGNAL(clicked()), this, SLOT(onEnabledBoxClicked()) );

	// Current settings.

	setSlide(slide);
	refresh();
}

BioXASFilterFlipperSlideView::~BioXASFilterFlipperSlideView()
{

}

void BioXASFilterFlipperSlideView::clear()
{
	filterView_->clear();

	enabledBox_->blockSignals(true);
	enabledBox_->setChecked(false);
	enabledBox_->blockSignals(false);
}

void BioXASFilterFlipperSlideView::update()
{
	updateFilterView();
	updateEnabledBox();
}

void BioXASFilterFlipperSlideView::refresh()
{
	// Clear the view.

	clear();

	// Set up UI elements.

	filterView_->setMinimumWidth(100);

	// Update the view.

	update();
}

void BioXASFilterFlipperSlideView::setSlide(BioXASFilterFlipperSlide *newSlide)
{
	if (slide_ != newSlide) {

		if (slide_) {
			filterView_->setFilter(0);

			disconnect( slide_, 0, this, 0 );
		}

		slide_ = newSlide;

		if (slide_) {
			filterView_->setFilter(slide_->filter());

			connect( slide_, SIGNAL(filterChanged(BioXASFilterFlipperFilter*)), this, SLOT(updateFilterView()) );
			connect( slide_, SIGNAL(enabledChanged(bool)), this, SLOT(updateEnabledBox()) );
		}

		refresh();

		emit slideChanged(slide_);
	}
}

void BioXASFilterFlipperSlideView::updateFilterView()
{
	filterView_->update();
}

void BioXASFilterFlipperSlideView::updateEnabledBox()
{
	enabledBox_->blockSignals(true);

	if (slide_)
		enabledBox_->setChecked(slide_->enabled());
	else
		enabledBox_->setChecked(false);

	enabledBox_->blockSignals(false);
}

void BioXASFilterFlipperSlideView::onEnabledBoxClicked()
{
	if (slide_) {
		slide_->setEnabled(enabledBox_->isChecked());
	}
}
