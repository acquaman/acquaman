#include "AMSelectablePeriodicTableView.h"

AMSelectablePeriodicTableView::AMSelectablePeriodicTableView(AMSelectablePeriodicTable *table, QWidget *parent)
	: AMPeriodicTableView(parent)
{
	table_ = table;
	connect(this, SIGNAL(clicked(int)), this, SLOT(onClicked(int)));

	// Go through every button and make it checkable.
	for (int i = 1; i <= table_->numberOfElements(); i++)
		button(i)->setCheckable(true);

	range_ = qMakePair(-1.0, -1.0);
}

void AMSelectablePeriodicTableView::onClicked(int id)
{
	QToolButton *clicked = button(id);

	if (clicked->isChecked())
		table_->selectElement(id);
	else
		table_->deselectElement(id);
}

void AMSelectablePeriodicTableView::setRange(double low, double high)
{
	range_ = qMakePair(low, high);
	const AMElement *temp;

	if (rangeIsValid()){

		for (int i = 1; i <= table_->numberOfElements(); i++){

			temp = table_->elementByAtomicNumber(i);

			if (temp->Kalpha().second.toDouble() < low
					|| (temp->Kalpha().second.toDouble() > high && temp->Lalpha().second.toDouble() < low)
					|| temp->emissionLines().isEmpty())
				button(i)->setEnabled(false);
			else
				button(i)->setEnabled(true);
		}
	}

	else {

		for (int i = 1; i <= table_->numberOfElements(); i++)
			button(i)->setEnabled(true);
	}
}
