#ifndef AMREGIONOFINTERESTVIEW_H
#define AMREGIONOFINTERESTVIEW_H

#include <QWidget>

#include "dataman/AMRegionOfInterest.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>

/// The view for a single region of interest.
class AMRegionOfInterestElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a region of interest element view.
 	virtual ~AMRegionOfInterestElementView();
	explicit AMRegionOfInterestElementView(AMRegionOfInterest *region, QWidget *parent = 0);

protected slots:
	/// Handles changes in the value.
	void onValueChanged(double value);

	/// Handles updating the lower bound spin box if the lower bound is changed from elsewhere.
	void updateLowerBound(double value);
	/// Handles updating the upper bound spin box if the upper bound is changed from elsewhere.
	void updateUpperBound(double value);
	/// Updates the single step for the upper and lower bound spin boxes based on the increment of the spectrum data source.
	void updateSpinBoxSingleStep();
	/// Handles reseting the region pointer to 0 if the region is destroyed.
	void onRegionDestroyed();

protected:
	/// The region that this view encapsulates.
	AMRegionOfInterest *region_;

	/// The spin box for the lower bound.
	QDoubleSpinBox *lowerBound_;
	/// The spin box for the upper bound.
	QDoubleSpinBox *upperBound_;
	/// The label holding the value.
	QLabel *value_;
};

/// The region of interest view holds all of the regions of interest.
class AMRegionOfInterestView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds a view based on the list of regions provided.
 	virtual ~AMRegionOfInterestView();
	explicit AMRegionOfInterestView(QList<AMRegionOfInterest *> regions, QWidget *parent = 0);

signals:

public slots:
	/// Adds a view for the new region of interest.
	void addRegionOfInterest(AMRegionOfInterest *newRegion);
	/// Removes the view associated with the given region of interest.
	void removeRegionOfInterest(AMRegionOfInterest *region);

protected:
	/// The list of the regions.
	QMap<AMRegionOfInterest *, AMRegionOfInterestElementView *> regionsAndViews_;
	/// The layout that holds all of the region of interest views.
	QVBoxLayout *regionOfInterestViewLayout_;
};

#endif // AMREGIONOFINTERESTVIEW_H
