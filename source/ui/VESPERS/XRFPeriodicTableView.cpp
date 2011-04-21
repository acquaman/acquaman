#include "XRFPeriodicTableView.h"

#include <QLabel>
#include <QVBoxLayout>

XRFPeriodicTableView::XRFPeriodicTableView(QWidget *parent) :
	QWidget(parent)
{
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

//	tableView_ = new AMPeriodicTableView;
	//tableView_->setMaximumWidth(600);

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();

	QHBoxLayout *tableLayout = new QHBoxLayout;
	tableLayout->addLayout(legendLayout);
	//tableLayout->addWidget(tableView_, Qt::AlignLeft);

	setLayout(tableLayout);
}
