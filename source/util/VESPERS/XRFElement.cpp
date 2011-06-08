#include "XRFElement.h"

XRFElement::XRFElement(AMElement *el, QObject *parent)
	: AMElement(el->name(), el->symbol(), QString::number(el->atomicNumber()), toStringList(el->edges()), toStringList(el->emissionLines()), parent)
{
	for (int i = 0; i < el->emissionLines().size(); i++)
		lineMap_.insert(el->emissionLines().at(i).first, el->emissionLines().at(i).second.toDouble());
}

bool XRFElement::addLine(QString line)
{
	// If no lines exist or line is not a duplicate, then add it to the selected list.
	if (selected_.isEmpty() || !selected_.contains(line)){

		selected_ << line;
		return true;
	}

	return false;
}

bool XRFElement::removeLine(QString line)
{
	// If the line exists in the list, remove it.
	if (selected_.contains(line)){

		selected_.removeAt(selected_.indexOf(line));
		return true;
	}

	return false;
}

QStringList XRFElement::toStringList(QList<QPair<QString, QString> > list)
{
	QStringList sList;

	for (int i = 0; i < list.size(); i++)
		sList << list.at(i).second;

	return sList;
}
