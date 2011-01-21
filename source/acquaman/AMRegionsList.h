/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

class AMRegionsList : public QObject
{
Q_OBJECT

public:
	AMRegionsList(QObject *parent = 0, bool setup = true);

	/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double start(int index) const;
	/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double delta(int index) const;
	/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double end(int index) const;
	AMRegionsListModel* model(){return regions_;}
	int count(){return regions_->rowCount(QModelIndex());}
	AMControl* defaultControl(){ return defaultControl_; }

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setStart(int index, double start){ return regions_->setData(regions_->index(index, 1), start, Qt::EditRole);}

	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setDelta(int index, double delta){ return regions_->setData(regions_->index(index, 2), delta, Qt::EditRole);}

	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setEnd(int index, double end){ return regions_->setData(regions_->index(index, 3), end, Qt::EditRole);}

	/// Pure virtual function. Should be implemented in beamline specific subclasses as a convenience function for above.
	/// Creates a new region using start, delta, and end values then calls addRegion(index, *region).
	virtual bool addRegion(int index, double start, double delta, double end);
	virtual bool addRegionSqueeze(int index);
	virtual bool appendRegion(double start, double delta, double end){ return addRegion(count(), start, delta, end);}

	/// Deletes the region refered to by index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(int index);
	bool deleteRegionSqueeze(int index);
	void setDefaultControl(AMControl* defaultControl){defaultControl_ = defaultControl; regions_->setDefaultControl(defaultControl);}

signals:
	void regionsChanged();

private slots:
		void onDataChanged(QModelIndex a,QModelIndex b){ Q_UNUSED(a); Q_UNUSED(b); emit regionsChanged();}

protected:
	/// Holds the list of AMXASRegion pointers.
	AMControl *defaultControl_;
	AMRegionsListModel *regions_;

	virtual bool setupModel();
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMRegion* region(int index) const;
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
	virtual double maxEnergy(){
		double curMax = -1e12;
		for(int x = 0; x < count(); x++){
			if(start(x) > curMax)
				curMax = start(x);
			if(end(x) > curMax)
				curMax = end(x);
		}
		return curMax;
	}

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
	virtual bool setupModel();
};

#endif // AMREGIONSLIST_H
