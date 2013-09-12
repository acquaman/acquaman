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
