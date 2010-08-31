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
#include "beamline/AMBeamlineActionsList.h"
#include "ui/AMVerticalStackWidget.h"

//class AMBeamlineActionListModel;
//class AMBeamlineActionsList;
class AMBeamlineActionsListView;
class AMBeamlineActionAdder;

class AMWorkflowManagerView : public QWidget
{
Q_OBJECT
public:
	explicit AMWorkflowManagerView(QWidget *parent = 0);

signals:
	void freeToScan(bool queueEmpty, bool queueNotRunning);
	void lockdownScanning(bool isLocked, QString reason);
	void addedScan(AMScanConfiguration *cfg);

public slots:
	void onStartQueueRequested();
	void onAddActionRequested();
	void onAddScanRequested(AMScanConfiguration *cfg, bool startNow = false);
	void onCancelAddScanRequest();
	void onInsertActionRequested(AMBeamlineActionItem *action, int index);
	void onBeamlineScanningChanged(bool scanning);

protected slots:
	void onQueueIsRunningChanged(bool isRunning);
	void onQueueIsEmptyChanged(bool isEmpty);

	void onNewScanConfigurationView();
	void onQueueAndScanningStatusChanged();

protected:
	QLabel *placeHolder_;
	QPushButton *startWorkflowButton_;
	QPushButton *addActionButton_;
	QVBoxLayout *vl_;

	AMBeamlineActionsList *workflowActions_;
	AMBeamlineActionsQueue *workflowQueue_;
	AMBeamlineActionsListView *workflowView_;
	bool cancelAddRequest_;

	AMBeamlineActionAdder *adder_;
};

class AMBeamlineActionsListView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, AMBeamlineActionsQueue *actionsQueue, QWidget *parent = 0);

	/*
	AMBeamlineActionItem* firstInQueue();
	int indexOfFirst();
	int visibleIndexOfFirst();
	*/

signals:

	void queueUpdated(int count);


protected slots:
	void onActionChanged(int index);
	void onActionAdded(int index);
	void onActionRemoved(int index);

	void onActionRemoveRequested(AMBeamlineActionItem *item);
	/*
	void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
	void handleRowsInsert(const QModelIndex &parent, int start, int end);
	void handleRowsRemoved(const QModelIndex &parent, int start, int end);
	void redrawBeamlineActionsList();
	*/

	/*
	void onFocusRequested(AMBeamlineActionItem *action);
	void onRemoveRequested(AMBeamlineActionItem *action);
	void onHideRequested(AMBeamlineActionItem *action);
	void onExpandRequested(AMBeamlineActionItem *action);
	void onActionStarted(AMBeamlineActionItem *action);
	void onActionSucceeded(AMBeamlineActionItem *action);

	*/
	void reindexViews();

protected:
	AMBeamlineActionsList *actionsList_;
	AMBeamlineActionsQueue *actionsQueue_;
	AMVerticalStackWidget *actionsViewList_;
	int focusAction_;

	/*
	QQueue<AMBeamlineActionItem*> actionsQueue_;
	QList<AMBeamlineActionView*> fullViewList_;
	QList<AMBeamlineActionView*> visibleViewList_;
	QQueue<AMBeamlineActionView*> viewQueue_;
	int focusAction_;
	QGroupBox *gb_;
	QScrollArea *sa_;
	QVBoxLayout *iib;

	int insertRowIndex_;
	*/
};

class AMBeamlineActionAdder : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionAdder(QWidget *parent = 0);

public slots:
	virtual void onQueueUpdated(int count);

signals:
	void insertActionRequested(AMBeamlineActionItem *action, int index);

protected slots:
	virtual void onActionTypeBoxUpdate(int curIndex);
	virtual void onActionSubTypeBoxUpdate(int curIndex);

	virtual void onNewMoveSetpoint(double value);
	virtual void onAddControlSetMoveAction();
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
	QLabel *xPosLabel_, *yPosLabel_, *zPosLabel_;
};

#endif // AMWORKFLOWMANAGERVIEW_H
