#include "AMSelectableElement.h"

#include "util/AMPeriodicTable.h"

 AMSelectableElement::~AMSelectableElement(){}
AMSelectableElement::AMSelectableElement(const QString &elementName, QObject *parent)
	: AMElement(*AMPeriodicTable::table()->elementByName(elementName))
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
	if (absorptionEdges().contains(edge) && !selectedAbsorptionEdges_.contains(edge)){

		selectedAbsorptionEdges_ << edge;
		emit absorptionEdgeSelected(edge);
		return true;
	}

	return false;
}

bool AMSelectableElement::deselectAbsorptionEdge(const AMAbsorptionEdge &edge)
{
	if (selectedAbsorptionEdges_.removeOne(edge)){

		emit absorptionEdgeDeselected(edge);
		return true;
	}

	return false;
}

bool AMSelectableElement::selectEmissionLine(const AMEmissionLine &line)
{
	// Only add emission lines if they are a part of this element and only if they aren't already selected.
	if (emissionLines().contains(line) && !selectedEmissionLines_.contains(line)){

		selectedEmissionLines_ << line;
		emit emissionLineSelected(line);
		return true;
	}

	return false;
}

bool AMSelectableElement::deselectEmissionLine(const AMEmissionLine &line)
{
	if (selectedEmissionLines_.removeOne(line)){

		emit emissionLineDeselected(line);
		return true;
	}

	return false;
}

void AMSelectableElement::removeAllAbsorptionEdges()
{
	selectedAbsorptionEdges_.clear();
	emit allAbsorptionEdgesDeselected();
}

void AMSelectableElement::removeAllEmissionLines()
{
	selectedEmissionLines_.clear();
	emit allEmissionLinesDeselected();
}

void AMSelectableElement::removeAllSelectedItems()
{
	removeAllAbsorptionEdges();
	removeAllEmissionLines();
}
