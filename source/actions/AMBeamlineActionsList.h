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


#ifndef AMBEAMLINEACTIONSLIST_H
#define AMBEAMLINEACTIONSLIST_H

//#include <QObject>
//#include <QAbstractListModel>
//#include <QQueue>

//#include "actions/AMBeamlineActionItem.h"

//class AMBeamlineActionListModel;

//class AMBeamlineActionsList : public QObject
//{
//Q_OBJECT

//public:
//	AMBeamlineActionsList(QObject *parent = 0);

//	AMBeamlineActionListModel* model();
//	int count();
//	AMBeamlineActionItem* action(int index) const;
//	int indexOf(AMBeamlineActionItem *iAction);

//public slots:
//	bool setAction(int index, AMBeamlineActionItem *action);
//	virtual bool addAction(int index, AMBeamlineActionItem *action);
//	virtual bool appendAction(AMBeamlineActionItem *action);
//	bool deleteAction(int index);
//	bool swapActions(int indexOfFirst);

//signals:
//	void actionChanged(int index);
//	void actionAdded(int index);
//	void actionRemoved(int index);
//	void actionStarted(int index);
//	void actionSucceeded(int index);
//	void actionReady(int index, bool ready);
//	void actionFailed(int index, int explanation);

//private slots:
//	void onDataChanged(QModelIndex a,QModelIndex b);
//	void onRowsInserted(QModelIndex parent, int start, int end);
//	void onRowsRemoved(QModelIndex parent, int start, int end);
//	void onActionStarted();
//	void onActionSucceeded();
//	void onActionReady(bool ready);
//	void onActionFailed(int explanation);

//protected:
//	AMBeamlineActionListModel *actions_;

//	virtual bool setupModel();

//private:
//	int insertRowLatch_;
//};

//class AMBeamlineActionListModel : public QAbstractListModel
//{
//Q_OBJECT
//public:
//	AMBeamlineActionListModel(QObject *parent = 0);

//	int rowCount(const QModelIndex & /*parent*/) const;
//	QVariant data(const QModelIndex &index, int role) const;
//	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
//	bool setData(const QModelIndex &index, const QVariant &value, int role);
//	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
//	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
//	bool swapData(int positionOfFirst);
//	Qt::ItemFlags flags(const QModelIndex &index) const;

//protected:
//	QList<AMBeamlineActionItem*> *actions_;
//};

//class AMBeamlineActionsQueue : public QObject
//{
//Q_OBJECT
//public:
//	AMBeamlineActionsQueue(AMBeamlineActionsList *fullList, QObject *parent = 0);

//	AMBeamlineActionItem* head();
//	AMBeamlineActionItem* peek();
//	int count();
//	int indexOfHead();
//	bool isEmpty();
//	bool peekIsEmpty();
//	bool isRunning();

//public slots:
//	void setBeamlineActionsList(AMBeamlineActionsList *fullList);
//	void startQueue();

//signals:
//	void headChanged();
//	void isEmptyChanged(bool isEmpty);
//	void isRunningChanged(bool isRunning);

//protected:
//	void initializeQueue();

//protected slots:
//	void onActionAdded(int index);
//	void onActionRemoved(int index);
//	void onActionStarted(int index);
//	void onActionSucceeded(int index);
//	void onActionReady(int index);
//	void onActionFailed(int index, int explanation);

//protected:
//	AMBeamlineActionsList *fullList_;
//	int headIndex_;
//	bool queueRunning_;
//};

#endif // AMBEAMLINEACTIONSLIST_H
