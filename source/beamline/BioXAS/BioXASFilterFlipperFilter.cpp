#include "BioXASFilterFlipperFilter.h"
#include "util/AMPeriodicTable.h"

BioXASFilterFlipperFilter::BioXASFilterFlipperFilter(const QString &elementSymbol, double thickness, QObject *parent) :
    QObject(parent)
{
	// Initialize member variables.

	elementSymbol_ = "None";
	thickness_ = 0;
	name_ = "None";

	// Current settings.

	setElementSymbol(elementSymbol);
	setThickness(thickness);
}

BioXASFilterFlipperFilter::~BioXASFilterFlipperFilter()
{

}

bool BioXASFilterFlipperFilter::isValid() const
{
	return isValid(elementSymbol_, thickness_);
}

bool BioXASFilterFlipperFilter::isValid(const QString &elementSymbol, double thickness) const
{
	bool result = false;

	if (AMPeriodicTable::table()->elementBySymbol(elementSymbol) && thickness > 0)
		result = true;

	return result;
}

void BioXASFilterFlipperFilter::setElementSymbol(const QString &newSymbol)
{
	if (elementSymbol_ != newSymbol && AMPeriodicTable::table()->elementBySymbol(newSymbol)) {
		elementSymbol_ = newSymbol;

		updateName();

		emit elementSymbolChanged(newSymbol);
	}
}

void BioXASFilterFlipperFilter::setThickness(double newThickness)
{
	if (thickness_ != newThickness) {
		thickness_ = newThickness;

		updateName();

		emit thicknessChanged(thickness_);
	}
}

void BioXASFilterFlipperFilter::updateName()
{
	setName( generateName(elementSymbol_, thickness_) );
}

void BioXASFilterFlipperFilter::setName(const QString &newName)
{
	if (name_ != newName) {
		name_ = newName;
		emit nameChanged(name_);
	}
}

QString BioXASFilterFlipperFilter::generateName(const QString &elementSymbol, double thickness) const
{
	QString result = QString();

	if (isValid(elementSymbol, thickness))
		result = elementSymbol + " - " + QString::number(thickness);
	else
		result = "None";

	return result;
}
