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


#ifndef AMRUNEXPERIMENTITEMS_H
#define AMRUNEXPERIMENTITEMS_H

#include "acquaman.h"
#include "ui/AMDragDropItemModel.h"
#include <QDateTime>

class AMDatabase;


/// This subclass of QStandardItem provides the extra features needed to store and display the run name beside the run date, edit the run name, and store the edited name back to the database.
class AMRunModelItem : public AMDragDropItem {

public:
 	virtual ~AMRunModelItem();
	explicit AMRunModelItem(AMDatabase* db, int id, const QString& editText = QString()) : AMDragDropItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
		setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		setDropEnabled(false);
	}

	/// Re-implemented from QStandardItem: Returns a special DisplayRole distinct from the EditRole. (The QStandardItem treat Qt::EditRole and Qt::DisplayRole as referring to the same data.)
	virtual QVariant data(int role) const;

	/// Re-implemented to order runs by date:
	virtual bool operator< ( const QStandardItem & other ) const {
		return data(AM::DateTimeRole).toDateTime() < other.data(AM::DateTimeRole).toDateTime();
	}


	/// Re-implemented to save the edited run name back to the database:
	virtual void setData(const QVariant &value, int role);

protected:
	AMDatabase* db_;
};

/// This subclass of QStandardItem provides the extra features needed to store edited experiment names back to the database.
class AMExperimentModelItem : public AMDragDropItem {

public:
 	virtual ~AMExperimentModelItem();
	explicit AMExperimentModelItem(AMDatabase* db, int id, const QString& editText = QString()) : AMDragDropItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
		setData(true, AM::CanCloseRole);	// experiments can be deleted...
		setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable);
		setDropEnabled(true);
	}

	/// Re-implemented to save the edited experiment name back to the database:
	virtual void setData(const QVariant &value, int role);

	/// Additional function to accept drops from the drag-and-drop system. We use this to accept dropped scans and add them to this experiment
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action);

	/// Convenience function to get the database id of this experiment
	int id() const {
		return data(AM::IdRole).toInt();
	}

	/// Retrieve the database that contains this experiment
	AMDatabase* database() const { return db_; }

protected:
	AMDatabase* db_;
};

#endif // AMRUNEXPERIMENTITEMS_H
