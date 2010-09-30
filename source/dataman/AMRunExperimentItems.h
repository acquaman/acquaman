#ifndef AMRUNEXPERIMENTITEMS_H
#define AMRUNEXPERIMENTITEMS_H

#include "acquaman.h"
#include "ui/AMDragDropItemModel.h"
#include <QDateTime>

class AMDatabase;


/// This subclass of QStandardItem provides the extra features needed to store and display the run name beside the run date, edit the run name, and store the edited name back to the database.
class AMRunModelItem : public AMDragDropItem {

public:
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

	/// Helper function to add an object (Scan Object, etc.) from the Objects table to this experiment. Returns true on success. Returns false if it's already in there, or if any other error occurs.
	bool addObjectToExperiment(int objectId);

	/// Convenience function to get the database id of this experiment
	int id() const {
		return data(AM::IdRole).toInt();
	}


protected:
	AMDatabase* db_;
};

#endif // AMRUNEXPERIMENTITEMS_H
