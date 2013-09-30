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


#include "AMSelectablePeriodicTable.h"

#include "util/AMSelectableElement.h"
#include "util/AMPeriodicTable.h"

AMSelectablePeriodicTable::AMSelectablePeriodicTable(QObject *parent)
	: AMCustomizablePeriodicTable(parent)
{
}

void AMSelectablePeriodicTable::selectElement(AMElement *element)
{
	savedElements_.append(element);
	emit elementSelected(element);
}

void AMSelectablePeriodicTable::deselectElement(AMElement *element)
{
	savedElements_.removeOne(element);
	emit elementDeselected(element);
}

void AMSelectablePeriodicTable::clearList()
{
	savedElements_.clear();
	emit listCleared();
}

void AMSelectablePeriodicTable::buildPeriodicTable()
{
	QList<AMElement *> elements = AMPeriodicTable::table()->elements();

	foreach (AMElement *element, elements){

		AMSelectableElement *newElement = new AMSelectableElement(element->name());
		periodicTable_ << newElement;

		connect(newElement, SIGNAL(absorptionEdgeSelected(AMAbsorptionEdge)), this, SLOT(onAbsorptionEdgeSelected(AMAbsorptionEdge)));
		connect(newElement, SIGNAL(absorptionEdgeDeselected(AMAbsorptionEdge)), this, SLOT(onAbsorptionEdgeDeselected(AMAbsorptionEdge)));
		connect(newElement, SIGNAL(emissionLineSelected(AMEmissionLine)), this, SLOT(onEmissionLineSelected(AMEmissionLine)));
		connect(newElement, SIGNAL(emissionLineDeselected(AMEmissionLine)), this, SLOT(onEmissionLineDeselected(AMEmissionLine)));
	}
}

void AMSelectablePeriodicTable::onAbsorptionEdgeSelected(const AMAbsorptionEdge &edge)
{
	emit absorptionEdgeSelected(elementBySymbol(edge.elementSymbol()), edge);
}

void AMSelectablePeriodicTable::onAbsorptionEdgeDeselected(const AMAbsorptionEdge &edge)
{
	emit absorptionEdgeDeselected(elementBySymbol(edge.elementSymbol()), edge);
}

void AMSelectablePeriodicTable::onEmissionLineSelected(const AMEmissionLine &line)
{
	emit emissionLineSelected(elementBySymbol(line.elementSymbol()), line);
}

void AMSelectablePeriodicTable::onEmissionLineDeselected(const AMEmissionLine &line)
{
	emit emissionLineDeselected(elementBySymbol(line.elementSymbol()), line);
}
