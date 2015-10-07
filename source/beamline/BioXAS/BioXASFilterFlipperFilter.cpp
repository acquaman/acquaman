#include "BioXASFilterFlipperFilter.h"

BioXASFilterFlipperFilter::BioXASFilterFlipperFilter(AMElement *element, double thickness, QObject *parent, const QString &name) :
    QObject(parent)
{
	// Initialize member variables.

	element_ = 0;
	name_ = "None";
	thickness_ = 0;

	// Current settings.

	setElement(element);
	setThickness(thickness);

	if (!name.isEmpty())
		setName(name);
}

BioXASFilterFlipperFilter::~BioXASFilterFlipperFilter()
{

}

void BioXASFilterFlipperFilter::setElement(AMElement *newElement)
{
	if (element_ != newElement) {
		element_ = newElement;

		setName(generateName(element_, thickness_));

		emit elementChanged(element_);
	}
}

void BioXASFilterFlipperFilter::setName(const QString &newName)
{
	if (name_ != newName) {
		name_ = newName;
		emit nameChanged(name_);
	}
}

void BioXASFilterFlipperFilter::setThickness(double newThickness)
{
	if (thickness_ != newThickness) {
		thickness_ = newThickness;

		setName(generateName(element_, thickness_));

		emit thicknessChanged(thickness_);
	}
}

QString BioXASFilterFlipperFilter::generateName(AMElement *element, double thickness)
{
	QString result = QString();

	if (element)
		result = element->symbol() + " - " + QString::number(thickness);
	else
		result = "None";

	return result;
}
