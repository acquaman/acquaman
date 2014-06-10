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


#ifndef AM_AMREGION_H
#define AM_AMREGION_H

#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include "beamline/AMControl.h"

/*
  FIX ME UP! I want to be able to have elastic regions ... start value is last end plus this delta.
class ContinuousRegion
{
public:
	ContinuousRegion(){};
	double startVal_;
	QList<double> values_;
};
*/

/// An AMRegion is the generalized parent of all "regions" one can imagine scanning, it can be subclasses to achieve particular functionality.
/*!
  The AMRegion class provides the basic interface to a particular region in a scan. Most scans are made up of a list of back to back regions.
  As such, an AMRegion holds only a pointer to an AMControl (the device being scanned) as well as start, delta, and end values.
  The region should begin at the "start" value and move in discrete steps of "delta" until it hits the "end" value.
  It is imaginable that the amount of time spent per point in each region is highly configurable.  For now, you can set a certain dwell time for each region.
  The actual implementation is up to the scan library being used.
  Checks like making sure that the delta moves in the right direction (positive if end > start OR negative if end < start) is NOT taken care of here.
  Responsibility for such checks is passed onto higher level classes (like AMScanConfiguration).
  */
class AMRegion: public QObject
{
	Q_OBJECT

	Q_PROPERTY(double start READ start WRITE setStart)
	Q_PROPERTY(double delta READ delta WRITE setDelta)
	Q_PROPERTY(double end READ end WRITE setEnd)
	Q_PROPERTY(double time READ time WRITE setTime)
	Q_PROPERTY(QString units READ units WRITE setUnits)
	Q_PROPERTY(QString timeUnits READ timeUnits WRITE setTimeUnits)

public:
	/// Constructor, only requires a QObject for a parent and defaults elastic start and end to false.
 	virtual ~AMRegion();
	AMRegion(QObject *parent = 0);
	/// Returns the stored start value as a double
	virtual double start() const { return start_; }
	/// Returns the stored delta value as a double
	virtual double delta() const { return delta_; }
	/// Returns the stored end value as a double
	virtual double end() const { return end_; }
	/// Returns the time spent per point in the region.
	virtual double time() const { return time_; }
	/// Returns a pointer the AMControl this region is operating on
	virtual AMControl* control() const { return ctrl_; }
	/// Returns a pointer to the AMControl this region uses to set the time.
	virtual AMControl *timeControl() const { return timeControl_; }
	/// Returns the state of whether or not the start value of the region can be adjusted by the end of the adjacent region being changed.
	virtual bool elasticStart() const { return elasticStart_; }
	/// Returns the state of whether or not the end value of the region can be adjusted by the start of the adjacent region being changed.
	virtual bool elasticEnd() const { return elasticEnd_; }
	/// Returns whether this region is valid or not.  Invalid regions are those that cannot be completed based on current start, delta, and end values.
	virtual bool isValid() const;
	/// Returns the units that the region is expressed in.
	virtual QString units() const { return units_; }
	/// Returns the units for the time in this region.
	virtual QString timeUnits() const { return timeUnits_; }

public slots:
	/// Sets the start value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.  Does not affect the AMControl directly.
	virtual bool setStart(double start);
	/// Sets the delta value from the double passed in. The value MUST BE non-zero, or function will return false. Does not affect the AMControl direclty.
	virtual bool setDelta(double delta);
	/// Sets the end value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.	Does not affect the AMControl directly.
	virtual bool setEnd(double end);
	/// Sets the time value to \param time.  Makes sure the time given is not negative and returns false if it is.  Does not affect the time AMControl directly.
	virtual bool setTime(double time);
	/// This changes the start value of the region.  If the start value is already in the process of changing this function does nothing.
	virtual bool adjustStart(double start);
	/// This changes the end value of the region.  If the end value is already in the process of changing this function does nothing.
	virtual bool adjustEnd(double end);
	/// Sets the AMControl for the region.
	virtual bool setControl(AMControl* ctrl) { ctrl_ = ctrl; return true; }
	/// Sets the time AMControl for the region.
	virtual bool setTimeControl(AMControl *control) { timeControl_ = control; return true; }
	/// Sets the state of whether the start value can be adjusted through changes to the end of its adjacent region.
	virtual bool setElasticStart(bool elastic) { elasticStart_ = elastic; return true; }
	/// Sets the state of whether the end value can be adjusted through changes to the start of its adjacent region.
	virtual bool setElasticEnd(bool elastic) { elasticEnd_ = elastic; return true; }
	/// Sets the units that the region will be expressed in.
	virtual bool setUnits(const QString &units);
	/// Sets the time units that the region will be expressed in.
	virtual bool setTimeUnits(const QString &units);

signals:
	/// Notifier that the start value has changed.  Only used if elastic start is enabled.
	void startChanged(double start);
	/// Notifier that the end value has changed.  Only used if elastic end is enabled.
	void endChanged(double end);

protected:
	/// Value for the beginning of the region.
	double start_;
	/// Stepping value for the region.
	double delta_;
	/// Value for the end of the region.
	double end_;
	/// Value for the time spent per point in the region.
	double time_;
	/// QString for holding the units.
	QString units_;
	/// QString for holding the time units.
	QString timeUnits_;
	/// AMControl for the region to step through.
	AMControl *ctrl_;
	/// AMControl for the time of the region.
	AMControl *timeControl_;
	/// Flag used to determine if the start value can be adjusted through changes to other regions.
	bool elasticStart_;
	/// Flag used to determine if the end value can be adjusted through changes to other regions.
	bool elasticEnd_;
	/// Flag used to determine if the start value is about to be changed by another region.
	bool initiatedStartAdjust_;
	/// Flag used to determine if the end vallue is about to be changed by another region.
	bool initiatedEndAdjust_;
};

class AMRegionsListModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	/// Constructor, pass a pointer to a list of AMXASRegions to set up the model. Such a list can be easily returned with regionsPtr() function in AMXASScanConfiguration class.
 	virtual ~AMRegionsListModel();
	AMRegionsListModel(QObject *parent = 0);

	/// Returns the number of regions in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
	/// Returns "4" statically. There are always four fields in the region: start, delta, end, and time.  However, the total number is 8.
	int columnCount(const QModelIndex & /*parent*/) const { return 8; }
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	/// Inserts an AMRegion into the model.  It builds a default AMRegion, sets the control to whatever the default control is at the time.
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// Removes an AMRegion from the model.
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// This allows editing of values within range (for ex: in a QTableView)
	Qt::ItemFlags flags(const QModelIndex &index) const;
	/// Returns the current list of AMRegions in the model.
	QList<AMRegion*> *regions() { return regions_; }

public slots:
	/// Sets the default control.  It is used for setting the control when inserting rows.
	void setDefaultControl(AMControl* defaultControl) { defaultControl_ = defaultControl; }
	/// Sets the default time control.  It is used for setting the time control when inserting rows.
	void setDefaultTimeControl(AMControl *defaultTimeControl) { defaultTimeControl_ = defaultTimeControl; }
	/// Sets the default units for the region.  It is used for setting the units when inserting rows.
	void setDefaultUnits(const QString &units) { defaultUnits_ = units; }
	/// Sets the default time units for the region.  It is used for setting the time units when inserting rows.
	void setDefaultTimeUnits(const QString &units) { defaultTimeUnits_ = units; }

protected:
	/// Internal pointer to the list of AMRegion.
	QList<AMRegion*> *regions_;
	/// Pointer to the default control used to create AMRegions.
	AMControl *defaultControl_;
	/// Pointer to the default time control used to create AMRegions.
	AMControl *defaultTimeControl_;
	/// Holds the default units used when creating AMRegions.
	QString defaultUnits_;
	/// Holds the default time units used when creating AMRegions.
	QString defaultTimeUnits_;
};

/// AMXASRegion is an implementation of AMRegion designed to scan energy regions; therefore, the AMControl is passed into the constructor and must be a beamline energy control.
/*!
  Simply pass an AMControl pointer into the constructor, and this child class will always refer to the beamline energy.
  \todo The setStart() and setEnd() are reimplemented to ensure that the energy passed into the region is within the energy limits of the beamline.
  The setControl(AMControl*) function has been castrated, it will always return false and no change will be made.
  */
class AMXASRegion : public AMRegion
{
Q_OBJECT

public:
	/// Constructor, takes an AMControl to act as the perminant control for this region, must be the beamline energy control.
 	virtual ~AMXASRegion();
	AMXASRegion(AMControl* beamlineEnergy, QObject *parent = 0) : AMRegion(parent) { ctrl_ = beamlineEnergy; }
	/// Re-implemented.  Forcing XAS scans to start from lower energy and go to higher energy.
	virtual bool isValid() const { return (start_ < end_ && delta_ > 0) ? true : false; }

public slots:
	/// Castrated function, does nothing and returns false.
	bool setControl(AMControl *ctrl) { Q_UNUSED(ctrl); return false; }
};

/// An AMXASRegionModel is used as an interface between any default model viewer in Qt and a list of AMXASRegion.
/*!
  This model reimplements the insert rows function to build AMXASRegions instead of generic AMRegions.
  \todo Move this up one level to ALL AMRegions and/or combine with the AMScanConfiguration classes (so they can be the model directly).
  */

class AMXASRegionsListModel : public AMRegionsListModel{
Q_OBJECT

public:
	/// Constructor.  Builds a model that is identical to AMRegionsListModel.  No new features added.
 	virtual ~AMXASRegionsListModel();
	AMXASRegionsListModel(QObject *parent = 0) : AMRegionsListModel(parent) {}

	/// Inserts an AMXASRegion into the model.  It builds a default AMXASRegion, sets the control to whatever the energy control is at the time.
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());

public slots:
	/// Sets the energy control that is used for scanning the energy in an XAS scan.
	void setEnergyControl(AMControl* energyControl) { setDefaultControl(energyControl); }
	/// Sets the time control that is used for dwelling on each point in an XAS scan.
	void setTimeControl(AMControl *timeControl) { setDefaultTimeControl(timeControl); }
};

/// An AMEXAFSRegion is an implementation of AMRegion designed to scan energy regions using either energy-space or k-space; therefore, the two AMControls that are passed into the constructor must be a beamline energy control and a beamline k-space energy control.
/*!
  Pass an AMControl pointer for both energy and k-space into the constructor and this child class will always refer to the appropriate control.
  Like AMXASRegion, the setControl(AMControl *) function remains castrated.  It will always return false and no change will be made.
  Because it extends the functionality of AMXASRegion, it subclasses it instead of AMRegion.
  Region has a type that defaults to Energy.
  All values are stored in energy space (eV).  That's why many of the getters and setters have explicit type arguments based on what you need.
  */
class AMEXAFSRegion : public AMXASRegion
{
	Q_OBJECT

	Q_PROPERTY(double edgeEnergy READ edgeEnergy WRITE setEdgeEnergy)
	Q_PROPERTY(RegionType type READ type WRITE setType)

	Q_ENUMS(RegionType)

public:
	/// Enum that defines what type of region it is.  Possibilities are Energy and kSpace.
	enum RegionType { Energy, kSpace };

	/// Constructor.  Takes two AMControls to act as the perminant energy and k-space control for this region.  Must be the beamline energy and beamline k-space control.
 	virtual ~AMEXAFSRegion();
	AMEXAFSRegion(AMControl *beamlineEnergy, AMControl *beamlineK, QObject *parent = 0)
		: AMXASRegion(beamlineEnergy, parent)
	{
		type_ = Energy;
		controlK_ = beamlineK;
		edgeEnergy_ = 0;
		isRelative_ = false;
	}

	/// Re-implemented.  Forcing XAS scans to start from lower energy and go to higher energy.
	virtual bool isValid() const
	{
		if (type_ == Energy && start_ < end_ && delta_ > 0)
			return true;

		else if (type_ == kSpace && start_ < end_ && delta_ > 0 && start_ >= 0)
			return true;

		return false;
	}

	/// Returns the stored start value as a double.  Returns value either as energy or k-space based on the current value of type.
	virtual double start() const { return (type() == Energy) ? start_ : toKSpace(start_); }
	/// Returns the stored end value as a double.  Returns value either as energy or k-space based on the current value of type.
	virtual double end() const { return (type() == Energy) ? end_ : toKSpace(end_); }
	/// Returns the time spent per point in the region.  In the case of k-space the time is the maximum time because the dwell time changes due to the an external app (currently).
	virtual double time() const { return time_; }
	/// Returns the units that the region is expressed in based on the type of region.  Returns whatever the energy units are set (likely eV) or "k".
	virtual QString units() const { return (type() == Energy) ? units_ : "k"; }
	/// Explicitly returns the units for energy space.
	QString energyUnits() const { return units_; }

	/// Returns whether the region energy is relative or absolute.
	bool isRealtive() const { return isRelative_; }

	/// Explicit getter based on the type passed into the function.  Returns the start value as a double.
	double startByType(RegionType type) { return (type == Energy) ? start_ : toKSpace(start_); }
	/// Explicit getter based on the type passed into the function.  Returns the end value as a double.
	double endByType(RegionType type) { return (type == Energy) ? end_ : toKSpace(end_); }
	/// Explicit getter based on the type passed into the function.  Returns the units.
	QString unitsByType(RegionType type) { return (type == Energy) ? units_ : "k"; }

	/// Returns the region type.
	AMEXAFSRegion::RegionType type() const { return type_; }
	/// Returns the edge energy for the list model.  Used when computing k <-> eV.
	double edgeEnergy() const { return edgeEnergy_; }

	/// Returns the control this region is using for scanning based on the current region type.
	AMControl *control() const { return (type_ == Energy) ? ctrl_ : controlK_; }

signals:
	/// Notifier that the type of the region has been changed.  Returns the new type.
	void typeChanged(RegionType);

public slots:
	/// Sets the start value from the double passed in. Assumes the value passed in is in the space of the region.  Makes sure the energy is within the allowable range, otherwise returns false.  Does not affect the AMControl directly.
	virtual bool setStart(double start);
	/// Sets the end value from the double passed in. Assumes the value passed in is in the space of the region.  Makes sure the energy is within the allowable range, otherwise returns false.	Does not affect the AMControl directly.
	virtual bool setEnd(double end);

	/// Sets the start value from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
	bool setStartByType(double start, RegionType type);
	/// Sets the end value from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
	bool setEndByType(double end, RegionType type);

	/// Sets the region type.
	bool setType(AMEXAFSRegion::RegionType type);
	/// Sets the edge energy for the list model.  Used when computing k <-> eV.  Must be an absolute energy.
	bool setEdgeEnergy(double energy)
	{
		if (energy >= 0){

			edgeEnergy_ = energy;
			return true;
		}

		return false;
	}

	/// Overloaded to accommodate the type difference for EXAFS.  This changes the start value of the region.  If the start value is already in the process of changing this function does nothing.
	virtual bool adjustStart(double start);
	/// Overloaded to accommodate the type difference for EXAFS.  This changes the end value of the region.  If the end value is already in the process of changing this function does nothing.
	virtual bool adjustEnd(double end);

	/// Sets whether the region is relative.
	bool setRelative(bool isRelative) { isRelative_ = isRelative; return true; }

protected:
	/// Returns the k-space value from \param energy using the current edge energy.  Returns -1 if invalid.
	double toKSpace(double energy) const;
	/// Returns the energy value fromm \param k using the current edge energy.  Returns -1 if invalid.
	double toEnergy(double k) const;

	/// The pointer to the k-space energy control.
	AMControl *controlK_;
	/// The type of region this is.
	RegionType type_;
	/// The edge energy, used for conversion between energy and k-space.  This assumes units of eV.
	double edgeEnergy_;
	/// Flag used to determine whether the energy PV is in absolute values or relative values.
	bool isRelative_;
};

/// An AMEXAFSRegionModel is used as an interface between any default model viewer in Qt and a list of AMEXAFSRegion.
/*!
  This model reimplements the insert rows function to build AMEXAFSRegions instead of generic AMRegions.
  \todo Move this up one level to ALL AMRegions and/or combine with the AMScanConfiguration classes (so they can be the model directly).
  */

class AMEXAFSRegionsListModel : public AMXASRegionsListModel{
Q_OBJECT

public:
	/// Constructor.  Builds a model that is identical to AMRegionsListModel.  No new features added.
 	virtual ~AMEXAFSRegionsListModel();
	AMEXAFSRegionsListModel(QObject *parent = 0) : AMXASRegionsListModel(parent) { defaultKControl_ = 0; defaultEdgeEnergy_ = 0; defaultIsRelative_ = false; }

	/// Returns "4" statically. There are always four fields in the region: start, delta, end, and time.  However, the total number is 10.
	int columnCount(const QModelIndex & /*parent*/) const { return 10; }
	/// Inserts an AMEXAFSRegion into the model.  It builds a default AMEXAFSRegion, sets the control to whatever the energy control is at the time.
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;

signals:
	/// Passes on that one of the regions' type has changed.
	void typeChanged();

public slots:
	/// Sets the k-space control that is used for scanning the energy in an EXAFS scan.  \note This sets the default control for the region.  If setEnergyControl was used previously, then it will be overwritten.
	void setKSpaceControl(AMControl* kSpaceControl) { defaultKControl_ = kSpaceControl; }
	/// Sets the default edge energy that all new AMEXAFSRegions will be set with.
	void setDefaultEdgeEnergy(double energy)
	{
		defaultEdgeEnergy_ = energy;

		for (int i = 0; i < regions_->size(); i++)
			((AMEXAFSRegion *)regions_->at(i))->setEdgeEnergy(energy);
	}
	/// Sets the default is relative to all new AMEXAFSRegions.
	void setDefaultIsRelative(bool isRelative) { defaultIsRelative_ = isRelative; }

protected:
	/// Pointer to the k-space control used to build AMEXAFSRegions.
	AMControl *defaultKControl_;
	/// Holds the default edge energy.  Used for conversions between energy and k-space for individual regions.
	double defaultEdgeEnergy_;
	/// Flag that holds whether the regions are relative or absolute in energy.
	bool defaultIsRelative_;
};

#endif // AM_AMREGION_H
