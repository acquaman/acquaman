/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMBEAMLINEPARALLELACTIONSLIST_H
#define AMBEAMLINEPARALLELACTIONSLIST_H

#include <QObject>
#include <QAbstractItemModel>

#include "beamline/AMBeamlineActionItem.h"
#include "util/AMBiHash.h"


class AMBeamlineParallelActionsListHolder;
class AMBeamlineParallelActionListModel;

class AMBeamlineParallelActionsList : public QObject
{
	Q_OBJECT

	public:
		AMBeamlineParallelActionsList(QObject *parent = 0);

		AMBeamlineParallelActionListModel* model();
		int stageCount();
		int count();
		int countAt(int stageIndex);
		/* NTBA - August 25th, 2011 (David Chevrier)
		   Need to check on how constant this function really is.
		   Should be address with the const * const question.
		 */
		QList<AMBeamlineActionItem*>* stage(int stageIndex) const;
		AMBeamlineActionItem* action(int stageIndex, int index) const;
		int stageIndexOf(QList<AMBeamlineActionItem*> *iList);
		int indexOf(QList<AMBeamlineActionItem*> *iList);
		int stageIndexOf(AMBeamlineActionItem *iAction);
		int indexOf(AMBeamlineActionItem *iAction);
		QPair<int,int> indicesOf(AMBeamlineActionItem *iAction);

		bool isRunning();

		void puke();

	public slots:
		bool setStage(int stageIndex, QList<AMBeamlineActionItem*> *stageList);
		bool setAction(int stageIndex, int index, AMBeamlineActionItem *action);
		virtual bool addStage(int stageIndex, QList<AMBeamlineActionItem*> *stageList);
		virtual bool addAction(int stageIndex, int index, AMBeamlineActionItem *action);
		virtual bool appendStage(QList<AMBeamlineActionItem*> *stageList);
		virtual bool appendAction(int stageIndex, AMBeamlineActionItem *action);
		bool deleteStage(int stageIndex);
		bool deleteAction(int stageIndex, int index);
		void start();
		void cancel();

	signals:
		void listSucceeded();
		void listFailed(int firstExplanation);
		void stageChanged(int stageIndex);
		void actionChanged(int stageIndex, int index);
		void stageAdded(int stageIndex);
		void actionAdded(int stageIndex, int index);
		void stageRemoved(int stageIndex);
		void actionRemoved(int stageIndex, int index);
		void stageStarted(int stageIndex);
		void actionStarted(int stageIndex, int index);
		void stageSucceeded(int stageIndex);
		void actionSucceeded(int stageIndex, int index);
		void stageReady(int stageIndex);
		void actionReady(int stageIndex, int index, bool ready);
		void stageFailed(int stageIndex, QList<int> index, int explanation);
		void actionFailed(int stageIndex, int index, int explanation);
		void stageProgress(double elasped, double total);
		void stageProgress(int stageIndex, double elapsed, double total);

	private slots:
		void onDataChanged(QModelIndex a,QModelIndex b);
		void onRowsInserted(QModelIndex parent, int start, int end);
		void onRowsRemoved(QModelIndex parent, int start, int end);
		void onActionStarted();
		void onActionSucceeded();
		void onActionReady(bool ready);
		void onActionFailed(int explanation);
		void onActionProgress(double, double);
		void onStageStarted(int stageIndex);
		void onStageSucceeded();
		void onStageFailed(QList<AMBeamlineActionItem*> failureList, int explanation);

	protected:
		AMBeamlineParallelActionListModel *actions_;
		AMBiHash<QList<AMBeamlineActionItem*>*, AMBeamlineParallelActionsListHolder*> holdersHash_;
		QList<double> lastIndexProgress_;
		int currentStage_;
		bool isRunning_;

		virtual bool setupModel();

	private:
		int insertRowLatch_;
};



class AMBeamlineParallelActionsListHolder : public QObject
{
Q_OBJECT
public:
	AMBeamlineParallelActionsListHolder(QObject *parent = 0);

public slots:
	void addAction(AMBeamlineActionItem *ai);
	void removeAction(AMBeamlineActionItem *ai);
	void actionSucceeded();
	void actionFailed(int explanation);

signals:
	void everythingFinished();
	void somethingFailed(QList<AMBeamlineActionItem*> failureList, int explanation);

protected:
	QList<AMBeamlineActionItem*> waitingOn_;
	QList<AMBeamlineActionItem*> failures_;

	bool hasFailed_;
	int firstFailureExplanation_;
};

class AMBeamlineParallelActionListModel : public QAbstractItemModel
{
Q_OBJECT
public:
	AMBeamlineParallelActionListModel(QObject *parent = 0);

	int rowCount(const QModelIndex & /*parent*/) const;
	int columnCount(const QModelIndex & /*parent*/) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex index(int row, const QModelIndex &parent);
	QModelIndex parent(const QModelIndex &child) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	/* NTBA March 14, 2011 David Chevrier
	Qt::ItemFlags flags(const QModelIndex &index) const;
	*/

protected:
	QList< QList<AMBeamlineActionItem*>* > *actions_;
	AMBiHash<int, int> listHash_;
	AMBiHash<int, QPair<int, int> > actionsHash_;
	int nextIndex_;
};

class AMBeamlineNumberAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	AMBeamlineNumberAction(int number, QObject *parent) :
			AMBeamlineActionItem(parent)
	{ number_ = number;}

	int getNumber() const { return number_;}

public slots:
	void start() {;}
	void cleanup() {;}

protected:
	int number_;
};


#endif // AMBEAMLINEPARALLELACTIONSLIST_H
