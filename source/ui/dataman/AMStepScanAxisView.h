#ifndef AMSTEPSCANAXISVIEW_H
#define AMSTEPSCANAXISVIEW_H

#include <QWidget>

#include "acquaman/AMStepScanConfiguration.h"

#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>

/// View that holds everything for a single region.
class AMStepScanAxisElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for a single region.
	explicit AMStepScanAxisElementView(AMScanAxisRegion *region, QWidget *parent = 0);

protected slots:

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

/// View that holds the collection of scan axis regions and allows the addition and deletion of regions.
class AMStepScanAxisView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the collection of regions.
	explicit AMStepScanAxisView(AMStepScanConfiguration *configuration, QWidget *parent = 0);

signals:

public slots:

protected:
	/// The pointer to the scan configuration that holds the container of the scan axes.
	AMStepScanConfiguration *configuration_;
	/// A mapping that maps the delete button to the region it is associated with.
	QMap<QAbstractButton *, AMStepScanAxisElementView> regionMap_;
	/// The
};

#endif // AMSTEPSCANAXISVIEW_H
