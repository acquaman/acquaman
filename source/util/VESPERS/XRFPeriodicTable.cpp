#include "XRFPeriodicTable.h"

#include "util/AMPeriodicTable.h"

XRFPeriodicTable::XRFPeriodicTable(double minEnergy, double maxEnergy, QObject *parent)
	: QObject(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;

	XRFElement *temp;

	for (int i = 0; i < AMPeriodicTable::table()->elements().size(); i++){

		temp = new XRFElement(AMPeriodicTable::table()->elementByAtomicNumber(i+1), minimumEnergy_, maximumEnergy_, this);
		xrfTable_ << temp;
		connect(temp, SIGNAL(lineAdded(XRFElement*,QString)), this, SLOT(onLineAdded(XRFElement*,QString)));
		connect(temp, SIGNAL(lineRemoved(XRFElement*,QString)), this, SLOT(onLineRemoved(XRFElement*,QString)));
		connect(this, SIGNAL(minimumEnergyChanged(double)), temp, SLOT(setMinimumEnergy(double)));
		connect(this, SIGNAL(maximumEnergyChanged(double)), temp, SLOT(setMaximumEnergy(double)));
	}
}

void XRFPeriodicTable::addToList(QString symbol, QString line)
{
	XRFElement *el = elementBySymbol(symbol);

	if (el)
		el->addLine(line);
}

void XRFPeriodicTable::removeFromList(QString symbol, QString line)
{
	XRFElement *el = elementBySymbol(symbol);

	if (el)
		el->removeLine(line);
}

void XRFPeriodicTable::onLineAdded(XRFElement *el, QString line)
{
	Q_UNUSED(line);

	XRFElement *temp;

	for (int i = 0; i < selectedElements_.size(); i++){

		temp = selectedElements_.at(i);
		if (temp->symbol().compare(el->symbol()) != 0){

			selectedElements_ << el;
			emit selectedElementsChanged();
		}
	}
}

void XRFPeriodicTable::onLineRemoved(XRFElement *el, QString line)
{
	Q_UNUSED(line);

	if (!el->hasLinesSelected()){

		selectedElements_.removeOne(el);
		emit selectedElementsChanged();
	}
}
