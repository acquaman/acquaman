#ifndef ACQMAN_AMREGIONSLIST_H
#define ACQMAN_AMREGIONSLIST_H

#include <QObject>

#include "AMRegion.h"

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
	AMControl* defaultControl(){ return defaultControl_; }

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
	virtual bool appendRegion(double start, double delta, double end){ return addRegion(count(), start, delta, end);}

	/// Deletes the region refered to by index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(size_t index){
		if((unsigned)index > (unsigned)count())
			return false;
		return regions_->removeRows(index, 1);
	}
	void setDefaultControl(AMControl* defaultControl){defaultControl_ = defaultControl; regions_->setDefaultControl(defaultControl);}

private slots:

protected:
	/// Holds the list of AMXASRegion pointers.
	AMControl *defaultControl_;
	AMRegionsListModel *regions_;

	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMRegion* region(size_t index) const;
};

class AMXASRegionsList : public AMRegionsList{
Q_OBJECT

public:
	AMXASRegionsList(QObject *parent = 0, bool setup = true) : AMRegionsList(parent, false) {
		if(setup)
			setupModel();
}

public slots:
	virtual void setEnergyControl(AMControl* energyControl){defaultControl_ = energyControl; ((AMXASRegionsListModel*)regions_)->setEnergyControl(energyControl);}

protected:
	virtual bool setupModel();
};

#endif // AMREGIONSLIST_H
