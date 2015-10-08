#include "BioXASFilterFlipperConfigurationView.h"

BioXASFilterFlipperConfigurationView::BioXASFilterFlipperConfigurationView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create and set layouts.

	layout_ = new QVBoxLayout();

	setLayout(layout_);

	// Current settings.

	setFilterFlipper(filterFlipper);
	refresh();
}

BioXASFilterFlipperConfigurationView::~BioXASFilterFlipperConfigurationView()
{

}

void BioXASFilterFlipperConfigurationView::clear()
{
	// Remove all widgets from the layout, clear mapping, and delete widgets.

	foreach (QWidget *view, indexedSlideViewMapping_.keys()) {
		layout_->removeWidget(view);
	}

	QList<QWidget*> indexedSlideViews = indexedSlideViewMapping_.keys();
	QList<BioXASFilterFlipperSlideView*> slideViews = indexedSlideViewMapping_.values();

	indexedSlideViewMapping_.clear();

	foreach (QWidget *widget, indexedSlideViews) {
		if (widget)
			widget->deleteLater();
	}

	foreach (BioXASFilterFlipperSlideView *slideView, slideViews) {
		if (slideView)
			slideView->deleteLater();
	}
}

void BioXASFilterFlipperConfigurationView::update()
{
	foreach (BioXASFilterFlipperSlideView *view, indexedSlideViewMapping_.values()) {
		if (view)
			view->update();
	}
}

void BioXASFilterFlipperConfigurationView::refresh()
{
	// Clears the view.

	clear();

	// Create UI elements.

	if (filterFlipper_) {
		QList<BioXASFilterFlipperSlide*> slides = filterFlipper_->slides();

		for (int i = 0; i < slides.count(); i++) {
			BioXASFilterFlipperSlide *slide = slides.at(i);

			BioXASFilterFlipperSlideView *slideView = createSlideView(slide);
			QWidget *view = createIndexedSlideView(i+1, slideView);

			indexedSlideViewMapping_.insert(view, slideView);
			layout_->addWidget(view);
		}
	}

	// Update the view.

	update();
}

void BioXASFilterFlipperConfigurationView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {
		filterFlipper_ = newFlipper;

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}

BioXASFilterFlipperSlideView* BioXASFilterFlipperConfigurationView::createSlideView(BioXASFilterFlipperSlide *slide)
{
	BioXASFilterFlipperSlideView *newView = 0;

	if (slide)
		newView = new BioXASFilterFlipperSlideView(slide, this);

	return newView;
}

QWidget* BioXASFilterFlipperConfigurationView::createIndexedSlideView(int index, BioXASFilterFlipperSlideView *slideView)
{
	QLabel *indexLabel = new QLabel(QString::number(index) + ": ");

	QHBoxLayout *viewLayout = new QHBoxLayout();
	viewLayout->setMargin(0);
	viewLayout->addWidget(indexLabel);
	viewLayout->addWidget(slideView);

	QWidget *view = new QWidget(this);
	view->setLayout(viewLayout);

	return view;
}
