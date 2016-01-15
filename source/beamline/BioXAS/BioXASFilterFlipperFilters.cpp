#include "BioXASFilterFlipperFilters.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilter.h"

BioXASFilterFlipperFilters::BioXASFilterFlipperFilters(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Filter Flipper Filters Control");
	setAllowsMovesWhileMoving(false);

	// We have a 'None' option by default.

	setFilterOption(0, 0);
}

BioXASFilterFlipperFilters::~BioXASFilterFlipperFilters()
{

}

void BioXASFilterFlipperFilters::setSlides(AMPVControl *newControl)
{
	if (setBaseControl(newControl))
		emit slidesChanged(newControl);
}

void BioXASFilterFlipperFilters::setFilter(int index, const QString &elementSymbol, double thickness)
{
	if (indexFilterMap_.keys().contains(index)) {

		// If the given index already has a filter, can modify the existing filter
		// with the given characteristics.

		setFilterElementSymbol(index, elementSymbol);
		setFilterThickness(index, thickness);

	} else {

		// If there is no existing filter, can create one.

		setFilterOption(index, createFilter(elementSymbol, thickness));
	}
}

void BioXASFilterFlipperFilters::removeFilter(int index)
{
	removeFilterOption(index);
}

void BioXASFilterFlipperFilters::updateOptions()
{
	// Clear the available value options, but not the index filter map.

	clearOptions();

	// Identify new list of possible filter options, found by iterating
	// through the index filter map.

	QList<int> indices = indexFilterMap_.keys();

	foreach (int index, indices) {
		addFilterOption(index, indexFilterMap_.value(index, 0));
	}
}

void BioXASFilterFlipperFilters::setFilterElementSymbol(int index, const QString &newSymbol)
{
	if (indexFilterMap_.keys().contains(index)) {
		BioXASFilterFlipperFilter *filter = indexFilterMap_.value(index, 0);

		if (filter)
			filter->setElementSymbol(newSymbol);
	}
}

void BioXASFilterFlipperFilters::setFilterThickness(int index, double thickness)
{
	if (indexFilterMap_.keys().contains(index)) {
		BioXASFilterFlipperFilter *filter = indexFilterMap_.value(index, 0);

		if (filter)
			filter->setThickness(thickness);
	}
}

void BioXASFilterFlipperFilters::setFilterOption(int index, BioXASFilterFlipperFilter *filter)
{
	if (indexFilterMap_.keys().contains(index))
		removeFilterOption(index);

	addFilterOption(index, filter);
}

void BioXASFilterFlipperFilters::addFilterOption(int index, BioXASFilterFlipperFilter *newFilter)
{
	// Add the filter to the index filter map and the control options.
	// Connect to its signals that indicate that its properties may have changed.

	if (AMSingleEnumeratedControl::addValueOption(index, filterToString(newFilter), index, false)) {
		indexFilterMap_.insert(index, newFilter);

		if (newFilter) {
			connect( newFilter, SIGNAL(elementSymbolChanged(QString)), this, SLOT(updateOptions()) );
			connect( newFilter, SIGNAL(thicknessChanged(double)), this, SLOT(updateOptions()) );
		}

		emit filtersChanged();
	}
}

void BioXASFilterFlipperFilters::removeFilterOption(int index)
{
	// Remove the filter from the index filter map and the control options,
	// and delete it.

	if (AMSingleEnumeratedControl::removeOption(index)) {

		BioXASFilterFlipperFilter *filter = indexFilterMap_.value(index, 0);

		if (filter) {
			filter->disconnect();
			filter->deleteLater();
		}

		indexFilterMap_.remove(index);

		emit filtersChanged();
	}
}

void BioXASFilterFlipperFilters::clearFilterOptions()
{
	// Clear the index filter map and the control options.

	if (AMSingleEnumeratedControl::clearOptions()) {
		indexFilterMap_.clear();

		emit filtersChanged();
	}
}

QString BioXASFilterFlipperFilters::filterToString(BioXASFilterFlipperFilter *filter) const
{
	QString result = "None";

	if (filter) {
		result = "Invalid";

		if (filter->isValid())
			result = QString("%1 - %2").arg(filter->elementSymbol()).arg(filter->thickness());
	}

	return result;
}

BioXASFilterFlipperFilter* BioXASFilterFlipperFilters::createFilter(const QString &elementSymbol, double thickness)
{
	return new BioXASFilterFlipperFilter(elementSymbol, thickness, this);
}
