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
	virtual bool setStart(double start) { start_ = start; qDebug() << "Trying setStart in AMRegion"; return TRUE;}
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

/*
  *
  *		Attempting to refactor
  *
  */

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

class AMRegionsList : public QObject
{
Q_OBJECT

public:
	AMRegionsList(QObject *parent = 0, bool setup = true);

	/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double start(size_t index) const;
	/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double delta(size_t index) const;
	/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double end(size_t index) const;
	AMRegionsListModel* model(){return regions_;}
	int count(){return regions_->rowCount(QModelIndex());}

	virtual bool setupModel();

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setStart(size_t index, double start){ return regions_->setData(regions_->index(index, 1), start, Qt::EditRole);}

	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setDelta(size_t index, double delta){ return regions_->setData(regions_->index(index, 2), delta, Qt::EditRole);}

	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setEnd(size_t index, double end){ return regions_->setData(regions_->index(index, 3), end, Qt::EditRole);}

	/// Pure virtual function. Should be implemented in beamline specific subclasses as a convenience function for above.
	/// Creates a new region using start, delta, and end values then calls addRegion(index, *region).
	virtual bool addRegion(size_t index, double start, double delta, double end);

	/// Deletes the region refered to by index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(size_t index){ return regions_->removeRows(index, 1);}
	void setDefaultControl(AMControl* defaultControl){defaultControl_ = defaultControl; regions_->setDefaultControl(defaultControl);}

private slots:
	bool setRegion(size_t index, AMRegion *region){;}
//	/// Adds a region into position refered to by index and renumbers subsequent regions accordingly. Returns true if successful, returns false if the index is invalid.
//	virtual bool addRegion(size_t index, AMRegion *region){;}

protected:
	/// Holds the list of AMXASRegion pointers.
//	QList<AMRegion*> regions_;
	AMControl *defaultControl_;
	AMRegionsListModel *regions_;

//	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMRegion* region(size_t index) const;
};

/*
  *
  *
  *
  */

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
	AMXASRegionsListModel(QObject *parent = 0) : AMRegionsListModel(parent) {qDebug() << "Running XASRegionsListModel constructor";}

//	/// Returns the number of regions in the list to generate the number of rows in a table or list
//	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
//	/// Returns "3" statically. There are always three fields in the region: start, delta, and end.
//	int columnCount(const QModelIndex & /*parent*/) const { return 4; }
//	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
//	QVariant data(const QModelIndex &index, int role) const;
//	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
//	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
//	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
//	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
//	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
//	/// This allows editing of values within range (for ex: in a QTableView)
//	Qt::ItemFlags flags(const QModelIndex &index) const;

//
//	QList<AMXASRegion*> *regions(){return regions_;}
//

public slots:
	void setEnergyControl(AMControl* energyControl){setDefaultControl(energyControl);}

protected:
//	QList<AMXASRegion*> *regions_;
};

class AMXASRegionsList : public AMRegionsList{
Q_OBJECT

public:
	AMXASRegionsList(QObject *parent = 0, bool setup = true) : AMRegionsList(parent, false) {
		qDebug() << "Running XASRegionsList constructor";
		if(setup)
			setupModel();
}

	virtual bool setupModel();

public slots:
	virtual void setEnergyControl(AMControl* energyControl){defaultControl_ = energyControl; ((AMXASRegionsListModel*)regions_)->setEnergyControl(energyControl);}

protected:
//	AMXASRegionsListModel *regions_;

//	virtual bool setupModel();
};














class AMXASRegionModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	/// Constructor, pass a pointer to a list of AMXASRegions to set up the model. Such a list can be easily returned with regionsPtr() function in AMXASScanConfiguration class.
	AMXASRegionModel(QList<AMXASRegion*> *regions, QObject *parent = 0) : QAbstractTableModel(parent) {
		regions_ = regions;
		beamlineEnergy_ = NULL;
	}

	/// Returns the number of regions in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
	/// Returns "3" statically. There are always three fields in the region: start, delta, and end.
	int columnCount(const QModelIndex & /*parent*/) const { return 3; }
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// This allows editing of values within range (for ex: in a QTableView)
	Qt::ItemFlags flags(const QModelIndex &index) const;

	QList<AMXASRegion*> *regions(){return regions_;}

public slots:
	void setBeamlineEnergy(AMControl* beamlineEnergy){beamlineEnergy_ = beamlineEnergy;}

private:
	/// Internal pointer to the list of AMXASRegion.

	QList<AMXASRegion*> *regions_;
	AMControl *beamlineEnergy_;
};

#endif // ACQMAN_AMREGION_H
