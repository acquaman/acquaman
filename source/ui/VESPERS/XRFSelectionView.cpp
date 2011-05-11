#include "XRFSelectionView.h"

#include <QGroupBox>
#include <QPalette>

XRFSelectionView::XRFSelectionView(double minimumEnergy, double maximumEnergy, QWidget *parent)
	: QWidget(parent)
{
	minimumEnergy_ = minimumEnergy;
	maximumEnergy_ = maximumEnergy;

	tableView_ = new XRFPeriodicTableView(minimumEnergy, maximumEnergy);
	QPalette palette = tableView_->palette();
	palette.setColor(QPalette::Window, QColor(79, 148, 205));
	tableView_->setPalette(palette);
	tableView_->setAutoFillBackground(true);

	elView_ = new VESPERSXRFElementView(AMPeriodicTable::table()->elementBySymbol("Fe"), minimumEnergy_, maximumEnergy_);
	palette = elView_->palette();
	palette.setColor(QPalette::Window, QColor(110, 139, 61));
	elView_->setPalette(palette);
	elView_->setAutoFillBackground(true);

	// This signal takes an element that was clicked inside the view and transfers it to the element view.  It puts checks where appropriate.
	connect(tableView_, SIGNAL(elementClicked(AMElement*,QList<QPair<int,QString> >)), elView_, SLOT(setElement(AMElement*,QList<QPair<int,QString> >)));
	// This signal is used to pass on what the last element that has been clicked on.
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SIGNAL(elementSelected(AMElement*)));

	// Passes on that an emission line from the given element has been selected to be added.  The table will check if it's valid or not.
	connect(elView_, SIGNAL(addROI(AMElement*,QPair<QString,QString>)), tableView_, SLOT(regionOfInterestAdded(AMElement*,QPair<QString,QString>)));
	// Passes on that a region of interest was successfully added.
	connect(tableView_, SIGNAL(addRegionOfInterest(AMElement*,QPair<QString,QString>)), this, SIGNAL(addRegionOfInterest(AMElement*,QPair<QString,QString>)));

	// Passes on that an emission line from the given element has been selected to be removed.  The table will check if it's valid or not.
	connect(elView_, SIGNAL(removeROI(AMElement*,QPair<QString,QString>)), tableView_, SLOT(regionOfInterestRemoved(AMElement*,QPair<QString,QString>)));
	// Passes on that a region of interest was successfully removed.
	connect(tableView_, SIGNAL(removeRegionOfInterest(AMElement*,QPair<QString,QString>)), this, SIGNAL(removeRegionOfInterest(AMElement*,QPair<QString,QString>)));

	connect(tableView_, SIGNAL(clearAllRegionsOfInterest()), this, SLOT(onClearList()));

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addWidget(tableView_, Qt::AlignLeft);
	tableLayout->addWidget(elView_, Qt::AlignCenter);

	QGroupBox *table = new QGroupBox;
	table->setLayout(tableLayout);
	table->setFlat(true);
	table->setFixedSize(900, 325);

	QVBoxLayout *xrfLayout = new QVBoxLayout;
	xrfLayout->addWidget(table, 0, Qt::AlignCenter);

	setLayout(xrfLayout);
}

void XRFSelectionView::onClearList()
{
	elView_->setElement(elView_->element(), QList<QPair<int, QString> >());
	emit clearAllRegionsOfInterest();
}
