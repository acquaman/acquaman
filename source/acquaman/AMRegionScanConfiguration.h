/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMREGIONSCANCONFIGURATION_H
#define AMREGIONSCANCONFIGURATION_H

#include <QObject>
#include "AMScanConfiguration.h"
#include "acquaman/AMRegionsList.h"
#include "dataman/info/AMControlInfo.h"

/// An AMRegionScanConfiguration is the parent class for any beamline that wants to implement a simple regions based scan.
/*!
  The AMRegionScanConfiguration class is designed to allow beamline specific implementation classes to inherit it, but take advantage of a common method for configuring the regions associated with the scan.
  To this end, the class holds a QList of pointers to AMRegion.
  Functions for adding and deleting regions, as well as editing regions are available.
  */
class AMRegionScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(QString regions READ dbReadRegions WRITE dbLoadRegions)
	Q_PROPERTY(double startValue READ startValue WRITE setStartValue)
	Q_PROPERTY(double endValue READ endValue WRITE setEndValue)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Region Scan Configuration")

public:
	/// Constructor, needs a pointer to a QObject to act as a parent and needs to know whether it should new its own regions list.  This allows a way to not new the wrong type of regions list.
	AMRegionScanConfiguration(QObject *parent = 0, bool setup = true);
	AMRegionScanConfiguration(const AMRegionScanConfiguration &original, bool setup = true);
	virtual ~AMRegionScanConfiguration();

	const QMetaObject* getMetaObject();

	/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double regionStart(int index) const { return regions_->start(index); }
	/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double regionDelta(int index) const { return regions_->delta(index); }
	/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double regionEnd(int index) const { return regions_->end(index); }
	/// Returns the time value of the region referred to by \param index.  If an invalid index is given, returns -1 (not a valid time value).
	double regionTime(int index) const { return regions_->time(index); }
	/// Returns the units for the region referred to by \param index.  If an invalid index is given, returns a null string.
	QString regionUnits(int index) const { return regions_->units(index); }
	/// Returns the time units for the region referred to by \param index.  If an invalid index is given, returns a null string.
	QString regionTimeUnits(int index) const { return regions_->timeUnits(index); }
	/// Returns whether elastic start is enabled for the region referred to by \param index.
	bool regionElasticStart(int index) const { return regions_->elasticStart(index); }
	/// Returns whether elastic end is enabled for the reigon referred to by \param index.
	bool regionElasticEnd(int index) const { return regions_->elasticEnd(index); }
	/// Returns the number of regions current in the scan configuration.
	int regionCount() const { return regions_->count(); }
	/// Returns the regions list.
	AMRegionsList* regions() const { return regions_; }

	/// Quick accessor for the start of the first region. If no regions are set, returns -1
	virtual double startValue() const;
	/// Quick accessor for the end of the final region. If no reginos are set, returns -1
	virtual double endValue() const;

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		return QString("Region Scan from %1%3 to %2%4").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(regionUnits(0)).arg(regionUnits(regionCount()-1));
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("Region Scan from %1%3 to %2%4").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(regionUnits(0)).arg(regionUnits(regionCount()-1));
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("Region Scan");
	}

	/// Pure virtual functions that need to be implemented.  Because controls aren't set in AMRegionScanConfiguration, subclasses need to implement the control info getter to their valid region and time controls.
	/// Returns the AMControlInfo for the region scan control.
	virtual AMControlInfo regionControlInfo() const = 0;
	/// Returns the AMControlInfo for the time control.
	virtual AMControlInfo timeControlInfo() const = 0;

	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return "Region Scan";
	}

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setRegionStart(int index, double start) { return regions_->setStart(index, start); }
	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setRegionDelta(int index, double delta) { return regions_->setDelta(index, delta); }
	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setRegionEnd(int index, double end) { return regions_->setEnd(index, end); }
	/// Sets the time value for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid or the time is negative.
	bool setRegionTime(int index, double time) { return regions_->setTime(index, time); }
	/// Sets the units for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setRegionUnits(int index, const QString &units) { return regions_->setUnits(index, units); }
	/// Sets the time units for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setRegionTimeUnits(int index, const QString &units) { return regions_->setTimeUnits(index, units); }
	/// Sets the elastic start state for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setRegionElasticStart(int index, bool state) { return regions_->setElasticStart(index, state); }
	/// Sets the elastic end state for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setRegionElasticEnd(int index, bool state) { return regions_->setElasticEnd(index, state); }
	/// Creates a new region using start, delta, end, and time values.
	virtual bool addRegion(int index, double start, double delta, double end, double time) { return regions_->addRegion(index, start, delta, end, time); }
	/// Overloaded.  Creates a new region using start, delta, and end values then calls addRegion(index, *region).
	virtual bool addRegion(int index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end); }
	/// Deletes the region refered to by index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(int index) { return regions_->deleteRegion(index); }

	/// Quick setter for the start of the first region. Returns false if there are no regions or the energy is out of range
	virtual bool setStartValue(double startValue);
	/// Quick setter for the end of the final region. Returns false if there are no regions or the energy is out of range
	virtual bool setEndValue(double endValue);

signals:
	/// Notifier that passes on the signal that current state of the regions have changed.
	void regionsChanged();

protected slots:
	/// Helper slot that handles notifying the database that the regions have changed and emits the regionsChanged() signal.
	void onRegionsChanged();

protected:

	/*!
		These are virtual functions in case a specific format or different information associated with the region is required.  The current
		way that they are stored is as a comma separated string with an ID tag as the first element and then the following pieces of the region
		in the following order:

		   1) Region starting point.
		   2) Region delta between points.
		   3) Region ending point.
		   4) Whether the region uses elastic starting points.
		   5) Whether the region uses elastic ending points.
		   6) The time spent on each point in the region.
		   7) The units for the scanned element of the region.
		   8) The units of time spent on each point.
	  */
	/// This returns a string that describes the current regions. Regions are stored in order as common separated start, delta, and end values. New lines divide regions. Empty string is no regions.
	virtual QString dbReadRegions() const;
	/// When loadFromDb() is called, this receives the string describing the regions and restores them.
	virtual void dbLoadRegions(const QString &regionsString);

protected:
	/// Holds the list of AMXASRegion pointers.
	AMRegionsList *regions_;
};

#endif // AMREGIONSCANCONFIGURATION_H
