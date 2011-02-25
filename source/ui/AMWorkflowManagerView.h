/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "dataman/AMSamplePlate.h"

#include "acquaman/AMScanConfiguration.h"
#include "beamline/AMBeamlineScanAction.h"
#include "beamline/AMBeamlineControlSetMoveAction.h"

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

	// disabled for now: AMBeamlineActionAdder *adder_;
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


#endif // AMWORKFLOWMANAGERVIEW_H
