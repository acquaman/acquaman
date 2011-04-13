#include "ui/AMPeriodicTableView.h"

#include <QToolButton>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

AMPeriodicTableView::AMPeriodicTableView(QWidget *parent)
	: QWidget(parent)
{
	// Initialize the table and signal mapper.
	table_ = AMPeriodicTable::table();
	elementMapper_ = new QSignalMapper(this);

	// Build the periodic table.
	QGridLayout *tableLayout = new QGridLayout;
	tableLayout->setSpacing(0);

	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(1)), 0, 0);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(2)), 0, 17);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(3)), 1, 0);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(4)), 1, 1);
	for (int i = 5; i <= 10; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 1, 7+i);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(11)), 2, 0);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(12)), 2, 1);
	for (int i = 13; i <= 18; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 2, i-1);
	for (int i = 19; i <= 36; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 3, i-19);
	for (int i = 37; i <= 54; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 4, i-37);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(55)), 5, 0);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(56)), 5, 1);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(57)), 5, 2);
	for (int i = 72; i <= 86; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 5, i-69);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(87)), 6, 0);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(88)), 6, 1);
	tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(89)), 6, 2);
	for (int i = 104; i <= 109; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 6, i-101);
	tableLayout->addWidget(new QLabel(tr("Lanthanides")), 7, 0, 1, 3, Qt::AlignRight);
	for (int i = 58; i <= 71; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 7, i-55);
	tableLayout->addWidget(new QLabel(tr("Actinides")), 8, 0, 1, 3, Qt::AlignRight);
	for (int i = 90; i <= 103; i++)
		tableLayout->addWidget(mapElement(table_->elementByAtomicNumber(i)), 8, i-87);

	// End of build periodic table.

	connect(elementMapper_, SIGNAL(mapped(int)), this, SIGNAL(clicked(int)));
	connect(this, SIGNAL(clicked(int)), this, SLOT(showElement(int)));

	setLayout(tableLayout);
	setMaximumSize(750, 300);
}

AMPeriodicTableView::~AMPeriodicTableView()
{
	delete elementMapper_;
}

void AMPeriodicTableView::showElement(int number)
{
	emit elementSelected(table_->elementByAtomicNumber(number));
}
