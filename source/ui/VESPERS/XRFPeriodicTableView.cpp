#include "XRFPeriodicTableView.h"

#include <QLabel>
#include <QVBoxLayout>

XRFPeriodicTableView::XRFPeriodicTableView(double minEnergy, double maxEnergy, QWidget *parent)
	: QWidget(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;

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

	disableElements();

	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SIGNAL(elementSelected(AMElement*)));

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addLayout(legendLayout);
	tableLayout->addWidget(tableView_, Qt::AlignLeft);

	setLayout(tableLayout);
}

void XRFPeriodicTableView::disableElements()
{
	QList<AMElement *> table(AMPeriodicTable::table()->elements());
	AMElement *temp;

	for (int i = 0; i < table.size(); i++){

		temp = table.at(i);
		// Undoes any previous disabling.
		tableView_->button(temp)->setEnabled(true);

		if (temp->Kalpha().second.toDouble() < minimumEnergy_
			|| (temp->Kalpha().second.toDouble() > maximumEnergy_ && temp->Lalpha().second.toDouble() < minimumEnergy_)
			|| temp->emissionLines().isEmpty())

			tableView_->button(temp)->setEnabled(false);
	}
}

void XRFPeriodicTableView::regionOfInterestAdded(AMElement *el, QPair<QString, QString> line)
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

void XRFPeriodicTableView::regionOfInterestRemoved(AMElement *el, QPair<QString, QString> line)
{
	QToolButton *clicked = tableView_->button(el);
	QPalette palette(clicked->palette());
	palette.setColor(QPalette::Button, this->palette().color(QPalette::Button));
	tableView_->button(el)->setPalette(palette);
}
