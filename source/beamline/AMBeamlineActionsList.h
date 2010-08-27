#ifndef AMBEAMLINEACTIONSLIST_H
#define AMBEAMLINEACTIONSLIST_H

#include <QObject>
#include <QAbstractListModel>
#include <QQueue>

#include "beamline/AMBeamlineScanAction.h"
#include "beamline/AMBeamlineControlMoveAction.h"

class AMBeamlineActionListModel;

class AMBeamlineActionsList : public QObject
{
Q_OBJECT

public:
	AMBeamlineActionsList(QObject *parent = 0);

	AMBeamlineActionListModel* model();//{ return actions_;}
	int count();//{return actions_->rowCount(QModelIndex());}
	AMBeamlineActionItem* action(size_t index) const;
	int indexOf(AMBeamlineActionItem *iAction);

public slots:
	bool setAction(size_t index, AMBeamlineActionItem *action);
	virtual bool addAction(size_t index, AMBeamlineActionItem *action);
	virtual bool appendAction(AMBeamlineActionItem *action);
	bool deleteAction(size_t index);

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
	void onActionStarted();
	void onActionSucceeded();
	void onActionReady(bool ready);
	void onActionFailed(int explanation);

protected:
	AMBeamlineActionListModel *actions_;

	virtual bool setupModel();

private:
	int insertRowLatch_;
};

class AMBeamlineActionListModel : public QAbstractListModel
{
Q_OBJECT
public:
	AMBeamlineActionListModel(QObject *parent = 0);

	int rowCount(const QModelIndex & /*parent*/) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
	QList<AMBeamlineActionItem*> *actions_;
};

class AMBeamlineActionsQueue : public QObject
{
Q_OBJECT
public:
	AMBeamlineActionsQueue(AMBeamlineActionsList *fullList, QObject *parent = 0);

	AMBeamlineActionItem* head();
	int count();
	int indexOfHead();
	bool isEmpty();

public slots:
	void setBeamlineActionsList(AMBeamlineActionsList *fullList);

signals:
	void headChanged();

protected:
	void initializeQueue();

protected slots:
	void onActionAdded(int index);
	void onActionSucceeded(int index);

protected:
	AMBeamlineActionsList *fullList_;
	int headIndex_;
};

#endif // AMBEAMLINEACTIONSLIST_H
