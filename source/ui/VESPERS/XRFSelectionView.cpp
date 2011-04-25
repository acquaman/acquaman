#include "XRFSelectionView.h"

#include <QGroupBox>

XRFSelectionView::XRFSelectionView(double minimumEnergy, double maximumEnergy, QWidget *parent)
	: QWidget(parent)
{
	minimumEnergy_ = minimumEnergy;
	maximumEnergy_ = maximumEnergy;

	tableView_ = new XRFPeriodicTableView(minimumEnergy, maximumEnergy);
	elView_ = new VESPERSXRFElementView(AMPeriodicTable::table()->elementBySymbol("Fe"), minimumEnergy_, maximumEnergy_);

	connect(tableView_, SIGNAL(elementClicked(AMElement*,QList<QPair<int,QString> >)), elView_, SLOT(setElement(AMElement*,QList<QPair<int,QString> >)));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SIGNAL(elementSelected(AMElement*)));
	//connect(elView_, SIGNAL(addROI(AMElement*,QPair<QString,QString>)), this, SIGNAL(addRegionOfInterest(AMElement*,QPair<QString,QString>)));
	connect(elView_, SIGNAL(addROI(AMElement*,QPair<QString,QString>)), tableView_, SLOT(regionOfInterestAdded(AMElement*,QPair<QString,QString>)));
	//connect(elView_, SIGNAL(removeROI(AMElement*,QPair<QString,QString>)), this, SIGNAL(removeRegionOfInterest(AMElement*,QPair<QString,QString>)));
	connect(elView_, SIGNAL(removeROI(AMElement*,QPair<QString,QString>)), tableView_, SLOT(regionOfInterestRemoved(AMElement*,QPair<QString,QString>)));

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
