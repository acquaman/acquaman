#ifndef AMWORKFLOWMANAGERVIEW_H
#define AMWORKFLOWMANAGERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QAbstractListModel>
#include "beamline/AMBeamlineScanAction.h"

class AMBeamlineActionListModel;
class AMBeamlineActionsList;
class AMBeamlineActionsListView;
class BeamlineActionGraphicItem;

class AMWorkflowManagerView : public QWidget
{
Q_OBJECT
public:
	explicit AMWorkflowManagerView(QWidget *parent = 0);

	bool isEmpty(){ return queueEmpty_;}

signals:
	void freeToScan(bool ready);

public slots:
	void onStartScanRequested();
	void onAddToQueueRequested(AMScanConfiguration *cfg);

protected:
	QLabel *placeHolder_;
	QVBoxLayout *vl_;

	AMBeamlineActionsList* workflowActions_;
	QStringList data_;
	QStringListModel *model_;
	QListView *listView_;
	bool queueEmpty_;
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

class AMBeamlineActionsList : public QObject
{
Q_OBJECT

public:
	AMBeamlineActionsList(QObject *parent = 0);

	AMBeamlineActionListModel* model(){ return actions_;}
	int count(){return actions_->rowCount(QModelIndex());}
	AMBeamlineActionItem* action(size_t index) const;

public slots:
	bool setAction(size_t index, AMBeamlineActionItem *action);
	virtual bool addAction(size_t index, AMBeamlineActionItem *action);
	virtual bool appendAction(AMBeamlineActionItem *action);
	bool deleteAction(size_t index);

signals:
	void actionsChanged();

private slots:
		void onDataChanged(QModelIndex a,QModelIndex b){ Q_UNUSED(a); Q_UNUSED(b); emit actionsChanged();}

protected:
	AMBeamlineActionListModel *actions_;

	virtual bool setupModel();
};

class AMBeamlineActionsListView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, QWidget *parent = 0);

protected slots:
	void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
	void handleRowsInsert(const QModelIndex &parent, int start, int end);
	void handleRowsRemoved(const QModelIndex &parent, int start, int end);
	void redrawBeamlineActionsList();

protected:
	AMBeamlineActionsList *actionsList_;
	QGraphicsScene *scene;
	QGraphicsView *view;
};

class BeamlineActionGraphicItem : public QGraphicsItem
{
public:
	BeamlineActionGraphicItem(int width);

	QRectF boundingRect() const { return QRectF(0, -15, width_, 30);}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	QColor color;
	int width_;
};

#endif // AMWORKFLOWMANAGERVIEW_H
