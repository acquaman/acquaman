#include "AMActionInfoDescriptionView.h"

AMActionInfoDescriptionView::AMActionInfoDescriptionView(AMActionInfo3 *info, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	info_ = 0;

	// Create UI elements.

	descriptionLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(descriptionLabel_);

	setLayout(layout);

	// Current settings.

	setInfo(info);
}

AMActionInfoDescriptionView::~AMActionInfoDescriptionView()
{

}

void AMActionInfoDescriptionView::clear()
{
	descriptionLabel_->clear();
}

void AMActionInfoDescriptionView::refresh()
{
	// Clears the view.

	clear();

	// Update view elements.

	updateDescriptionLabel();
}

void AMActionInfoDescriptionView::setInfo(AMActionInfo3 *newInfo)
{
	if (info_ != newInfo) {

		if (info_)
			disconnect( info_, 0, this, 0 );

		info_ = newInfo;

		if (info_)
			connect( info_, SIGNAL(infoChanged()), this, SLOT(updateDescriptionLabel()) );

		refresh();

		emit infoChanged(info_);
	}
}

void AMActionInfoDescriptionView::updateDescriptionLabel()
{
	QString descriptionText = "No action set.";

	if (info_)
		descriptionText = info_->longDescription();

	descriptionLabel_->setText(descriptionText);
}


