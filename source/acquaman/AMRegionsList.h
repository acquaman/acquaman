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


#ifndef ACQMAN_AMREGIONSLIST_H
#define ACQMAN_AMREGIONSLIST_H

#include <QObject>

#include "AMRegion.h"

/*!
  This class manages the AMRegionsListModel and also handles connecting the different AMRegions together.  For instance, if the start and end for a region
  are elastic then it handles making those connections to make sure that they behave properly.  It also provides the possibility of selecting whether to
  setup the model using AMRegionsListModel or if it is subclassed, the subclasses model.
  */
class AMRegionsList : public QObject
{
Q_OBJECT

public:
	/// Constructor.  Can choose to setup the model.  Valuable when you want subclases to setup their own model.
	AMRegionsList(QObject *parent = 0, bool setup = true);

	/// Returns the start value of the region referred to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double start(int index) const;
	/// Returns the delta value of the region referred to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double delta(int index) const;
	/// Returns the end value of the region referred to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double end(int index) const;
	/// Returns the time value of the region referred to by \param index.  If an invalid index is given, returns -1 (not a valid time value).
	double time(int index) const;
	/// Returns whether elastic start is enabled for the region referred to by \param region.  False is returned if an invalid index is given as well as if it is not enabled.
	bool elasticStart(int index) const;
	/// Returns whether elastic end is enabled for the region referred to by \param region.  False is returned if an invalid index is given as well as if it is not enabled.
	bool elasticEnd(int index) const;
	/// Returns the model being managed by this list.
	AMRegionsListModel* model() { return regions_; }
	/// Returns the number of elements in the list.
	int count() { return regions_->rowCount(QModelIndex()); }
	/// Returns the default control used by this list to move from the start to the end of the region.
	AMControl* defaultControl() { return defaultControl_; }
	/// Returns the default time control used by this list for the dwell time of each point.
	AMControl *defaultTimeControl() { return defaultTimeControl_; }

	/// Returns the sensible start position.
	double sensibleStart() const { return sensibleStart_; }
	/// Returns the sensible end position.
	double sensibleEnd() const { return sensibleEnd_; }

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setStart(int index, double start) { return regions_->setData(regions_->index(index, 1), start, Qt::EditRole); }
	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setDelta(int index, double delta) { return regions_->setData(regions_->index(index, 2), delta, Qt::EditRole); }
	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setEnd(int index, double end) { return regions_->setData(regions_->index(index, 3), end, Qt::EditRole); }
	/// Sets the time value for the region referred to by index.  Returns true if successful, returns false if the index is invalid or the time is negative.
	bool setTime(int index, double time) { return regions_->setData(regions_->index(index, 7), time, Qt::EditRole); }
	/// Sets the elastic start state for the region referred to by \param index. Returns true if successful, returns false if the index is invalid.
	bool setElasticStart(int index, bool state) { return regions_->setData(regions_->index(index, 4), state, Qt::EditRole); }
	/// Sets the elastic end state for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setElasticEnd(int index, bool state) { return regions_->setData(regions_->index(index, 5), state, Qt::EditRole); }

	/// Creates a new region using start, delta, end, and time values then calls addRegion(index, *region).
	virtual bool addRegion(int index, double start, double delta, double end, double time);
	/// Overloaded.  Creates a new region using start, delta, and end values.  It sets the time to the previous region's time or 1 second if this is the first region to be added.
	virtual bool addRegion(int index, double start, double delta, double end);
	/// Creates a new region at \param index and auto fills the start and end values to start and end values of the regions that surround it.  Uses values defined by sensibleStart() and sensibleEnd() when the new region is prepended or appended to the current list.
	virtual bool addRegionSqueeze(int index);
	/// Creates a new region using start,  delta, end, and time values and adds it to the end of the current list.
	virtual bool appendRegion(double start, double delta, double end, double time) { return addRegion(count(), start, delta, end, time); }
	/// Overloaded.  Creates a new region using start, delta, and end values.  It sets the time to the previous region's time or 1 second if this is the first region to be appended.
	virtual bool appendRegion(double start, double delta, double end) { return count() == 0 ? addRegion(0, start, delta, end, 1) : addRegion(count(), start, delta, end, time(count() - 1)); }

	/// Deletes the region referred to by \param index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(int index);
	/// Deletes the region referred to by \param index and renumbers subsequent regions accordingly.  Returns true if successful, returns false if the index is invalid.  It also makes an intelligent change to the start and end values of the surrounding regions to push them together.
	bool deleteRegionSqueeze(int index);
	/// Sets the default control used for moving through the regions and also passes the control to the model it is managing.
	void setDefaultControl(AMControl* defaultControl) { defaultControl_ = defaultControl; regions_->setDefaultControl(defaultControl); }
	/// Sets the default time control used for time dwelling on each point and also passes the control to the model it is managing.
	void setDefaultTimeControl(AMControl *defaultTimeControl) { defaultTimeControl_ = defaultTimeControl; regions_->setDefaultTimeControl(defaultTimeControl); }

	/// Sets the value for sensible start values.
	void setSensibleStart(double val) { sensibleStart_ = val; }
	/// Sets the value for the sensible end values.
	void setSensibleEnd(double val) { sensibleEnd_ = val; }
	/// Sets the range for sensible values.
	void setSensibleRange(double start, double end) { sensibleStart_ = start; sensibleEnd_ = end; }

signals:
	/// Notifier that the data contained within the model has changed.
	void regionsChanged();

private slots:
	/// Slot that handles passing on the signal that the regions have changed.
	void onDataChanged(QModelIndex a,QModelIndex b) { Q_UNUSED(a); Q_UNUSED(b); emit regionsChanged();}

protected:
	/// Function used to setup the model that the list should manage.  When subclassed, call in the constructor to build your own custom model.
	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMRegion* region(int index) const;

	/// Pointer to the control used by these regions.
	AMControl *defaultControl_;
	/// Pointer to the control used for setting the time in these regions.
	AMControl *defaultTimeControl_;
	/// Pointer to the general model this list manages.
	AMRegionsListModel *regions_;

	/// A minimum value that can be used for intelligent energy selection in the XXXSqueeze functions.
	double sensibleStart_;
	/// A maximum value that can be used for intelligent energy selection in the XXXSqueeze functions.
	double sensibleEnd_;
};

/// This class subclasses the AMRegionsList class to add some functionality specific to AMXASRegions.  Calls its own setupModel() to setup AMXASRegions instead of generic AMRegions.
class AMXASRegionsList : public AMRegionsList{
Q_OBJECT

public:
	/// Constructor.  Sets up its own regions model.
	AMXASRegionsList(QObject *parent = 0, bool setup = true) : AMRegionsList(parent, false) { if(setup) setupModel(); }

public slots:
	/// Sets the energy control for the AMXASRegions.  Also sets the default control for the regions list.
	virtual void setEnergyControl(AMControl* energyControl) { defaultControl_ = energyControl; ((AMXASRegionsListModel*)regions_)->setEnergyControl(energyControl); }
	/// Sets the time control for the AMXASRegions.  Also sets the default time control for the regions list.
	virtual void setTimeControl(AMControl *timeControl) { defaultTimeControl_ = timeControl; ((AMXASRegionsListModel *)regions_)->setTimeControl(timeControl); }

	/// Returns the maximum energy from all the regions.
	virtual double maxEnergy(){

		double curMax = 1e-12;

		for(int x = 0; x < count(); x++){

			if(start(x) > curMax)
				curMax = start(x);

			if(end(x) > curMax)
				curMax = end(x);
		}

		return curMax;
	}

	/// Returns the minimum energy from all the regions.
	virtual double minEnergy(){

		double curMin = 1e12;

		for(int x = 0; x < count(); x++){

			if(start(x) < curMin)
				curMin = start(x);

			if(end(x) < curMin)
				curMin = end(x);
		}

		return curMin;
	}

protected:
	/// Function used to setup the model that the list should manage.  When subclassed, call in the constructor to build your own custom model.
	virtual bool setupModel();
};

#endif // AMREGIONSLIST_H
