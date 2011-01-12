#include "AMActionRunnerQueueView.h"

#include "actions2/AMActionRunner.h"

#include <QTreeView>
#include <QBoxLayout>
#include <QFrame>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>

AMActionRunnerQueueView::AMActionRunnerQueueView(AMActionRunner* actionRunner, QWidget *parent) :
    QWidget(parent)
{
	actionRunner_ = actionRunner;

	treeView_ = new QTreeView();
	treeView_->setModel(actionRunner_->queueModel());
	treeView_->setHeaderHidden(true);
	treeView_->setAttribute(Qt::WA_MacShowFocusRect, false);
	treeView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QFrame* topFrame = new QFrame();
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setObjectName("topFrame");
	topFrame->setStyleSheet("QFrame#topFrame {\n/*background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));*/\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 0, 0, 105), stop:0.494444 rgba(0, 0, 0, 105), stop:0.5 rgba(0, 0, 0, 134), stop:1 rgba(0, 0, 0, 134));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout(topFrame);
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 4);

	hideButton_ = new QToolButton();
	hideButton_->setStyleSheet("QToolButton {\nborder: none;\nbackground-color: rgba(255, 255, 255, 0);\ncolor: white;\n image: url(:/22x22/arrow-white-right.png)} \nQToolButton::checked {\n	image: url(:/22x22/arrow-white-down.png);\n}\n");
	hideButton_->setCheckable(true);
	hideButton_->setChecked(true);
	hl->addWidget(hideButton_);
	hl->addSpacing(10);

	QVBoxLayout* vl2 = new QVBoxLayout();
	headerTitle_ = new QLabel("Upcoming Actions");
	headerTitle_->setStyleSheet("color: white;\nfont: 20pt \"Lucida Grande\"");
	headerSubTitle_ = new QLabel(QString("%1 actions in the workflow queue.").arg(actionRunner_->queuedActionCount()));
	headerSubTitle_->setStyleSheet("color: rgb(204, 204, 204);\nfont: 12pt \"Lucida Grande\"");
	vl2->addWidget(headerTitle_);
	vl2->addWidget(headerSubTitle_);
	hl->addLayout(vl2);
	hl->addStretch(1);

	duplicateButton_ = new QPushButton("Duplicate");
	duplicateButton_->setDisabled(true);
	hl->addWidget(duplicateButton_);
	deleteButton_ = new QPushButton("Delete");
	deleteButton_->setDisabled(true);
	hl->addWidget(deleteButton_);
	hl->addSpacing(20);

	pauseButton_ = new QPushButton(actionRunner_->queuePaused() ? "Queue Paused\n(Click to run)" : "Queue Running\n(Click to pause)");
	QIcon pauseIcon(":/22x22/media-playback-pause.png");
	pauseIcon.addPixmap(QPixmap(":/22x22/media-playback-start.png"), QIcon::Normal, QIcon::On);
	pauseButton_->setIcon(pauseIcon);
	pauseButton_->setIconSize(QSize(22, 22));
	pauseButton_->setCheckable(true);
	pauseButton_->setChecked(true);
	pauseButton_->setStyleSheet("QPushButton { border: 2px solid #8f8f91; border-radius: 6px;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(102,255,102), stop: 1 #dadbde);min-width: 140px; padding: 0px 5px 0 5px;} QPushButton:pressed { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa); } QPushButton:flat { border: none; /* no border for a flat push button */ } QPushButton:checked { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(102,102,255), stop: 1 #f6f7fa); }");
	hl->addWidget(pauseButton_);

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);
	vl->addWidget(topFrame);
	vl->addWidget(treeView_);

	connect(hideButton_, SIGNAL(toggled(bool)), treeView_, SLOT(setVisible(bool)));
	connect(pauseButton_, SIGNAL(toggled(bool)), this, SLOT(onPauseButtonClicked(bool)));
	connect(duplicateButton_, SIGNAL(clicked()), this, SLOT(onDuplicateButtonClicked()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));

	connect(actionRunner_->queueModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onQueueActionAddedOrRemoved()));
	connect(actionRunner_->queueModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(onQueueActionAddedOrRemoved()));

	connect(treeView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged()));

	connect(actionRunner_, SIGNAL(queuePausedChanged(bool)), this, SLOT(onQueuePaused(bool)));
}

void AMActionRunnerQueueView::onPauseButtonClicked(bool isPaused)
{
	pauseButton_->setText(isPaused ? "Queue Paused\n(Click to run)" : "Queue Running\n(Click to pause)");
	actionRunner_->setQueuePaused(isPaused);
}

void AMActionRunnerQueueView::onQueueActionAddedOrRemoved()
{
	headerSubTitle_->setText(QString("%1 actions in the workflow queue").arg(actionRunner_->queuedActionCount()));
}

void AMActionRunnerQueueView::onSelectionChanged()
{
	bool hasSelectedItems = (treeView_->selectionModel()->selectedRows().count() != 0);

	deleteButton_->setEnabled(hasSelectedItems);
	duplicateButton_->setEnabled(hasSelectedItems);
}

void AMActionRunnerQueueView::onDeleteButtonClicked()
{
	QModelIndexList selectedRows = treeView_->selectionModel()->selectedRows(0);
	foreach(QModelIndex i, selectedRows) {
		// todozzz.... For now, only delete top-level actions. Will need to figure out deleting actions inside loops later.
		if(!i.parent().isValid()) {
			actionRunner_->deleteActionInQueue(i.row());
		}
	}
}

void AMActionRunnerQueueView::onDuplicateButtonClicked()
{
	QList<int> indexesToCopy;

	QModelIndexList selectedRows = treeView_->selectionModel()->selectedRows(0);
	foreach(QModelIndex i, selectedRows) {
		// todozzz.... For now, only duplicate top-level actions. Will need to figure out deleting actions inside loops later.
		if(!i.parent().isValid()) {
			indexesToCopy << i.row();
		}
	}

	actionRunner_->duplicateActionsInQueue(indexesToCopy);
}

void AMActionRunnerQueueView::onQueuePaused(bool isPaused)
{
	pauseButton_->setChecked(isPaused);
}




//QModelIndex AMActionRunnerQueueModel::index(int row, int column, const QModelIndex &parent) const
//{
//	// column must be 0.
//	if(column != 0)
//		return QModelIndex();

//	// if parent is the root... This is an index for a top-level action in the queue.
//	if(!parent.isValid()) {
//		if(row < 0 || row >= actionRunner_->queuedActionCount())
//			return QModelIndex();
//		return createIndex(row, column, 0);
//	}
//	else {
//		AMAction* parentAction = actionAtIndex(parent);
//		if(!parentAction)
//			return QModelIndex();

//		// todozzzz If a loop action, provide indexes for child actions. The internal pointer will be a pointer to the parent (loop) action.
//		// for now:
//		return QModelIndex();
//	}
//}

//QModelIndex AMActionRunnerQueueModel::parent(const QModelIndex &child) const
//{
//	// if no parent pointer stored in the internal id, this is a top-level action
//	if(!child.internalPointer())
//		return QModelIndex();
//	else {
//		// so child.internalPointer() is our parent action. But we need to know what its row is in _its_ parent (which might be the top level.
//		AMAction* parentAction = qobject_cast<AMAction*>
//	}
//}

//int AMActionRunnerQueueModel::rowCount(const QModelIndex &parent) const
//{
//}

//int AMActionRunnerQueueModel::columnCount(const QModelIndex &parent) const
//{
//}

//QVariant AMActionRunnerQueueModel::data(const QModelIndex &index, int role) const
//{
//}

//bool AMActionRunnerQueueModel::hasChildren(const QModelIndex &parent) const
//{
//}

//AMAction * AMActionRunnerQueueModel::actionAtIndex(const QModelIndex &index)
//{
//	// if no parent pointer stored in the internal id, this is a top-level action
//	if(!index.internalPointer()) {
//		if(index.row() < 0 || index.row() >= actionRunner_->queuedActionCount())
//			return 0;
//		return actionRunner_->queuedActionAt(index.row());
//	}
//	else {
//		// the internal pointer is a pointer to a hierarchical loop action
//		AMAction* parentAction = qobject_cast<AMAction*>(index.internalPointer());
//		// todozzzz return sub action
//		return 0;
//	}
//}
