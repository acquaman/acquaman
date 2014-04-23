#ifndef AMEXAFSSCANAXISVIEW_H
#define AMEXAFSSCANAXISVIEW_H

#include <QWidget>

#include "acquaman/AMStepScanConfiguration.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QToolButton>
#include <QCheckBox>

/// View that holds everything for a single region.
class AMEXAFSScanAxisElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for a single region.
	explicit AMEXAFSScanAxisElementView(AMScanAxisEXAFSRegion *region, QWidget *parent = 0);

	/// Returns the region this view looks at.
	AMScanAxisEXAFSRegion *region() const { return region_; }

protected slots:
	/// Sets the value for the start spin box.
	void setStartSpinBox(const AMNumber &value);
	/// Sets the value for the delta spin box.
	void setDeltaSpinBox(const AMNumber &value);
	/// Sets the value for the end spin box.
	void setEndSpinBox(const AMNumber &value);
	/// Sets the value for the time spin box.
	void setTimeSpinBox(const AMNumber &value);
	/// Sets the value for the maximum time sping box.
	void setMaximumTimeSpinBox(const AMNumber &value);

	/// Handles setting the inKSpace flag of the region.
	void onInKSpaceUpdated(bool inKSpace);
	/// Handles setting the start position of the region.
	void onStartPositionUpdated();
	/// Handles setting the delta position of the region.
	void onDeltaPositionUpdated();
	/// Handles setting the end position of the region.
	void onEndPositionUpdated();
	/// Handles setting the time per point of the region.
	void onTimeUpdated();
	/// Handles setting the maximum time per point of the region.
	void onMaximumTimeUpdated();

protected:

	/// The pointer to the region.
	AMScanAxisEXAFSRegion *region_;
	/// The check box that switches whether the region is in k space or not.
	QCheckBox *inKSpace_;
	/// The spin box that holds the start position.
	QDoubleSpinBox *start_;
	/// The spin box that holds the delta position.
	QDoubleSpinBox *delta_;
	/// The spin box that holds the end position.
	QDoubleSpinBox *end_;
	/// The spin box that holds the time per point.
	QDoubleSpinBox *time_;
	/// The spin box thhat holds the maximum time per point.
	QDoubleSpinBox *maximumTime_;
};

/// View that holds the collection of scan axis regions and allows the addition and deletion of EXAFS regions.
class AMEXAFSScanAxisView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the collection of EXAFS regions.
	explicit AMEXAFSScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles adding a new region.
	void onAddRegionButtonClicked();
	/// Handles removing a step scan axis element view after a user clicks the delete button.
	void onDeleteButtonClicked(QAbstractButton *button);
	/// Handles switching whether the regions are locked or not.
	void onLockRegionsToggled(bool toggled);

protected:
	/// Helper method that builds and connects all the key aspects around a given AMScanAxisRegion.
	void buildScanAxisRegionView(int index, AMScanAxisEXAFSRegion *region);
	/// Returns whether the regions should be locked together or not.
	bool regionsLocked() const { return !lockRegionsButton_->isChecked(); }

	/// The pointer to the scan configuration that holds the container of the scan axes.
	AMStepScanConfiguration *configuration_;
	/// A mapping that maps the delete button to the region it is associated with.
	QMap<QAbstractButton *, AMEXAFSScanAxisElementView *> regionMap_;
	/// A map that holds the layout that holds the delete button and the element view for memory management.
	QMap<QAbstractButton *, QLayout *> layoutMap_;
	/// The main layout that is added to and removed from.
	QVBoxLayout *scanAxisViewLayout_;
	/// The button group for mapping a delete button to a element view.
	QButtonGroup *deleteButtonGroup_;
	/// The add region button.  Used for proper placement of the popup menu when adding regions.
	QToolButton *addRegionButton_;
	/// The button that locks regions together.
	QToolButton *lockRegionsButton_;
};

#endif // AMEXAFSSCANAXISVIEW_H
