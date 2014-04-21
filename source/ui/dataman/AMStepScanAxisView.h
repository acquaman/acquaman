#ifndef AMSTEPSCANAXISVIEW_H
#define AMSTEPSCANAXISVIEW_H

#include <QWidget>

#include "acquaman/AMStepScanConfiguration.h"

#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QToolButton>

/// View that holds everything for a single region.
class AMStepScanAxisElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for a single region.
	explicit AMStepScanAxisElementView(AMScanAxisRegion *region, QWidget *parent = 0);

	/// Returns the region this view looks at.
	AMScanAxisRegion *region() const { return region_; }

protected slots:
	/// Sets the value for the start spin box.
	void setStartSpinBox(const AMNumber &value);
	/// Sets the value for the delta spin box.
	void setDeltaSpinBox(const AMNumber &value);
	/// Sets the value for the end spin box.
	void setEndSpinBox(const AMNumber &value);
	/// Sets the value for the time spin box.
	void setTimeSpinBox(const AMNumber &value);

	/// Handles setting the start position of the region.
	void onStartPositionUpdated();
	/// Handles setting the delta position of the region.
	void onDeltaPositionUpdated();
	/// Handles setting the end position of the region.
	void onEndPositionUpdated();
	/// Handles setting the time position of the region.
	void onTimeUpdated();

protected:

	/// The pointer to the region.
	AMScanAxisRegion *region_;
	/// The spin box that holds the start position.
	QDoubleSpinBox *start_;
	/// The spin box that holds the delta position.
	QDoubleSpinBox *delta_;
	/// The spin box that holds the end position.
	QDoubleSpinBox *end_;
	/// The spin box that holds the time position.
	QDoubleSpinBox *time_;
};

/// View that holds the collection of scan axis regions and allows the addition and deletion of regions.  Currently, only assumes a single scan axis.
class AMStepScanAxisView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the collection of regions.
	explicit AMStepScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles adding a new region.
	void onAddRegionButtonClicked();
	/// Handles removing a step scan axis element view after a user clicks the delete button.
	void onDeleteButtonClicked(QAbstractButton *button);

protected:
	/// Helper method that builds and connects all the key aspects around a given AMScanAxisRegion.
	void buildScanAxisRegionView(int index, AMScanAxisRegion *region);

	/// The pointer to the scan configuration that holds the container of the scan axes.
	AMStepScanConfiguration *configuration_;
	/// A mapping that maps the delete button to the region it is associated with.
	QMap<QAbstractButton *, AMStepScanAxisElementView *> regionMap_;
	/// A map that holds the layout that holds the delete button and the element view for memory management.
	QMap<QAbstractButton *, QLayout *> layoutMap_;
	/// The main layout that is added to and removed from.
	QVBoxLayout *scanAxisViewLayout_;
	/// The button group for mapping a delete button to a element view.
	QButtonGroup *deleteButtonGroup_;
	/// The add region button.  Used for proper placement of the popup menu when adding regions.
	QToolButton *addRegionButton_;
};

#endif // AMSTEPSCANAXISVIEW_H
