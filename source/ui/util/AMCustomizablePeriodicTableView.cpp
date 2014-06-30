/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMCustomizablePeriodicTableView.h"

#include <QGridLayout>
#include <QLabel>

 AMCustomizablePeriodicTableView::~AMCustomizablePeriodicTableView(){}
AMCustomizablePeriodicTableView::AMCustomizablePeriodicTableView(AMCustomizablePeriodicTable *table, QWidget *parent)
	: QWidget(parent)
{
	periodicTable_ = table;

	// Initialize the signal mapper.
	elementMapper_ = new QSignalMapper(this);
	connect(elementMapper_, SIGNAL(mapped(int)), this, SLOT(onElementClicked(int)));

	setMaximumSize(750, 300);
}

void AMCustomizablePeriodicTableView::onElementClicked(int number)
{
	emit elementSelected(periodicTable_->elementByAtomicNumber(number));
}

QAbstractButton *AMCustomizablePeriodicTableView::mapElement(AMElement *element)
{
	QToolButton *button = new QToolButton;
	button->setFont(QFont("Times New Roman", 12));
	button->setText(element->symbol());
	button->setFixedSize(30, 25);
	button->setDisabled(element->emissionLines().isEmpty()
						&& element->absorptionEdges().isEmpty());

	connect(button, SIGNAL(clicked()), elementMapper_, SLOT(map()));
	elementMapper_->setMapping(button, element->atomicNumber());

	return button;
}

void AMCustomizablePeriodicTableView::buildPeriodicTableView()
{
	QGridLayout *tableLayout = new QGridLayout;
	tableLayout->setSpacing(0);

	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(1)), 0, 0);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(2)), 0, 17);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(3)), 1, 0);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(4)), 1, 1);
	for (int i = 5; i <= 10; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 1, 7+i);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(11)), 2, 0);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(12)), 2, 1);
	for (int i = 13; i <= 18; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 2, i-1);
	for (int i = 19; i <= 36; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 3, i-19);
	for (int i = 37; i <= 54; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 4, i-37);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(55)), 5, 0);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(56)), 5, 1);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(57)), 5, 2);
	for (int i = 72; i <= 86; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 5, i-69);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(87)), 6, 0);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(88)), 6, 1);
	tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(89)), 6, 2);
	for (int i = 104; i <= 109; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 6, i-101);
	tableLayout->addWidget(new QLabel(tr("Lanthanides")), 7, 0, 1, 3, Qt::AlignRight);
	for (int i = 58; i <= 71; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 7, i-55);
	tableLayout->addWidget(new QLabel(tr("Actinides")), 8, 0, 1, 3, Qt::AlignRight);
	for (int i = 90; i <= 103; i++)
		tableLayout->addWidget(mapElement(periodicTable_->elementByAtomicNumber(i)), 8, i-87);

	setLayout(tableLayout);
}
