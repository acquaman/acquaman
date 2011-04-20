#include "VESPERSXRFDetectorView.h"
#include "ui/VESPERS/VESPERSXRFElementView.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

VESPERSXRFDetectorView::VESPERSXRFDetectorView(XRFDetector *detector, QWidget *parent)
	: QWidget(parent)
{

	if (!detector)
		return;

	detector_ = detector;
	detectorView_ = new XRFDetailedDetectorView(detector_);

	QFont font(this->font());
	font.setBold(true);

	QLabel *legend = new QLabel(QString("Legend"));
	legend->setFont(font);
	QLabel *kLines = new QLabel(QString("K-lines"));
	kLines->setFont(font);
	kLines->setStyleSheet("QLabel { background-color: rgb(0,255,0) ; border-width: 2px ; border-style: solid }");
	QLabel *lLines = new QLabel(QString("L-lines"));
	lLines->setFont(font);
	lLines->setStyleSheet("QLabel { background-color: rgb(255,255,0) ; border-width: 2px ; border-style: solid }");
	QLabel *mLines = new QLabel(QString("M-lines"));
	mLines->setFont(font);
	mLines->setStyleSheet("QLabel { background-color: rgb(0,255,255) ; border-width: 2px ; border-style: solid }");

	tableView_ = new AMPeriodicTableView;
	tableView_->setMaximumWidth(600);
	VESPERSXRFElementView *elView = new VESPERSXRFElementView(AMPeriodicTable::table()->elementBySymbol("Fe"));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), elView, SLOT(setElement(AMElement*)));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), detectorView_, SLOT(showEmissionLines(AMElement*)));
	connect(elView, SIGNAL(addROI(AMElement*,QPair<QString,QString>)), this, SLOT(addRegionOfInterest(AMElement*,QPair<QString,QString>)));
	connect(elView, SIGNAL(removeROI(AMElement*,QPair<QString,QString>)), this, SLOT(removeRegionOfInterest(AMElement*,QPair<QString,QString>)));

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addLayout(legendLayout);
	tableLayout->addWidget(tableView_, Qt::AlignLeft);
	tableLayout->addWidget(elView);

	QGroupBox *table = new QGroupBox;
	table->setLayout(tableLayout);
	table->setFlat(true);
	table->setFixedSize(900, 325);

	QVBoxLayout *xrfLayout = new QVBoxLayout;
	xrfLayout->addWidget(detectorView_);
	xrfLayout->addWidget(table, 0, Qt::AlignCenter);

	setLayout(xrfLayout);
}

void VESPERSXRFDetectorView::addRegionOfInterest(AMElement *el, QPair<QString, QString> line)
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

void VESPERSXRFDetectorView::removeRegionOfInterest(AMElement *el, QPair<QString, QString> line)
{
	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());
	palette.setColor(QPalette::Button, this->palette().color(QPalette::Button));
	tableView_->button(el)->setPalette(palette);
}
