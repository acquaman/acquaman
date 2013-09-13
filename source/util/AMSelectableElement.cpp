#include "AMSelectableElement.h"

#include "util/AMPeriodicTable.h"

AMSelectableElement::AMSelectableElement(const QString &elementName, QObject *parent)
	: AMElement(AMPeriodicTable::table()->elementByName(elementName))
{
	setParent(parent);
}

AMSelectableElement::AMSelectableElement(const AMElement &element, QObject *parent)
	: AMElement(element)
{
	setParent(parent);
}

AMSelectableElement::AMSelectableElement(const AMSelectableElement &original)
	: AMElement(original)
{

}

AMSelectableElement::AMSelectableElement()
	: AMElement()
{

}

bool AMSelectableElement::selectAbsorptionEdge(const AMAbsorptionEdge &edge)
{
	// Only add edges if they are a part of this element and only if they aren't already selected.
	if (edges().contains(edge) && !selectedAbsorptionEdges_.contains(edge)){

		selectedAbsorptionEdges_ << edge;
		return true;
	}

	return false;
}

bool AMSelectableElement::deselectAbsorptionEdge(const AMAbsorptionEdge &edge)
{
	return selectedAbsorptionEdges_.removeOne(edge);
}

bool AMSelectableElement::selectEmissionLine(const AMEmissionLine &line)
{
	// Only add emission lines if they are a part of this element and only if they aren't already selected.
	if (emissionLines().contains(line) && !selectedEmissionLines_.contains(line)){

		selectedEmissionLines_ << line;
		return true;
	}

	return false;
}

bool AMSelectableElement::deselectEmissionLine(const AMEmissionLine &line)
{
	return selectedEmissionLines_.removeOne(line);
}

void AMSelectableElement::removeAllAbsorptionEdges()
{
	selectedAbsorptionEdges_.clear();
}

void AMSelectableElement::removeAllEmissionLines()
{
	selectedEmissionLines_.clear();
}

void AMSelectableElement::removeAllSelectedItems()
{
	removeAllAbsorptionEdges();
	removeAllEmissionLines();
}
