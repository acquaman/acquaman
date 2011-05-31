#include "XRFPeriodicTableView.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

XRFPeriodicTableView::XRFPeriodicTableView(XRFPeriodicTable *xrfTable, QWidget *parent)
	: QWidget(parent)
{
	xrfTable_ = xrfTable;
	connect(xrfTable_, SIGNAL(maximumEnergyChanged(double)), this, SLOT(disableElements()));
	connect(xrfTable_, SIGNAL(maximumEnergyChanged(double)), this, SLOT(disableElements()));

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

	QToolButton *trashButton = new QToolButton;
	trashButton->setIcon(QIcon(":/trashcan.png"));
	connect(trashButton, SIGNAL(clicked()), xrfTable_, SLOT(clearAll()));

	tableView_ = new AMPeriodicTableView;
	tableView_->setMaximumWidth(600);

	disableElements();

	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SIGNAL(elementSelected(AMElement*)));
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();
	legendLayout->addWidget(trashButton, 0, Qt::AlignCenter);

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addLayout(legendLayout);
	tableLayout->addWidget(tableView_, Qt::AlignLeft);

	setLayout(tableLayout);
}

void XRFPeriodicTableView::disableElements()
{
	QList<AMElement *> table(xrfTable_->elements());
	AMElement *temp;
	double min = xrfTable_->minimumEnergy();
	double max = xrfTable_->maximumEnergy();

	for (int i = 0; i < table.size(); i++){

		temp = table.at(i);
		// Resets the button state.
		tableView_->button(temp)->setEnabled(true);

		if (temp->Kalpha().second.toDouble() < min
			|| (temp->Kalpha().second.toDouble() > max && temp->Lalpha().second.toDouble() < min)
			|| temp->emissionLines().isEmpty())

			tableView_->button(temp)->setEnabled(false);
	}
}

void XRFPeriodicTableView::regionOfInterestAdded(AMElement *el, QString line)
{

	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());

	if (line.contains("K"))
		palette.setColor(QPalette::Window, Qt::green);
	else if (line.contains("L"))
		palette.setColor(QPalette::Window, Qt::yellow);
	else if (line.contains("M"))
		palette.setColor(QPalette::Window, Qt::cyan);

	clicked->setPalette(palette);
}

void XRFPeriodicTableView::regionOfInterestRemoved(AMElement *el, QString line)
{
	Q_UNUSED(line);

	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());
	palette.setColor(QPalette::Window, this->palette().color(QPalette::Window));
	clicked->setPalette(palette);
}
