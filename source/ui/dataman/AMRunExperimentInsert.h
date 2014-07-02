/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMRUNEXPERIMENTTREE_H
#define AMRUNEXPERIMENTTREE_H

#include <QStandardItemModel>
#include <QDateTime>
#include "acquaman.h"

class AMDatabase;
class QAbstractItemView;

/// This class provides an "insert" or "hook" into the AMWindowPaneModel. Given two existing items in the model, it will use these as a "run heading" and "experiment heading", and insert the runs and experiments found in the database underneath them, as 'alias' link items.  (See AMWindowPaneModel for more information about alias items.)
/*! The newExperimentAdded(QModelIndex) signal is emitted when we detect that a new experiment has been created.  It useful as a request to the view containing these items, to start editing the name of the new experiment. */
class AMRunExperimentInsert : public QObject
{
Q_OBJECT
public:
	explicit AMRunExperimentInsert(AMDatabase* db, QStandardItem* runHeading, QStandardItem* experimentHeading, QObject *parent = 0);

	virtual ~AMRunExperimentInsert() {
	}

signals:

	/// Emitted when a new item has been created
	void newExperimentAdded(const QModelIndex& index);

public slots:


protected slots:
	/// This slot receives updated() and removed() signals from the database, and (if a refresh hasn't been scheduled yet), schedules a refreshRuns() or refreshExperiments() for once control returns to the Qt event loop.  This provides a performance boost by potentially only doing one refresh for multiple sequential database updates.
	void onDatabaseUpdated(const QString& table, int id);
	/// This slot receives created() signals from the database.  It works the same as onDatabaseUpdated(), but for newly-created experiments, it selects them and starts editing them so that the user can enter their own name.
	void onDatabaseObjectCreated(const QString& table, int id);

	/// request data from the database and fill the model
	void refreshRuns();
	/// request data from the database and fill the model
	void refreshExperiments();



protected:
	void refresh();

	/// Top-level items (run header and experiment header)
	QStandardItem *runItem_, *experimentItem_;


	/// The database to search for runs and experiments
	AMDatabase* db_;
	/// Indicates whether a refresh_ has already been scheduled. If it has, there's no point in scheduling another one.  These flags are cleared after the refreshes are completed.
	bool runRefreshScheduled_, expRefreshScheduled_;

	/// Used as a flag to indicate that a new experiment was just added... You should select and start editing this one so that the user can provide their desired name. Is equal to -1 in all other situations.
	int newExperimentId_;
};


/* QString treeview_ss = "QTreeView { color: white; background: black; }" +
		QString("QTreeView { show-decoration-selected: 0; }") +
		QString("QTreeView::item:selected { border: 1px solid grey; }") +
		QString("QTreeView::item:selected:!active { color: white; border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:selected:active { border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:hover { border: 1px solid grey; }") + */

#endif // AMRUNEXPERIMENTTREE_H
