#include "XRFElement.h"

XRFElement::XRFElement(AMElement *el, double minimumEnergy, double maximumEnergy, QObject *parent)
	: AMElement(el->name(), el->symbol(), el->atomicNumber(), el->edges(), el->emissionLines(), parent)
{
	minimumEnergy_ = minimumEnergy;
	maximumEnergy_ = maximumEnergy;

	for (int i = 0; i < el->emissionLines().size(); i++)
		lineMap_.insert(el->emissionLines().at(i).first, el->emissionLines().at(i).second.toDouble());
}

void XRFElement::addLine(QString line)
{
	// If no lines exist or line is not a duplicate, then add it to the selected list.
	if (selected_.isEmpty() || (!selected_.contains(line) && energyAcceptable(lineMap_.value(line)))){

		selected_ << line;
		emit lineAdded(this, line);
	}
}

void XRFElement::removeLine(QString line)
{
	// If the line exists in the list, remove it.
	if (selected_.contains(line)){

		selected_.removeAt(selected_.indexOf(line));
		emit lineRemoved(this, line);
	}
}

void XRFElement::removeAllLines()
{
	while(!selected_.isEmpty())
		removeLine(selected_.first());
}
