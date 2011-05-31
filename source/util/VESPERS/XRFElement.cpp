#include "XRFElement.h"

XRFElement::XRFElement(AMElement *el, QObject *parent)
	: AMElement(el->name(), el->symbol(), el->atomicNumber(), el->edges(), el->emissionLines(), parent)
{
}
