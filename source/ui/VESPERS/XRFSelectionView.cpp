#include "XRFSelectionView.h"

#include <QGroupBox>

XRFSelectionView::XRFSelectionView(QWidget *parent) :
	QWidget(parent)
{
	tableView_ = new XRFPeriodicTableView;

	VESPERSXRFElementView *elView = new VESPERSXRFElementView(AMPeriodicTable::table()->elementBySymbol("Fe"));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), elView, SLOT(setElement(AMElement*)));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SIGNAL(elementSelected(AMElement*)));
	connect(elView, SIGNAL(addROI(AMElement*,QPair<QString,QString>)), this, SLOT(addRegionOfInterest(AMElement*,QPair<QString,QString>)));
	connect(elView, SIGNAL(removeROI(AMElement*,QPair<QString,QString>)), this, SLOT(removeRegionOfInterest(AMElement*,QPair<QString,QString>)));

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addWidget(tableView_, Qt::AlignLeft);
	tableLayout->addWidget(elView);

	QGroupBox *table = new QGroupBox;
	table->setLayout(tableLayout);
	table->setFlat(true);
	table->setFixedSize(900, 325);

	QVBoxLayout *xrfLayout = new QVBoxLayout;
	xrfLayout->addWidget(table, 0, Qt::AlignCenter);

	setLayout(xrfLayout);
}

void XRFSelectionView::addRegionOfInterest(AMElement *el, QPair<QString, QString> line)
{

	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());

	if (line.first.contains("K"))
		palette.setColor(QPalette::Button, Qt::green);
	else if (line.first.contains("L"))
		palette.setColor(QPalette::Button, Qt::yellow);
	else if (line.first.contains("M"))
		palette.setColor(QPalette::Button, Qt::cyan);

	clicked->setPalette(palette);
}

void XRFSelectionView::removeRegionOfInterest(AMElement *el, QPair<QString, QString> line)
{
	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());
	palette.setColor(QPalette::Button, this->palette().color(QPalette::Button));
	tableView_->button(el)->setPalette(palette);
}
