#ifndef AMBEAMLINEPARALLELACTIONSLIST_H
#define AMBEAMLINEPARALLELACTIONSLIST_H

#include <QObject>
#include <QAbstractItemModel>

#include "beamline/AMBeamlineActionItem.h"
#include "AMBiHash.h"


class AMBeamlineParallelActionsListHolder;
class AMBeamlineParallelActionListModel;
/*
class AMBeamlineParallelActionsList : public QObject
{
	Q_OBJECT

	public:
		AMBeamlineParallelActionsList(QObject *parent = 0);

		AMBeamlineParallelActionListModel* model();
		int count();
		AMBeamlineActionItem* action(int index) const;
		int indexOf(AMBeamlineActionItem *iAction);

	public slots:
		bool setAction(int index, AMBeamlineActionItem *action);
		virtual bool addAction(int index, AMBeamlineActionItem *action);
		virtual bool appendAction(AMBeamlineActionItem *action);
		bool deleteAction(int index);

	signals:
		void actionChanged(int index);
		void actionAdded(int index);
		void actionRemoved(int index);
		void actionStarted(int index);
		void actionSucceeded(int index);
		void actionReady(int index, bool ready);
		void actionFailed(int index, int explanation);

	private slots:
		void onDataChanged(QModelIndex a,QModelIndex b);//{ Q_UNUSED(a); Q_UNUSED(b); emit actionsChanged();}
		void onRowsInserted(QModelIndex parent, int start, int end);
		void onRowsRemoved(QModelIndex parent, int start, int end);
		void onColumnsInserted(QModelIndex parent, int start, int end);
		void onColumnsRemoved(QModelIndex parent, int start, int end);
		void onActionStarted();
		void onActionSucceeded();
		void onActionReady(bool ready);
		void onActionFailed(int explanation);

	protected:
		AMBeamlineParallelActionListModel *actions_;

		virtual bool setupModel();

	private:
		int insertRowLatch_;
};
*/


class AMBeamlineParallelActionsListHolder : public QObject
{
Q_OBJECT
public:
	AMBeamlineParallelActionsListHolder(QObject *parent = 0);

public slots:
	void addAction(AMBeamlineActionItem* ai);
	void actionFinished();

signals:
	void everythingFinished();

protected:
	QList<AMBeamlineActionItem*> waitingOn_;
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
	/*
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
