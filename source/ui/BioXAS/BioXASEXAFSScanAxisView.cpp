#include "BioXASEXAFSScanAxisView.h"

BioXASEXAFSScanAxisView::BioXASEXAFSScanAxisView(AMScanAxis *scanAxis, const QString &title, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	scanAxis_ = 0;
	regionsLocked_ = true;

	// Create UI elements.

	titleLabel_ = new QLabel();

	lockRegionsButton_ = new QToolButton();

	addRegionButton_ = new QToolButton();
	addRegionButton_->setIcon(QIcon(":22x22/list-add.png"));

	deleteButtonsGroup_ = new QButtonGroup();

	regionsView_ = 0;

	// Create and set layouts.

	regionsLayout_ = new QVBoxLayout();

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->addWidget(titleLabel);
	topLayout->addStretch();
	topLayout->addWidget(lockRegionsButton_);
	topLayout->addWidget(addRegionButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(topLayout);
	layout->addLayout(regionsLayout_);

	setLayout(layout);

	// Make connections.

	connect( addRegionButton_, SIGNAL(clicked()), this, SLOT(onAddRegionButtonClicked()) );
	connect( lockRegionsButton_, SIGNAL(toggled(bool)), this, SLOT(onLockRegionsToggled(bool)) );

	// Current settings.

	setTitle(title);
	setScanAxis(scanAxis);
}

BioXASEXAFSScanAxisView::~BioXASEXAFSScanAxisView()
{

}

void BioXASEXAFSScanAxisView::setTitle(const QString &newTitle)
{
	titleLabel_->setText(newTitle);
}

void BioXASEXAFSScanAxisView::setScanAxis(AMScanAxis *newAxis)
{
	if (scanAxis_ != newAxis) {

		if (scanAxis_) {

		}

		scanAxis_ = newAxis;

		if (scanAxis_) {
			connect( scanAxis_, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(refresh()) );
			connect( scanAxis_, SIGNAL(regionOverwritten(AMScanAxisRegion*)), this, SLOT(refresh()) );
			connect( scanAxis_, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(refresh()) );
			connect( scanAxis_, SIGNAL(regionsChanged()), this, SLOT(update()) );
		}

		refresh();

		emit scanAxisChanged(scanAxis_);
	}
}

void BioXASEXAFSScanAxisView::clear()
{
	clearRegionsView();
}

void BioXASEXAFSScanAxisView::update()
{
	updateLockRegionsButton();
	updateAddRegionButton();
}

void BioXASEXAFSScanAxisView::refresh()
{
	// Clear the view.

	clear();

	// Create new regions view.

	QWidget *regionsView

	// Update the view.

	update();
}

void BioXASEXAFSScanAxisView::setRegionsLocked(bool locked)
{
	if (regionsLocked_ != locked) {
		regionsLocked_ = locked;

		updateLockRegionsButton();
	}
}

void BioXASEXAFSScanAxisView::clearRegionsView()
{
	// Clear all views from the list of region views.

	foreach (QWidget *regionView, regionViews_) {
		regionViews_.removeOne(regionView);
	}

	// Clear all delete buttons from the buttons group.

	for (int buttonID = 0, buttonCount = deleteButtonsGroup_->buttons().count(); buttonID < buttonCount; buttonID++) {
		QAbstractButton *button = deleteButtonsGroup_->button(buttonID);
		deleteButtonsGroup_->removeButton(button);
	}

	// Delete the main regions view, and all of its children.

	if (regionsView_) {
		regionsLayout_->removeWidget(regionsView_);

		regionsView_->deleteLater();
		regionsView_ = 0;
	}
}

void BioXASEXAFSScanAxisView::updateLockRegionsButton()
{
	bool enabled = false;
	QString iconName;
	QString tooltip;

	if (scanAxis_)
		enabled = true;

	if (regionsLocked_) {
		iconName = ":/22x22/lock.png";
		tooltip = "Unlock the regions from each other.";
	} else {
		iconName = ":/22x22/unlock.png";
		tooltip = "Lock regions together.";
	}

	lockRegionsButton_->setEnabled(enabled);
	lockRegionsButton_->setIcon(iconName);
	lockRegionsButton_->setToolTip(tooltip);
}

void BioXASEXAFSScanAxisView::updateAddRegionButton()
{
	bool enabled = false;

	if (scanAxis_)
		enabled = true;

	addRegionButton_->setEnabled(enabled);
}

void BioXASEXAFSScanAxisView::updateRegionsView()
{

}

QWidget* BioXASEXAFSScanAxisView::createRegionsView(const QList<AMScanAxisEXAFSRegion *> &regions)
{
	QWidget *view = 0;

	if (!regions.isEmpty()) {

		QVBoxLayout *regionsLayout = new QVBoxLayout();

		// Create a view for every region listed, add view to main view and to list of region views.

		foreach (AMScanAxisEXAFSRegion *region, regions) {
			QWidget *regionView = createEXAFSRegionView(region);
			regionsLayout->addWidget(regionView);
		}

		view = new QWidget();
		view->setLayout(regionsLayout);
	}
}

AMEXAFSScanAxisElementView* BioXASEXAFSScanAxisView::createEXAFSRegionView(AMScanAxisEXAFSRegion *exafsRegion)
{
	AMEXAFSScanAxisElementView *result = 0;

	if (region)
		result = new AMEXAFSScanAxisElementView(exafsRegion);

	return result;
}

QWidget* BioXASEXAFSScanAxisView::regionViewByButton(QAbstractButton *deleteButton)
{
	QWidget *view = 0;

	if (deleteButton) {
		int buttonID = deleteButtonsGroup_->id(deleteButton);

		if (buttonID != -1)
			view = regionViews_.at(buttonID);
	}

	return view;
}
