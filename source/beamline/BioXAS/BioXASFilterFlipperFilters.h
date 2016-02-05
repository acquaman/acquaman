#ifndef BIOXASFILTERFLIPPERFILTERS_H
#define BIOXASFILTERFLIPPERFILTERS_H

#include "beamline/AMPVControl.h"
#include "beamline/AMSingleEnumeratedControl.h"

class BioXASFilterFlipperFilter;

class BioXASFilterFlipperFilters : public AMSingleEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperFilters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperFilters();

	/// Returns a list of filter indices.
	QList<int> filterIndices() const { return indexFilterMap_.keys(); }
	/// Returns the list of filters.
	QList<BioXASFilterFlipperFilter*> filters() const { return indexFilterMap_.values(); }
	/// Returns the filter at the given index.
	BioXASFilterFlipperFilter* filterAt(int index) const { return indexFilterMap_.value(index, 0); }

	/// Returns the slides control.
	AMPVControl* slides() const { return qobject_cast<AMPVControl*>(control_); }

signals:
	/// Notifier that the current slide index control has changed.
	void slidesChanged(AMPVControl *newControl);
	/// Notifier that the filter options have changed.
	void filtersChanged();

public slots:
	/// Sets the current slide index control.
	void setSlides(AMPVControl *newControl);
	/// Sets the filter at the given index by filter characteristics.
	void setFilter(int index, const QString &elementSymbol, double thickness);
	/// Removes the filter at the given index.
	void removeFilter(int index);

protected slots:
	/// Updates the available options.
	virtual void updateOptions();

	/// Sets the element symbol for the filter at the given index.
	void setFilterElementSymbol(int index, const QString &newSymbol);
	/// Sets the thickness for the filter at the given index.
	void setFilterThickness(int index, double thickness);

	/// Sets the filter option.
	void setFilterOption(int index, BioXASFilterFlipperFilter *filter);
	/// Adds a filter option.
	void addFilterOption(int index, BioXASFilterFlipperFilter *newFilter);
	/// Removes a filter option.
	void removeFilterOption(int index);
	/// Clears all filter options.
	void clearFilterOptions();

protected:
	/// Returns a string representation of the given filter.
	QString filterToString(BioXASFilterFlipperFilter *filter) const;
	/// Creates and returns a filter with the given characteristics.
	BioXASFilterFlipperFilter* createFilter(const QString &elementSymbol, double thickness);

protected:
	/// The index filter map.
	QMap<int, BioXASFilterFlipperFilter*> indexFilterMap_;
};

#endif // BIOXASFILTERFLIPPERFILTERS_H
