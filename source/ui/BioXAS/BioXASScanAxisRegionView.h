#ifndef BIOXASSCANAXISREGIONVIEW_H
#define BIOXASSCANAXISREGIONVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLayout>

#include "dataman/AMScanAxisRegion.h"

#define BIOXASSCANAXISREGIONVIEW_POSITION_MIN -100000
#define BIOXASSCANAXISREGIONVIEW_POSITION_MAX 100000
#define BIOXASSCANAXISREGIONVIEW_TIME_MIN 0
#define BIOXASSCANAXISREGIONVIEW_TIME_MAX 100

class BioXASScanAxisRegionView : public QWidget
{
	Q_OBJECT

public:
	/// Enumeration of possible view modes.
	enum ViewMode { Absolute = 0, Relative = 1 };

	/// Constructor.
	explicit BioXASScanAxisRegionView(AMScanAxisRegion *region, ViewMode viewMode = Absolute, const AMNumber &baseValue = AMNumber(AMNumber::InvalidError), const QString &positionUnits = "", const QString &timeUnits = "s", QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASScanAxisRegionView();

	/// Returns true if the region view can display absolute region information.
	virtual bool canViewAbsolute() const;
	/// Returns true if the view can display relative region information.
	virtual bool canViewRelative() const;

	/// Returns the current view mode.
	ViewMode viewMode() const { return viewMode_; }
	/// Returns the region being viewed.
	AMScanAxisRegion* region() const { return region_; }

signals:
	/// Notifier that the view mode has changed.
	void viewModeChanged(ViewMode newMode);
	/// Notifier that the region being viewed has changed.
	void regionChanged(AMScanAxisRegion *newRegion);
	/// Notifier that the relative mode base value has changed.
	void baseValueChanged(const AMNumber &newValue);

public slots:
	/// Sets the current view mode.
	void setViewMode(ViewMode newMode);
	/// Sets the region being viewed.
	void setRegion(AMScanAxisRegion *newRegion);
	/// Sets the relative mode base value.
	void setBaseValue(const AMNumber &newValue);

protected slots:
	/// Updates the start box.
	void updateStartBox();
	/// Updates the step box.
	void updateStepBox();
	/// Updates the end box.
	void updateEndBox();
	/// Updates the time box.
	void updateTimeBox();

	/// Handles modifying the region in response to changes in the start box.
	void onStartBoxValueChanged();
	/// Handles modifying the region in response to changes in the step box.
	void onStepBoxValueChanged();
	/// Handles modifying the region in response to changes in the end box.
	void onEndBoxValueChanged();
	/// Handles modifying the region in response to changes in the time box.
	void onTimeBoxValueChanged();

protected:
	/// Returns the absolute value from the given base value and relative value.
	virtual AMNumber getAbsoluteValue(const AMNumber &baseValue, const AMNumber &relativeValue) const;
	/// Returns the relative value from the given base value and absolute value.
	virtual AMNumber getRelativeValue(const AMNumber &baseValue, const AMNumber &absoluteStartValue) const;

protected:
	/// The view mode.
	ViewMode viewMode_;
	/// The region being viewed.
	AMScanAxisRegion *region_;
	/// The region base value, for use in Relative view mode.
	AMNumber baseValue_;
	/// The position units.
	QString positionUnits_;
	/// The time units.
	QString timeUnits_;

	/// The region start box.
	QDoubleSpinBox *startBox_;
	/// The region step box.
	QDoubleSpinBox *stepBox_;
	/// The region end box.
	QDoubleSpinBox *endBox_;
	/// The region time box.
	QDoubleSpinBox *timeBox_;
};

#endif // BIOXASSCANAXISREGIONVIEW_H
