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


#ifndef AM_AMREGIONSLIST_H
#define AM_AMREGIONSLIST_H

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
 	virtual ~AMRegionsList();
	AMRegionsList(QObject *parent = 0, bool setup = true);

	/// Returns the start value of the region referred to by index. If an invalid index is given, returns -1 (not a valid energy value).
	virtual double start(int index) const;
	/// Returns the delta value of the region referred to by index. If an invalid index is given, returns 0 (not a valid delta value).
	virtual double delta(int index) const;
	/// Returns the end value of the region referred to by index. If an invalid index is given, returns -1 (not a valid energy value).
	virtual double end(int index) const;
	/// Returns the time value of the region referred to by \param index.  If an invalid index is given, returns -1 (not a valid time value).
	virtual double time(int index) const;
	/// Returns the units for the region referred to by \param index.  If an invalid index is given, returns a null string.
	QString units(int index) const;
	/// Returns teh time units for the region referred to by \param index.  If an invalid index is given, returns a null string.
	QString timeUnits(int index) const;
	/// Returns whether elastic start is enabled for the region referred to by \param region.  False is returned if an invalid index is given as well as if it is not enabled.
	bool elasticStart(int index) const;
	/// Returns whether elastic end is enabled for the region referred to by \param region.  False is returned if an invalid index is given as well as if it is not enabled.
	bool elasticEnd(int index) const;
	/// Returns whether the region referred to by \param index is valid.
	bool isValid(int index) const { return regions_->regions()->at(index)->isValid(); }
	/// Returns whether the regions list is valid or not.  Returns true only when ALL regions are valid, returns false otherwise.
	bool isValid() const;
	/// Returns the model being managed by this list.
	AMRegionsListModel* model() const { return regions_; }
	/// Returns the number of elements in the list.
	int count() const { return regions_->rowCount(QModelIndex()); }
	/// Returns the default control used by this list to move from the start to the end of the region.
	AMControl* defaultControl() const { return defaultControl_; }
	/// Returns the default time control used by this list for the dwell time of each point.
	AMControl *defaultTimeControl() const { return defaultTimeControl_; }
	/// Returns the default units for this region.  If no default has been set then an empty string is returned.
	QString defaultUnits() const { return defaultUnits_; }
	/// Returns the default time units for this region.  If no default has been set then an empty string is returned.
	QString defaultTimeUnits() const { return defaultTimeUnits_; }

	/// Returns the sensible start position.
	double sensibleStart() const { return sensibleStart_; }
	/// Returns the sensible end position.
	double sensibleEnd() const { return sensibleEnd_; }

	/// Calculates the total time required to acquire all the regions. The base implementation assumes compatible/identical time units.
	virtual double totalAcquisitionTime() const;

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setStart(int index, double start) { return regions_->setData(regions_->index(index, 1), start, Qt::EditRole); }
	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setDelta(int index, double delta) { return regions_->setData(regions_->index(index, 2), delta, Qt::EditRole); }
	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setEnd(int index, double end) { return regions_->setData(regions_->index(index, 3), end, Qt::EditRole); }
	/// Sets the time value for the region referred to by index.  Returns true if successful, returns false if the index is invalid or the time is negative.
	bool setTime(int index, double time) { return regions_->setData(regions_->index(index, 7), time, Qt::EditRole); }
	/// Sets the units string for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setUnits(int index, const QString &units)
	{
		if (index < regions_->regions()->size())
			return regions_->regions()->at(index)->setUnits(units);

		return false;
	}
	/// Sets the time units string for the region referred to by \param index.  Returns true if successful, returns false if the index is invalid.
	bool setTimeUnits(int index, const QString &units)
	{
		if (index < regions_->regions()->size())
			return regions_->regions()->at(index)->setTimeUnits(units);

		return false;
	}
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
	/// Sets the default units.  These units are set to every new region that is created.
	void setDefaultUnits(const QString &units) { defaultUnits_ = units; }
	/// Sets the default time units.  These units are set to every new region that is created.
	void setDefaultTimeUnits(const QString &units) { defaultTimeUnits_ = units; }

	/// Sets the value for sensible start values.
	void setSensibleStart(double val) { sensibleStart_ = val; }
	/// Sets the value for the sensible end values.
	void setSensibleEnd(double val) { sensibleEnd_ = val; }
	/// Sets the range for sensible values.
	void setSensibleRange(double start, double end) { sensibleStart_ = start; sensibleEnd_ = end; }

	/// Returns the maximum energy from all the regions.
	virtual double maximumValue(){

		double curMax = -1e12;

		for(int x = 0; x < count(); x++){

			if(start(x) > curMax)
				curMax = start(x);

			if(end(x) > curMax)
				curMax = end(x);
		}

		return curMax;
	}

	/// Returns the minimum energy from all the regions.
	virtual double minimumValue(){

		double curMin = 1e12;

		for(int x = 0; x < count(); x++){

			if(start(x) < curMin)
				curMin = start(x);

			if(end(x) < curMin)
				curMin = end(x);
		}

		return curMin;
	}

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
	AMRegion* region(int index) const { return (index < regions_->regions()->size() && index >= 0) ? regions_->regions()->at(index) : 0; }

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

	/// String holding the default units for this regions list.
	QString defaultUnits_;
	/// String holding the default time units for this regions list.
	QString defaultTimeUnits_;
};

/// This class subclasses the AMRegionsList class to add some functionality specific to AMXASRegions.  Calls its own setupModel() to setup AMXASRegions instead of generic AMRegions.
class AMXASRegionsList : public AMRegionsList{
Q_OBJECT

public:
	/// Constructor.  Sets up its own regions model.
 	virtual ~AMXASRegionsList();
	AMXASRegionsList(QObject *parent = 0, bool setup = true) : AMRegionsList(parent, false) { if(setup) setupModel(); }

public slots:
	/// Sets the energy control for the AMXASRegions.  Also sets the default control for the regions list.
	virtual void setEnergyControl(AMControl* energyControl) { defaultControl_ = energyControl; ((AMXASRegionsListModel*)regions_)->setEnergyControl(energyControl); }
	/// Sets the time control for the AMXASRegions.  Also sets the default time control for the regions list.
	virtual void setTimeControl(AMControl *timeControl) { defaultTimeControl_ = timeControl; ((AMXASRegionsListModel *)regions_)->setTimeControl(timeControl); }

protected:
	/// Function used to setup the model that the list should manage.  When subclassed, call in the constructor to build your own custom model.
	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMXASRegion* xasRegion(int index) const { return (index < regions_->regions()->size() && index >= 0) ? qobject_cast<AMXASRegion *>(regions_->regions()->at(index)) : 0; }
};

/// This class subclasses the AMXASRegionsList class to add even more functionality specific to AMEXAFSRegions.  Calls its own setupModel() to setup AMEXAFSRegions instead of AMXASRegions.
class AMEXAFSRegionsList : public AMXASRegionsList{
Q_OBJECT

public:
	/// Constructor.  Sets up its own regions model.
 	virtual ~AMEXAFSRegionsList();
	AMEXAFSRegionsList(QObject *parent = 0, bool setup = true)
		: AMXASRegionsList(parent, false)
	{
		defaultEdgeEnergy_ = 0;
		defaultKControl_ = 0;
		defaultIsRelative_ = false;

		if(setup)
			setupModel();
	}

	/// Returns the type of the region referred to by \param index.
	AMEXAFSRegion::RegionType type(int index) const;
	/// Returns the edge energy of the region referred to by \param index.
	double edgeEnergy(int index) const;
	/// Returns whether the edge energy of the region referred to by \param index uses relative energy or not.
	bool isRelative(int index) const { return exafsRegion(index)->isRealtive(); }
	/// Returns the energy units for the region referred to by \param index.  This is so that the units can be retrieved even if the region is in k-space.
	QString energyUnits(int index) const { return exafsRegion(index)->energyUnits(); }
	/// Explicit getter based on the type passed into the function.  Returns the start value as a double from the region referred to by \param index.
	double startByType(int index, AMEXAFSRegion::RegionType type) { return exafsRegion(index)->startByType(type); }
	/// Explicit getter based on the type passed into the function.  Returns the end value as a double from the region referred to by \param index.
	double endByType(int index, AMEXAFSRegion::RegionType type) { return exafsRegion(index)->endByType(type); }

	/// Returns the default k-control used by this list to move from the start to the end of the region in k-space.
	AMControl* defaultKControl() const { return defaultKControl_; }
	/// Returns the default edge energy for the regions list.  This is the energy that is set as the edge energy to all newly created regions.
	double defaultEdgeEnergy() const { return defaultEdgeEnergy_; }
	/// Returns the default of whether the region is relative or absolute.
	bool defaultIsRelative() const { return defaultIsRelative_; }

	/// Returns whether any of the regions are in the extended region (k-space).
	bool hasKSpace() const
	{
		for (int i = 0; i < count(); i++)
			if (exafsRegion(i)->type() == AMEXAFSRegion::kSpace)
				return true;

		return false;
	}

signals:
	/// Notifier that the regions list contains k-space regions.
	void regionsHaveKSpaceChanged(bool);

public slots:
	/// Sets the type of the region referred to by \param index.
	bool setType(int index, AMEXAFSRegion::RegionType type) { return regions_->setData(regions_->index(index, 8), (type == AMEXAFSRegion::Energy) ? true : false, Qt::EditRole); }
	/// Sets the edge energy of the region referred to by \param index.
	bool setEdgeEnergy(int index, double energy) { return regions_->setData(regions_->index(index, 9), energy, Qt::EditRole); }
	/// Sets whether the region referred to by \param index uses relative or absolute energy units.
	bool setRelative(int index, bool isRelative) { return exafsRegion(index)->setRelative(isRelative); }
	/// Sets the start value for the region referred to by \param index from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
	bool setStartByType(int index, double start, AMEXAFSRegion::RegionType type) { return exafsRegion(index)->setStartByType(start, type); }
	/// Sets the end value for the region referred to by \param index from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
	bool setEndByType(int index, double end, AMEXAFSRegion::RegionType type) { return exafsRegion(index)->setEndByType(end, type); }

	/// Overloaded for EXAFS.  Creates a new region at \param index and auto fills the start and end values to start and end values of the regions that surround it.  Uses values defined by sensibleStart() and sensibleEnd() when the new region is prepended or appended to the current list.
	virtual bool addRegionSqueeze(int index);
	/// Deletes the region referred to by \param index and renumbers subsequent regions accordingly.  Returns true if successful, returns false if the index is invalid.  It also makes an intelligent change to the start and end values of the surrounding regions to push them together.
	bool deleteRegionSqueeze(int index);

	/// Sets the default edge energy for the regions list.  This is the energy that is set as the edge energy to all newly created regions.
	void setDefaultEdgeEnergy(double energy) { defaultEdgeEnergy_ = energy; ((AMEXAFSRegionsListModel *)regions_)->setDefaultEdgeEnergy(energy); }
	/// Sets the k-space control for the AMEXAFSRegions.  Also sets the default control for the regions list.
	virtual void setKControl(AMControl* kControl) { defaultKControl_ = kControl; ((AMEXAFSRegionsListModel*)regions_)->setKSpaceControl(kControl); }
	/// Sets the default for whether the regions are relative or not.
	void setDefaultIsRelative(bool isRelative) { defaultIsRelative_ = isRelative; ((AMEXAFSRegionsListModel *)regions_)->setDefaultIsRelative(isRelative); }

	/// Returns the maximum energy from all the regions.
	virtual double maximumValue(){

		double curMax = -1e12;

		for(int x = 0; x < count(); x++){

			if(startByType(x, AMEXAFSRegion::Energy) > curMax)
				curMax = startByType(x, AMEXAFSRegion::Energy);

			if(endByType(x, AMEXAFSRegion::Energy) > curMax)
				curMax = endByType(x, AMEXAFSRegion::Energy);
		}

		return curMax;
	}

	/// Returns the minimum energy from all the regions.
	virtual double minimumValue(){

		double curMin = 1e12;

		for(int x = 0; x < count(); x++){

			if(startByType(x, AMEXAFSRegion::Energy) < curMin)
				curMin = startByType(x, AMEXAFSRegion::Energy);

			if(endByType(x, AMEXAFSRegion::Energy) < curMin)
				curMin = endByType(x, AMEXAFSRegion::Energy);
		}

		return curMin;
	}

protected slots:
	/// Helper slot that emits the regionsHaveKSpaceChanged signal.
	void onTypeChanged() { emit regionsHaveKSpaceChanged(hasKSpace()); }

protected:
	/// Function used to setup the model that the list should manage.  When subclassed, call in the constructor to build your own custom model.
	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMEXAFSRegion* exafsRegion(int index) const { return (index < regions_->regions()->size() && index >= 0) ? qobject_cast<AMEXAFSRegion *>(regions_->regions()->at(index)) : 0; }

	/// Holds the default edge energy.  Used when making new regions.
	double defaultEdgeEnergy_;
	/// Pointer to the control that moves regions through k-space.
	AMControl *defaultKControl_;
	/// Flag holding whether the regions are relative or not.
	bool defaultIsRelative_;
};


#endif // AMREGIONSLIST_H
