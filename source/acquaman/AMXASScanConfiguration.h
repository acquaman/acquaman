/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef ACQMAN_XASSCANCONFIGURATION_H
#define ACQMAN_XASSCANCONFIGURATION_H

#include <QObject>
#include "AMScanConfiguration.h"
#include "acquaman/AMRegionsList.h"
#include "dataman/info/AMControlInfoList.h"

/// An AMXASScanConfiguration is the parent class for any beamline that wants to implement a simple XAS (X-Ray Absorption Spectroscopy) scan.
/*!
  The AMXASScanConfiguration class is designed to allow beamline specific implementation classes to inherit it, but take advantage of a common method for configuring the energy regions.
  To this end, the class holds a QList of pointers to AMXASRegion.
  The AMXASRegion class inherits from AMRegion and optimizes the class to explicitly control beamline energy.
  Functions for adding and deleting regions, as well as editing regions are available.
  \todo Make a class on top of this (AMRegionScanConfiguration?) that uses just AMRegions? Then reimplement some of this...
  */
class AMXASScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(QString XASRegions READ dbReadXASRegions WRITE dbLoadXASRegions)
	Q_PROPERTY(double startEnergy READ startEnergy WRITE setStartEnergy)
	Q_PROPERTY(double endEnergy READ endEnergy WRITE setEndEnergy)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic XAS Scan Configuration")

public:
	/// Constructor, needs only a pointer to a QObject to act as a parent.
	AMXASScanConfiguration(QObject *parent = 0);
	AMXASScanConfiguration(const AMXASScanConfiguration &original);

	const QMetaObject* getMetaObject();

	/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double regionStart(int index) const { return regions_->start(index); }
	/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double regionDelta(int index) const { return regions_->delta(index); }
	/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double regionEnd(int index) const { return regions_->end(index); }
	/// Returns the time value of the region referred to by \param index.  If an invalid index is given, returns -1 (not a valid time value).
	double regionTime(int index) const { return regions_->time(index); }
	/// Returns whether elastic start is enabled for the region referred to by \param index.
	bool regionElasticStart(int index) const { return regions_->elasticStart(index); }
	/// Returns whether elastic end is enabled for the reigon referred to by \param index.
	bool regionElasticEnd(int index) const { return regions_->elasticEnd(index); }
	/// Returns the number of regions current in the scan configuration.
	int regionCount() const { return regions_->count(); }
	/// Returns the regions list.
	AMXASRegionsList* regions() { return regions_; }

	/// Quick accessor for the start of the first region. If no regions are set, returns -1
	double startEnergy() const;
	/// Quick accessor for the end of the final region. If no reginos are set, returns -1
	double endEnergy() const;

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by AMBeamlineScanAction to set the title for the action view.
	virtual QString description() const {
		return QString("XAS Scan from %1 to %2").arg(regionStart(0)).arg(regionEnd(regionCount()-1));
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("XAS Scan from %1 to %2").arg(regionStart(0)).arg(regionEnd(regionCount()-1));
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("XAS").arg(regionStart(0)).arg(regionEnd(regionCount()-1));
	}

	/// Pure virtual functions that need to be implemented.  Because controls aren't set in AMXASScanConfiguration, subclasses need to implement the control info getter to their valid energy and time controls.
	/// Returns the AMControlInfo for the energy control.
	virtual AMControlInfo energyControlInfo() const = 0;
	/// Returns the AMControlInfo for the time control.
	virtual AMControlInfo timeControlInfo() const = 0;

	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return "XAS";
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
	bool setStartEnergy(double startEnergy);
	/// Quick setter for the end of the final region. Returns false if there are no regions or the energy is out of range
	bool setEndEnergy(double endEnergy);

signals:
	/// Notifier that passes on the signal that current state of the regions have changed.
	void regionsChanged();

protected slots:
	/// Helper slot that handles notifying the database that the regions have changed and emits the regionsChanged() signal.
	void onRegionsChanged();

protected:
	/// This returns a string that describes the current regions. Regions are stored in order as common separated start, delta, and end values. New lines divide regions. Empty string is no regions.
	QString dbReadXASRegions() const;
	/// When loadFromDb() is called, this receives the string describing the regions and restores them.
	void dbLoadXASRegions(const QString &XASRegionsString);

protected:
	/// Holds the list of AMXASRegion pointers.
	AMXASRegionsList *regions_;
};

#endif // ACQMAN_XASSCANCONFIGURATION_H
