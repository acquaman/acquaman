#ifndef AMWORKFLOWMANAGERVIEW_H
#define AMWORKFLOWMANAGERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDebug>
#include <QQueue>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QStringListModel>
#include <QAbstractListModel>
#include "beamline/AMBeamlineScanAction.h"
#include "beamline/AMBeamlineControlMoveAction.h"

class AMBeamlineActionListModel;
class AMBeamlineActionsList;
class AMBeamlineActionsListView;
class AMBeamlineActionAdder;

class AMWorkflowManagerView : public QWidget
{
Q_OBJECT
public:
	explicit AMWorkflowManagerView(QWidget *parent = 0);

	bool isEmpty(){ return queueEmpty_;}

signals:
	void freeToScan(bool ready);
	void addedToQueue(AMScanConfiguration *cfg);

public slots:
	void onStartScanRequested();
	void onStartQueueRequested();
	void onAddActionRequested();
	void onAddToQueueRequested(AMScanConfiguration *cfg);
	void onInsertActionRequested(AMBeamlineActionItem *action, int index);

protected:
	QLabel *placeHolder_;
	QPushButton *startWorkflowButton_;
	QPushButton *addActionButton_;
	QVBoxLayout *vl_;

	AMBeamlineActionsList *workflowActions_;
	AMBeamlineActionsListView *workflowView_;
	bool queueEmpty_;

	AMBeamlineActionAdder *adder_;
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
	int indexOf(AMBeamlineActionItem *iAction);

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

	AMBeamlineActionItem* firstInQueue();
	int indexOfFirst();
	int visibleIndexOfFirst();

signals:
	void queueUpdated(QQueue<AMBeamlineActionItem*> actionQueue);

protected slots:
	void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
	void handleRowsInsert(const QModelIndex &parent, int start, int end);
	void handleRowsRemoved(const QModelIndex &parent, int start, int end);
	void redrawBeamlineActionsList();

	void onFocusRequested(AMBeamlineActionItem *action);
	void onRemoveRequested(AMBeamlineActionItem *action);
	void onHideRequested(AMBeamlineActionItem *action);
	void onExpandRequested(AMBeamlineActionItem *action);
	void onActionStarted(AMBeamlineActionItem *action);
	void onActionSucceeded(AMBeamlineActionItem *action);

	void reindexViews();

protected:
	AMBeamlineActionsList *actionsList_;
	QQueue<AMBeamlineActionItem*> actionsQueue_;
	QList<AMBeamlineActionView*> fullViewList_;
	QList<AMBeamlineActionView*> visibleViewList_;
	QQueue<AMBeamlineActionView*> viewQueue_;
	int focusAction_;
	QGroupBox *gb_;
	QScrollArea *sa_;
	QVBoxLayout *iib;

	int insertRowIndex_;
};

class AMBeamlineActionAdder : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionAdder(QWidget *parent = 0);

public slots:
	virtual void onQueueUpdated(QQueue<AMBeamlineActionItem*> actionsQueue);

signals:
	void insertActionRequested(AMBeamlineActionItem *action, int index);

protected slots:
	virtual void onActionTypeBoxUpdate(int curIndex);
	virtual void onActionSubTypeBoxUpdate(int curIndex);

	virtual void onNewMoveSetpoint(double value);
	virtual void onAddMoveAction();

protected:
	QList<QStringList> subTypesLists_;
	AMPVwStatusControl *movePV_;
	double moveSetpoint_;

	QVBoxLayout *vl_;
	QComboBox *addWhereBox_;
	QComboBox *actionTypeBox_;
	QComboBox *actionSubTypeBox_;
	QWidget *nextStepWidget_;
	QDoubleSpinBox *moveSetpointDSB_;
};

#endif // AMWORKFLOWMANAGERVIEW_H
