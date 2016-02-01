#include "BioXASFilterFlipperFilter.h"
#include "util/AMPeriodicTable.h"

BioXASFilterFlipperFilter::BioXASFilterFlipperFilter(const QString &elementSymbol, double thickness, QObject *parent) :
    QObject(parent)
{
	// Initialize member variables.

	elementSymbol_ = "None";
	thickness_ = 0;

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
		emit elementSymbolChanged(newSymbol);
	}
}

void BioXASFilterFlipperFilter::setThickness(double newThickness)
{
	if (thickness_ != newThickness) {
		thickness_ = newThickness;
		emit thicknessChanged(thickness_);
	}
}
