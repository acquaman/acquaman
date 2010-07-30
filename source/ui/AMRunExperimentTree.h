#ifndef AMRUNEXPERIMENTTREE_H
#define AMRUNEXPERIMENTTREE_H

#include <QStandardItemModel>
#include <QDateTime>
#include "acquaman.h"

class AMDatabase;
class AMSidebar;

/// This subclass of QStandardItem provides the extra features needed to store and display the run name beside the run date, edit the run name, and store the edited name back to the database.
class AMRunModelItem : public QStandardItem {

public:
	explicit AMRunModelItem(AMDatabase* db, int id, const QString& editText = QString()) : QStandardItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
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
class AMExperimentModelItem : public QStandardItem {

public:
	explicit AMExperimentModelItem(AMDatabase* db, int id, const QString& editText = QString()) : QStandardItem(editText) {
		db_ = db;
		setData(id, AM::IdRole);
	}

	/// Re-implemented to save the edited experiment name back to the database:
	virtual void setData(const QVariant &value, int role);

protected:
	AMDatabase* db_;
};

/// This class provides an "insert" or "hook" into a tree view and item model. Given two existing items in the model, it will use these as a "run heading" and "experiment heading", and insert the runs and experiments found in the database underneath them.  Additionally, if you connect the view's AMAbstractItemView::clicked(QModelIndex) signal to this class's onItemClicked(QModelIndex) slot, it will be able to issue runSelected(id) and experimentSelected(id) signals.
class AMRunExperimentInsert : public QObject
{
Q_OBJECT
public:
	explicit AMRunExperimentInsert(AMDatabase* db, QStandardItem* runHeading, QStandardItem* experimentHeading, QObject *parent = 0);

signals:
	/// Emitted when a run is selected. When the Runs heading is selected, this will be emitted with \c id = -1.
	void runSelected(int id);
	/// Emitted when an experiment is selected.  When the Experiments heading is selected, this will be emitted with \c id = -1.
	void experimentSelected(int id);

public slots:
	/// Connect this slot to the view's clicked(const QModelIndex& index) signal. It will emit runSelected and experimentSelected as required.
	void onItemClicked(const QModelIndex& index);


protected slots:
	/// This slot receives updated() signals from the database, and (if a refresh hasn't been scheduled yet), schedules a refreshRuns() or refreshExperiments() for once control returns to the Qt event loop.  This provides a performance boost by potentially only doing one refresh for multiple sequential database updates.
	void onDatabaseUpdated(const QString& table, int id);

	/// request data from the database and fill the model
	void refreshRuns();
	/// request data from the database and fill the model
	void refreshExperiments();


protected:
	void refresh();

	/// Top-level items (run header and experiment header)
	QStandardItem *runItem_, *experimentItem_;

	AMDatabase* db_;
	/// Indicates whether a refresh_ has already been scheduled. If it has, there's no point in scheduling another one.  These flags are cleared after the refreshes are completed.
	bool runRefreshScheduled_, expRefreshScheduled_;
};


/* QString treeview_ss = "QTreeView { color: white; background: black; }" +
		QString("QTreeView { show-decoration-selected: 0; }") +
		QString("QTreeView::item:selected { border: 1px solid grey; }") +
		QString("QTreeView::item:selected:!active { color: white; border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:selected:active { border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:hover { border: 1px solid grey; }") + */

#endif // AMRUNEXPERIMENTTREE_H
