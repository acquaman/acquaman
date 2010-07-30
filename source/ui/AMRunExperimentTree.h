#ifndef AMRUNEXPERIMENTTREE_H
#define AMRUNEXPERIMENTTREE_H

#include <QStandardItemModel>
#include <QDateTime>

class AMDatabase;

/// This class is a near-identical replacement for QStandardItem, except that it stores and returns a DisplayRole separate from the EditRole. The DisplayRole is a string concatenation of the EditRole string and the UserRole+1 string (which is used in AMRunExperimentModel to store the date of the run).
class AMRunModelItem : public QStandardItem {

public:
	explicit AMRunModelItem(const QString& editText = QString()) : QStandardItem(editText) {}

	/// Re-implemented from QStandardItem: Returns a special DisplayRole distinct from the EditRole. (The QStandardItem treat Qt::EditRole and Qt::DisplayRole as referring to the same data.)
	virtual QVariant data(int role) const;
};

class AMRunExperimentModel : public QStandardItemModel
{
Q_OBJECT
public:
	explicit AMRunExperimentModel(AMDatabase* db, QObject *parent = 0);

signals:

public slots:

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

#include <QTreeView>

class AMRunExperimentTree : public QTreeView {
	Q_OBJECT
public:
	explicit AMRunExperimentTree(AMDatabase* db, QWidget* parent = 0);

signals:

public slots:



protected:
	AMRunExperimentModel* model_;

};

/* QString treeview_ss = "QTreeView { color: white; background: black; }" +
		QString("QTreeView { show-decoration-selected: 0; }") +
		QString("QTreeView::item:selected { border: 1px solid grey; }") +
		QString("QTreeView::item:selected:!active { color: white; border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:selected:active { border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:hover { border: 1px solid grey; }") + */

#endif // AMRUNEXPERIMENTTREE_H
