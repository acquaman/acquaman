#ifndef AMRUNEXPERIMENTITEMS_H
#define AMRUNEXPERIMENTITEMS_H

#include "acquaman.h"
#include <QStandardItem>
#include <QDateTime>

class AMDatabase;


/// This subclass of QStandardItem provides the extra features needed to store and display the run name beside the run date, edit the run name, and store the edited name back to the database.
class AMRunModelItem : public QStandardItem {

public:
	explicit AMRunModelItem(AMDatabase* db, int id, const QString& editText = QString()) : QStandardItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
		setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	}

	/// Re-implemented from QStandardItem: Returns a special DisplayRole distinct from the EditRole. (The QStandardItem treat Qt::EditRole and Qt::DisplayRole as referring to the same data.)
	virtual QVariant data(int role) const;

	/// Re-implemented to order runs by date:
	virtual bool operator< ( const QStandardItem & other ) const {
		return data(AM::DateTimeRole).toDateTime() < other.data(AM::DateTimeRole).toDateTime();
	}

	/// Re-implemented as a custom type, distinguished from QStandardItem
	virtual int type() const {
		return AM::RunItem;
	}

	/// Re-implemented to save the edited run name back to the database:
	virtual void setData(const QVariant &value, int role);

protected:
	AMDatabase* db_;
};

/// This subclass of QStandardItem provides the extra features needed to store edited experiment names back to the database.
class AMExperimentModelItem : public QStandardItem {

public:
	explicit AMExperimentModelItem(AMDatabase* db, int id, const QString& editText = QString()) : QStandardItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
		setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable);
		setDropEnabled(true);
	}

	/// Re-implemented to save the edited experiment name back to the database:
	virtual void setData(const QVariant &value, int role);

	/// Re-implemented as a custom type, distinguished from QStandardItem
	virtual int type() const {
		return AM::ExperimentItem;
	}

	/// Additional function to accept drops from the drag-and-drop system. We use this to accept dropped scans and add them to this experiment
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action);

protected:
	AMDatabase* db_;
};

#endif // AMRUNEXPERIMENTITEMS_H
