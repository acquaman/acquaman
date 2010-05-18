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

public:
	/// Constructor, only requires a QObject for a parent
	AMRegion(QObject *parent = 0);
	/// Returns the stored start value as a double
	virtual double start() const { return start_;}
	/// Returns the stored delta value as a double
	virtual double delta() const { return delta_;}
	/// Returns the stored end value as a double
	virtual double end() const { return end_;}
	/// Returns a pointer the AMControl this region is operating on
	virtual AMControl* control() const { return ctrl_;}

public slots:
	/// Sets the start value from the double passed in. Does not affect the AMControl directly.
	/// \todo A check on the limits of the AMControl?
	virtual bool setStart(double start) { start_ = start; return TRUE;}
	/// Sets the delta value from the double passed in. The value MUST BE non-zero, or function will return false. Does not affect the AMControl direclty.
	virtual bool setDelta(double delta);
	/// Sets the end value from the double passed in. Does not affect the AMControl directly.
	/// \todo A check on the limits of the AMControl?
	virtual bool setEnd(double end) { end_ = end; return TRUE;}
	/// Sets the AMControl for the region.
	virtual bool setControl(AMControl* ctrl) { ctrl_ = ctrl; return true;}

protected:
	/// Value for the beginning of the region.
	double start_;
	/// Stepping value for the region.
	double delta_;
	/// Value for the end of the region.
	double end_;
	/// AMControl for the region to step through.
	AMControl *ctrl_;
};

class AMRegionsListModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	/// Constructor, pass a pointer to a list of AMXASRegions to set up the model. Such a list can be easily returned with regionsPtr() function in AMXASScanConfiguration class.
	AMRegionsListModel(QObject *parent = 0);

	/// Returns the number of regions in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
	/// Returns "3" statically. There are always three fields in the region: start, delta, and end.
	int columnCount(const QModelIndex & /*parent*/) const { return 4; }
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// This allows editing of values within range (for ex: in a QTableView)
	Qt::ItemFlags flags(const QModelIndex &index) const;

	QList<AMRegion*> *regions(){return regions_;}

public slots:
	void setDefaultControl(AMControl* defaultControl){defaultControl_ = defaultControl;}

protected:
	/// Internal pointer to the list of AMXASRegion.
	QList<AMRegion*> *regions_;
	AMControl *defaultControl_;
};

/// AMXASRegion is an impementation of AMRegion designed to scan energy regions; therefore, the AMControl is passed into the constructor and must be a beamline energy control.
/*!
  Simply pass an AMControl pointer into the constructor, and this child class will always refer to the beamline energy.
  The setStart() and setEnd() are reimplemented to ensure that the energy passed into the region is within the energy limits of the beamline.
  The setControl(AMControl*) function has been castrated, it will always return false and no change will be made.
  */
class AMXASRegion : public AMRegion
{
Q_OBJECT

public:
	/// Constructor, takes an AMControl to act as the perminant control for this region, must be the beamline energy control.
	AMXASRegion(AMControl* beamlineEnergy, QObject *parent = 0) : AMRegion(parent) {ctrl_ = beamlineEnergy;}

public slots:
	/// Sets the start value from the double passed in. Checks for the validity of the input. Does not affect the beamline energy directly.
	bool setStart(double start);
	/// Sets the end value from the double passed in. Checks for the validity of the input. Does not affect the beamline energy directly.
	bool setEnd(double end);
	/// Castrated function, does nothing and returns false.
	bool setControl(AMControl *ctrl){Q_UNUSED(ctrl); return false;}
};

/// An AMXASRegionModel is used as an interface between any default model viewer in Qt and a list of AMXASRegion.
/*!
  Simply pass a pointer to a list of AMXASRegion into the constructor and the model will be automatically set up.
  \todo Move this up one level to ALL AMRegions and/or combine with the AMScanConfiguration classes (so they can be the model direclty).
  */

class AMXASRegionsListModel : public AMRegionsListModel{
Q_OBJECT

public:
	AMXASRegionsListModel(QObject *parent = 0) : AMRegionsListModel(parent) {}

	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());

public slots:
	void setEnergyControl(AMControl* energyControl){setDefaultControl(energyControl);}

protected:
};

#endif // ACQMAN_AMREGION_H
