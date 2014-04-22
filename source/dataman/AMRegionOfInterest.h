#ifndef AMREGIONOFINTEREST_H
#define AMREGIONOFINTEREST_H

#include "dataman/database/AMDbObject.h"
#include "util/AMRange.h"
#include "analysis/AMRegionOfInterestAB.h"

/// Holds the essential information for a region of interest.  This would be the name, energy, and range around the energy.
class AMRegionOfInterest : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(double lowerBound READ lowerBound WRITE setLowerBound)
	Q_PROPERTY(double upperBound READ upperBound WRITE setUpperBound)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Region of Interest")

public:
	/// Constructor.  Requires a name, energy, and range for the region of interest.
	Q_INVOKABLE AMRegionOfInterest(const QString &name = "Invalid Region of Interest", double energy = 0.0, const AMRange &range = AMRange(), QObject *parent = 0);
	/// Constructor.  Requires a name, energy, the minimum range value, and the maximum range value.
	AMRegionOfInterest(const QString &name, double energy, double minimum, double maximum, QObject *parent);
	/// Copy constructor.
	AMRegionOfInterest(const AMRegionOfInterest &original);
	/// Destructor.
	virtual ~AMRegionOfInterest();

	/// Virtual copy constructor.  Returns a copy of this region of interest.  Caller is responsible for memory.
	AMRegionOfInterest *createCopy() const;

	/// The assignment operator.  Copies all information from one and sets the values here.
	AMRegionOfInterest &operator=(const AMRegionOfInterest &other);

	/// Returns whether the region of interest is null.
	bool isNull() const { return boundingRange_.isNull(); }
	/// Returns whether the region of interest is valid or not.
	bool isValid() const { return boundingRange_.isValid(); }

	/// Returns the energy of the region of interest.
	double energy() const { return energy_; }
	/// Returns the range that defines the width of the region of interest.
	const AMRange &boundingRange() const { return boundingRange_; }
	/// Returns the minimum value of the range.
	double lowerBound() const { return boundingRange_.minimum(); }
	/// Returns the maximum value of the range.
	double upperBound() const { return boundingRange_.maximum(); }

	/// Returns the value of the region of interest.
	double value() const { return value_->value(AMnDIndex()); }
	/// Returns the value data source.
	AMDataSource *valueSource() const { return value_; }

signals:
	/// Notifier that the energy of the region of interest has changed.  Passes the new energy.
	void energyChanged(double);
	/// Notifier that the range that defines the width has changed.  Passes the range.
	void boundingRangeChanged(const AMRange &);
	/// Notifier that the lower bound has changed.  Passes the new lower bound.
	void lowerBoundChanged(double);
	/// Notifier that the upper bound has changed.  Passes the new upper bound.
	void upperBoundChanged(double);
	/// Notifier that the value of the region has changed.
	void valueChanged(double);

public slots:
	/// Set the energy for the region of interest.
	void setEnergy(double energy);
	/// Sets the range that defines the width of the region of interest.
	void setBoundingRange(const AMRange &range);
	/// Sets the range that defines the width of the region of interest with explicit values for the lower and upper bounds.
	void setBoundingRange(double lowerBound, double upperBound);
	/// Sets the lower bound value of the region of interest.
	void setLowerBound(double lowerBound);
	/// Sets the upper bound value of the regionn of interest.
	void setUpperBound(double upperBound);
	/// Sets the spectrum data source for the region of interest.
	void setSpectrumSource(AMDataSource *source);

protected slots:
	/// Handles passing on the value changed on.
	void onValueChanged();

protected:
	/// The energy of the region of interest.
	double energy_;
	/// The range that defines the width of the region of interest.
	AMRange boundingRange_;
	/// The analysis block that determines the value of the region of interest.
	AMRegionOfInterestAB *value_;
};

#endif // AMREGIONOFINTEREST_H
