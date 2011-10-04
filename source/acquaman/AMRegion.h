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


#ifndef ACQMAN_AMREGION_H
#define ACQMAN_AMREGION_H

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

public:
	/// Constructor, only requires a QObject for a parent and defaults elastic start and end to false.
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
	AMRegionsListModel(QObject *parent = 0);

	/// Returns the number of regions in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
	/// Returns "4" statically. There are always three fields in the region: start, delta, end, and time.  However, the total number is 8.
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

protected:
	/// Internal pointer to the list of AMRegion.
	QList<AMRegion*> *regions_;
	/// Pointer to the default control used to create AMRegions.
	AMControl *defaultControl_;
	/// Pointer to the default time control used to create AMRegions.
	AMControl *defaultTimeControl_;
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
	AMXASRegion(AMControl* beamlineEnergy, QObject *parent = 0) : AMRegion(parent) { ctrl_ = beamlineEnergy; }

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
	AMXASRegionsListModel(QObject *parent = 0) : AMRegionsListModel(parent) {}

	/// Inserts an AMXASRegion into the model.  It builds a default AMXASRegion, sets the control to whatever the energy control is at the time.
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());

public slots:
	/// Sets the energy control that is used for scanning the energy in an XAS scan.
	void setEnergyControl(AMControl* energyControl) { setDefaultControl(energyControl); }
	/// Sets the time control that is used for dwelling on each point in an XAS scan.
	void setTimeControl(AMControl *timeControl) { setDefaultTimeControl(timeControl); }
};

#endif // ACQMAN_AMREGION_H
