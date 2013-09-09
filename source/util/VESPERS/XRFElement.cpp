/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "XRFElement.h"

XRFElement::XRFElement(const AMElement *el, QObject *parent)
	: AMElement(el->name(), el->symbol(), QString::number(el->atomicNumber()), interimEdgeList(el->edges()), toStringList(el->emissionLines()), parent)
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

QStringList XRFElement::interimEdgeList(QList<AMAbsorptionEdge> list)
{
	QStringList sList;

	for (int i = 0; i < list.size(); i++)
		sList << list.at(i).energyString();

	return sList;
}
