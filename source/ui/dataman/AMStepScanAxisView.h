/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMSTEPSCANAXISVIEW_H
#define AMSTEPSCANAXISVIEW_H

#include <QWidget>

#include "acquaman/AMStepScanConfiguration.h"

#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>

/// View that holds everything for a single region.
class AMStepScanAxisElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for a single region.
	explicit AMStepScanAxisElementView(AMScanAxisRegion *region, QWidget *parent = 0, QString startLabel = "", QString endLabel = "");

	/// Returns the region this view looks at.
	AMScanAxisRegion *region() const { return region_; }

signals:
	/// Notifier that the start has changed.  This is ALWAYS in eV.
	void startValueChanged(const AMNumber &);
	/// Notifier that the end has changed.  This is ALWAYS in eV.
	void endValueChanged(const AMNumber &);

public slots:
	/// Sets the value for the start spin box.
	void setStartSpinBox(const AMNumber &value);
	/// Sets the value for the delta spin box.
	void setDeltaSpinBox(const AMNumber &value);
	/// Sets the value for the end spin box.
	void setEndSpinBox(const AMNumber &value);
	/// Sets the value for the time spin box.
	void setTimeSpinBox(const AMNumber &value);

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

/// View that holds the collection of scan axis regions and allows the addition and deletion of regions.  Currently, only assumes a single scan axis.
class AMStepScanAxisView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the collection of regions.
	explicit AMStepScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent = 0, QString startLabel = "Start", QString endLabel = "End");

signals:

public slots:
	/// Adds a region to the axis.
	void insertRegion(int index, AMScanAxisRegion *region);
	/// Removes a region from the given index.
	void removeRegion(int index);

protected slots:
	/// Handles adding a new region.
	void onAddRegionButtonClicked();
	/// Handles removing a step scan axis element view after a user clicks the delete button.
	void onDeleteButtonClicked(QAbstractButton *button);
	/// Handles switching whether the regions are locked or not.
	void onLockRegionsToggled(bool toggled);

protected:
	/// Helper method that builds and connects all the key aspects around a given AMScanAxisRegion.
	void buildScanAxisRegionView(int index, AMScanAxisRegion *region);
	/// Returns whether the regions should be locked together or not.
	bool regionsLocked() const { return !lockRegionsButton_->isChecked(); }
	/// Connects start and end points of regions together.
	void connectRegions() const;
	/// Disconnects start and end points of regions from each other.
	void disconnectRegions() const;

	/// The pointer to the scan configuration that holds the container of the scan axes.
	AMStepScanConfiguration *configuration_;
	/// List specifically for locking the regions together properly.
	QList<AMStepScanAxisElementView *> lockedElementViewList_;
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
	/// The button that locks regions together.
	QToolButton *lockRegionsButton_;
	/// Holds the label for the start of a region.
	QString startLabelString_;
	/// Holds the label for the end of a region.
	QString endLabelString_;
};

#endif // AMSTEPSCANAXISVIEW_H
