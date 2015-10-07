#include "BioXASFilterFlipperView.h"

BioXASFilterFlipperView::BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create UI elements.

	configurationView_ = new BioXASFilterFlipperConfigurationView(this);

	QLabel *currentFilterPrompt = new QLabel("Current filter: ");
	currentFilterBox_ = new QComboBox(this);

	QLabel *statusPrompt = new QLabel("Status: ");
	statusLabel_ = new QLabel();

	// Create and set layouts.

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addWidget(configurationView_);

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

	if (filterFlipper_) {
		QList<BioXASFilterFlipperFilter*> filters = filterFlipper_->filters();

		for (int i = 0; i < filters.count(); i++) {
			BioXASFilterFlipperFilter *filter = filters.at(i);

			if (filter)
				currentFilterBox_->addItem(filter->name(), QVariant(i));
		}
	}

	// Update the view.

	update();
}

void BioXASFilterFlipperView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {

		if (filterFlipper_) {
			disconnect( filterFlipper_, 0, this, 0 );
		}

		filterFlipper_ = newFlipper;

		if (filterFlipper_ && filterFlipper_->currentSlideControl()) {
			connect( filterFlipper_->currentSlideControl(), SIGNAL(valueChanged(double)), this, SLOT(updateCurrentFilterBox()) );
		}

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}

void BioXASFilterFlipperView::updateCurrentFilterBox()
{
	if (filterFlipper_ && filterFlipper_->currentSlideControl()) {

		// Identify the box index corresponding to the current filter index.

		int currentFilterIndex = int(filterFlipper_->currentSlideControl()->value());

		int boxIndex = 0;

		for (int i = 0; i < currentFilterBox_->count() && boxIndex == 0; i++) {
			int filterIndex = currentFilterBox_->itemData(i).toInt();

			if (filterIndex == currentFilterIndex)
				boxIndex = i;
		}

		// Set the filter's name to be the current box choice.

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
