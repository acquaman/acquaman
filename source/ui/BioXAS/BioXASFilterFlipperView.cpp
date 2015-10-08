#include "BioXASFilterFlipperView.h"

BioXASFilterFlipperView::BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create UI elements.

	configurationView_ = new BioXASFilterFlipperConfigurationView(0, this);

	QLabel *currentFilterPrompt = new QLabel("Current filter: ");
	currentFilterBox_ = new QComboBox(this);

	QLabel *statusPrompt = new QLabel("Status: ");
	statusLabel_ = new QLabel();

	// Create and set layouts.

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addStretch();
	leftLayout->addWidget(configurationView_);
	leftLayout->addStretch();

	QGridLayout *propertiesLayout = new QGridLayout();
	propertiesLayout->addWidget(currentFilterPrompt, 0, 0, 1, 1, Qt::AlignRight);
	propertiesLayout->addWidget(currentFilterBox_, 0, 1);
	propertiesLayout->addWidget(statusPrompt, 1, 0, 1, 1, Qt::AlignRight);
	propertiesLayout->addWidget(statusLabel_, 1, 1);

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->setMargin(0);
	rightLayout->addStretch();
	rightLayout->addLayout(propertiesLayout);
	rightLayout->addStretch();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(leftLayout);
	layout->addLayout(rightLayout);

	setLayout(layout);

	// Current settings.

	setFilterFlipper(filterFlipper);
}

BioXASFilterFlipperView::~BioXASFilterFlipperView()
{

}

void BioXASFilterFlipperView::clear()
{
	currentFilterBox_->clear();
	statusLabel_->clear();
}

void BioXASFilterFlipperView::update()
{
	updateCurrentFilterBox();
	updateStatusLabel();
}

void BioXASFilterFlipperView::refresh()
{
	// Clear the view.

	clear();

	// Setup view elements.

	currentFilterBox_->addItem("None");

	// The 'current filter' box is indexed 1 --> n (with zero being reserved for 'None'.
	// The filter flipper slides are indexed 0 --> n-1.
	// The filter flipper slide index is saved as a QVariant for the entry's item data
	// for easy reference later.

	if (filterFlipper_) {
		QList<BioXASFilterFlipperSlide*> slides = filterFlipper_->slides();

		for (int i = 0; i < slides.count(); i++) {
			BioXASFilterFlipperSlide *slide = slides.at(i);

			if (slide && slide->hasFilter() && slide->enabled())
				currentFilterBox_->addItem(slide->filter()->name(), QVariant(i));
		}
	}

	// Update the view.

	update();
}

void BioXASFilterFlipperView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {

		if (filterFlipper_) {
			configurationView_->setFilterFlipper(0);

			disconnect( filterFlipper_, 0, this, 0 );
		}

		filterFlipper_ = newFlipper;


		if (filterFlipper_) {
			configurationView_->setFilterFlipper(filterFlipper_);

			foreach (BioXASFilterFlipperSlide *slide, filterFlipper_->slides()) {
				connect( slide, SIGNAL(filterChanged(BioXASFilterFlipperFilter*)), this, SLOT(refresh()) );
				connect( slide, SIGNAL(enabledChanged(bool)), this, SLOT(refresh()) );
			}
		}

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}

void BioXASFilterFlipperView::updateCurrentFilterBox()
{
	if (filterFlipper_ && filterFlipper_->currentSlide()) {

		// Identify the box index corresponding to the current filter index.

		int currentSlideIndex = int(filterFlipper_->currentSlideIndex());

		int boxIndex = 0;

		for (int i = 0; i < currentFilterBox_->count() && boxIndex == 0; i++) {
			int slideIndex = currentFilterBox_->itemData(i).toInt();

			if (slideIndex == currentSlideIndex)
				boxIndex = i;
		}

		// Set the filter at the given index to be the current box choice.

		currentFilterBox_->setCurrentIndex(boxIndex);
	}
}

void BioXASFilterFlipperView::updateStatusLabel()
{
	if (filterFlipper_ && filterFlipper_->statusControl()) {
		QString status = filterFlipper_->statusToString(int(filterFlipper_->statusControl()->value()));
		statusLabel_->setText(status);
	}
}

void BioXASFilterFlipperView::updateFilterFlipperCurrentSlide()
{
	if (filterFlipper_ && filterFlipper_->currentSlideControl()) {

		// Identify the filter index corresponding to the current box index.

		int currentFilterIndex = currentFilterBox_->itemData(currentFilterBox_->currentIndex()).toInt();
		filterFlipper_->currentSlideControl()->move(currentFilterIndex);
	}
}
