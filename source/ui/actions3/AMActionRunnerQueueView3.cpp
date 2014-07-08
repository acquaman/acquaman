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


#include "AMActionRunnerQueueView3.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/AMActionRegistry3.h"

#include "util/AMFontSizes.h"

#include <QTreeView>
#include <QBoxLayout>
#include <QFrame>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>

 AMActionRunnerQueueView3::~AMActionRunnerQueueView3(){}
AMActionRunnerQueueView3::AMActionRunnerQueueView3(AMActionRunner3* actionRunner, QWidget *parent) :
	QWidget(parent)
{
	actionRunner_ = actionRunner;

	isCollapsed_ = false;
	treeView_ = new QTreeView();
	treeView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	treeView_->setModel(actionRunner_->queueModel());
	treeView_->setHeaderHidden(true);
	treeView_->setAttribute(Qt::WA_MacShowFocusRect, false);
	treeView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	treeView_->setAlternatingRowColors(true);
	treeView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	treeView_->setAutoScroll(true);
	treeView_->setAutoScrollMargin(20);

	treeView_->setDragEnabled(true);
	treeView_->viewport()->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDragDropMode(QTreeView::DragDrop);

	treeView_->setItemDelegate(new AMActionRunnerQueueItemDelegate3(this));

	QFrame* topFrame = new QFrame();
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setObjectName("topFrame");
	topFrame->setStyleSheet("QFrame#topFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout(topFrame);
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	hideButton_ = new QToolButton();
	hideButton_->setStyleSheet("QToolButton {\nborder: none;\nbackground-color: rgba(255, 255, 255, 0);\ncolor: white;\n image: url(:/22x22/arrow-white-down.png)} \nQToolButton::checked {\n	image: url(:/22x22/arrow-white-right.png);\n}\n");
	hideButton_->setCheckable(true);
	hideButton_->setChecked(false);
	hl->addWidget(hideButton_);
	hl->addSpacing(10);

	QVBoxLayout* vl2 = new QVBoxLayout();
	vl2->setContentsMargins(0,0,0,0);
	vl2->setSpacing(0);
	headerTitle_ = new QLabel("Upcoming Actions");
	headerTitle_->setStyleSheet("color: white;\nfont: " AM_FONT_XLARGE_ "pt \"Lucida Grande\"");
	headerSubTitle_ = new QLabel(QString("%1 actions in the workflow queue.").arg(actionRunner_->queuedActionCount()));
	headerSubTitle_->setStyleSheet("color: rgb(204, 204, 204);\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
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

	pauseButton_ = new QPushButton;
	QIcon pauseIcon(":/22x22/media-playback-pause.png");
	pauseIcon.addPixmap(QPixmap(":/22x22/media-playback-start.png"), QIcon::Normal, QIcon::On);
	pauseButton_->setIcon(pauseIcon);
	pauseButton_->setIconSize(QSize(22, 22));
	pauseButton_->setCheckable(true);
	pauseButton_->setChecked(true);
	pauseButton_->setStyleSheet("QPushButton { border: 2px solid #8f8f91; border-radius: 6px;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(102,255,102), stop: 1 #dadbde);min-width: 140px; padding: 0px 5px 0 5px;} QPushButton:pressed { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa); } QPushButton:flat { border: none; /* no border for a flat push button */ } QPushButton:checked { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(102,102,255), stop: 1 #f6f7fa); } QPushButton:checked:pressed { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa); }");
	hl->addWidget(pauseButton_);
	pauseButtonConfiguration();

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);
	vl->addWidget(topFrame);
	vl->addWidget(treeView_);

	connect(hideButton_, SIGNAL(toggled(bool)), this, SLOT(collapse(bool)));
	connect(pauseButton_, SIGNAL(toggled(bool)), this, SLOT(onPauseButtonClicked(bool)));
	connect(duplicateButton_, SIGNAL(clicked()), this, SLOT(onDuplicateButtonClicked()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));

	connect(actionRunner_->queueModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onQueueActionAddedOrRemoved()));
	connect(actionRunner_->queueModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(onQueueActionAddedOrRemoved()));

	connect(treeView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged()));

	connect(actionRunner_, SIGNAL(queuePausedChanged(bool)), this, SLOT(onQueuePaused(bool)));
}

void AMActionRunnerQueueView3::onPauseButtonClicked(bool isPaused)
{
	actionRunner_->setQueuePaused(isPaused);
	pauseButtonConfiguration();
}

void AMActionRunnerQueueView3::onQueueActionAddedOrRemoved()
{
	headerSubTitle_->setText(QString("%1 actions in the workflow queue").arg(actionRunner_->queuedActionCount()));
	pauseButtonConfiguration();
}

void AMActionRunnerQueueView3::pauseButtonConfiguration()
{
	if (actionRunner_->queuedActionCount() == 0 && !actionRunner_->actionRunning())
		pauseButton_->setText("Queue Empty\n(No actions to run)");

	else if (actionRunner_->queuePaused() && actionRunner_->actionRunning())
		pauseButton_->setText("Queue Stopping\n(Click to resume)");

	else if (actionRunner_->queuePaused())
		pauseButton_->setText("Queue Stopped\n(Click to start)");

	else
		pauseButton_->setText("Queue Running\n(Click to stop at next action)");

	pauseButton_->setDisabled(actionRunner_->queuedActionCount() == 0);
}

void AMActionRunnerQueueView3::onSelectionChanged()
{
	QModelIndexList selectedIndexes = treeView_->selectionModel()->selectedRows();

	deleteButton_->setEnabled(selectedIndexes.count() != 0);
	duplicateButton_->setEnabled(allSelectedIndexesAtSameLevel(selectedIndexes));
}

void AMActionRunnerQueueView3::onDeleteButtonClicked()
{
	QModelIndexList selectedRows = treeView_->selectionModel()->selectedRows(0);
	actionRunner_->queueModel()->deleteActionsInQueue(selectedRows);
}

void AMActionRunnerQueueView3::onDuplicateButtonClicked()
{
	QModelIndexList selectedRows = treeView_->selectionModel()->selectedRows(0);
	actionRunner_->queueModel()->duplicateActionsInQueue(selectedRows);
}

void AMActionRunnerQueueView3::onQueuePaused(bool isPaused)
{
	pauseButton_->setChecked(isPaused);
}

bool AMActionRunnerQueueView3::allSelectedIndexesAtSameLevel(const QModelIndexList &selectedIndexes)
{
	if(selectedIndexes.isEmpty())
		return false;

	QModelIndex firstParent = selectedIndexes.at(0).parent();
	foreach(QModelIndex i, selectedIndexes)
		if(i.parent() != firstParent)
			return false;

	return true;
}


void AMActionRunnerQueueView3::collapse(bool doCollapse)
{
	bool wasCollapsed = isCollapsed();
	treeView_->setHidden(doCollapse);
	isCollapsed_ = doCollapse;

	if(isCollapsed_ != wasCollapsed)
		emit collapsed(doCollapse);
}


QWidget * AMActionRunnerQueueItemDelegate3::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)

	const AMActionRunnerQueueModel3* model = qobject_cast<const AMActionRunnerQueueModel3*>(index.model());
	if(!model)
		return 0;

	AMAction3* action = model->actionAtIndex(index);
	if(!action)
		return 0;

	QWidget* rv = AMActionRegistry3::s()->createEditorForInfo(action->info());
	if(rv) {
		rv->setParent(parent);
		rv->setFocusPolicy(Qt::StrongFocus);
		rv->setBackgroundRole(QPalette::Window);
		rv->setAutoFillBackground(true);
	}

	return rv;
}

#include <QKeyEvent>
bool AMActionRunnerQueueItemDelegate3::eventFilter(QObject *object, QEvent *event)
{
	QWidget* widget = qobject_cast<QWidget*>(object);

	if(widget && event->type() == QEvent::KeyRelease) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if(keyEvent->key() == Qt::Key_Escape)
			emit closeEditor(widget);

	}
	return QObject::eventFilter(object, event);
}


//void AMActionRunnerQueueItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//}




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
 AMActionRunnerQueueItemDelegate3::~AMActionRunnerQueueItemDelegate3(){}
