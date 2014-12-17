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


#include "AMActionRunnerCurrentView3.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMAction3.h"
#include "actions3/AMListAction3.h"
#include "actions3/editors/AMLiveLoopActionEditor3.h"
#include "ui/actions3/AMCancelActionPrompt.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include "AMQEvents.h"

#include <QTreeView>
#include <QBoxLayout>
#include <QFrame>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QProgressBar>
#include <QFrame>
#include <QStringBuilder>
#include <QTimer>
#include <QMessageBox>
#include <QPixmapCache>
#include <QMenu>
#include <QMessageBox>
#include <QCheckBox>

AMActionRunnerCurrentView3::AMActionRunnerCurrentView3(AMActionRunner3* actionRunner, QWidget *parent)
	: AMActionRunnerCurrentViewBase(actionRunner, parent)
{
	// setup UI
	QFrame* topFrame = new QFrame();
	topFrame->setObjectName("topFrame");
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setStyleSheet("QFrame#topFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout();
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	QVBoxLayout* vl2 = new QVBoxLayout();
	vl2->setContentsMargins(0,0,0,0);
	vl2->setSpacing(0);
	headerTitle_ = new QLabel("Current Action");
	headerTitle_->setStyleSheet("color: white;\nfont: " AM_FONT_XLARGE_ "pt \"Lucida Grande\"");
	headerSubTitle_ = new QLabel("No action running.");
	headerSubTitle_->setStyleSheet("color: rgb(204, 204, 204);\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	vl2->addWidget(headerTitle_);
	vl2->addWidget(headerSubTitle_);
	hl->addLayout(vl2);
	hl->addSpacing(40);
	hl->addStretch(0);

	timeElapsedLabel_->setStyleSheet("color: white;\nfont: " AM_FONT_LARGE_ "pt \"Lucida Grande\"");
	hl->addWidget(timeElapsedLabel_);
	hl->addSpacing(10);

	progressBar_ = new QProgressBar();
	progressBar_->setObjectName("progressBar");
	progressBar_->setStyleSheet("#progressBar { border: 1px solid black; color: white; border-radius: 7px; background: gray; } #progressBar::chunk { background: rgb(60,119,197); border-radius: 7px; border: 0px; }");
	progressBar_->setMaximumWidth(600);
	progressBar_->setAlignment(Qt::AlignCenter);
	progressBar_->setRange(0,1);
	progressBar_->setValue(0);
	hl->addWidget(progressBar_, 1);
	hl->addSpacing(10);

	timeRemainingLabel_->setStyleSheet("color: white;\nfont: " AM_FONT_LARGE_ "pt \"Lucida Grande\"");
	hl->addWidget(timeRemainingLabel_);
	hl->addSpacing(20);

	pauseButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	pauseButton_->setText("Pause");
	pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
	pauseButton_->setEnabled(false);
	hl->addWidget(pauseButton_);

	skipButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	skipButton_->setText("Skip");
	skipButton_->setIcon(QIcon(":/media-seek-forward.png"));
	skipButton_->setEnabled(false);
	hl->addWidget(skipButton_);

	cancelButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	cancelButton_->setText("Cancel");
	cancelButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));
	cancelButton_->setEnabled(false);
	hl->addWidget(cancelButton_);

	topFrame->setLayout(hl);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);
	vl->addWidget(topFrame);

	currentActionView_ = new QTreeView();
	currentActionView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	currentActionView_->setMaximumHeight(48);
	currentActionView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	vl->addWidget(currentActionView_);

	currentActionView_->setModel(new AMActionRunnerCurrentModel3(actionRunner_, this));
	currentActionView_->setSelectionMode(QAbstractItemView::NoSelection);
	currentActionView_->setHeaderHidden(true);
	currentActionView_->setAttribute(Qt::WA_MacShowFocusRect, false);
	currentActionView_->setItemDelegate(new AMActionRunnerCurrentItemDelegate3(this));

	setLayout(vl);
}

AMActionRunnerCurrentView3::~AMActionRunnerCurrentView3(){}

void AMActionRunnerCurrentView3::onCurrentActionChanged(AMAction3* action)
{
	AMActionRunnerCurrentViewBase::onCurrentActionChanged(action);

	// model will handle the tree view on its own. But... we want to make some more visible room if it's a list action, to show the sub-actions.
	AMListAction3* listAction = qobject_cast<AMListAction3*>(action);

	if(listAction) {

		currentActionView_->setMaximumHeight(qMin(168, int((listAction->subActionCount()+1)*48)));
		QTimer::singleShot(0, currentActionView_, SLOT(expandAll()));
	}

	else
		currentActionView_->setMaximumHeight(48);

	// Figure out the tool tip.
	if (listAction){

		if (listAction->subActionMode() == AMListAction3::Sequential && listAction->currentSubAction())
			whatIsRunning_ = "Action currently running:\n\n" % listAction->currentSubAction()->info()->name();

		// Parallel
		else{

			whatIsRunning_ = listAction->subActionCount() > 1 ? "Actions currently running:\n\n" : "Action currently running:\n\n";

			for (int i = 0; i < listAction->subActionCount(); i++)
				if (listAction->subActionAt(i)->state() == AMAction3::Running)
					whatIsRunning_.append(listAction->subActionAt(i)->info()->name() % ", ");

			whatIsRunning_.chop(2);
		}

	}

	else if (action){

		whatIsRunning_ = "Action currently running:\n\n" % action->info()->name();
	}

	else
		whatIsRunning_ = "";

	setToolTip(whatIsRunning_);

	if(!action) {

		headerTitle_->setText("Current Action");
		headerSubTitle_->setText("No action running.");
	}
}

void AMActionRunnerCurrentView3::updateDescriptionViews(AMAction3 *action)
{
	headerTitle_->setText("Current Action: " % action->info()->typeDescription());
	headerSubTitle_->setText(action->statusText());
	timeElapsedLabel_->setText("0:00");
	double expectedDuration = action->expectedDuration();
	timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration) : "N/A");
}

void AMActionRunnerCurrentView3::onStatusTextChanged(const QString &newStatus)
{
	headerSubTitle_->setText(newStatus);
}

// The QAbstractItemModel for handling the delegates and tree view aspects of the view.
/////////////////////////////////////////////////////////////////

AMActionRunnerCurrentModel3::~AMActionRunnerCurrentModel3(){}

AMActionRunnerCurrentModel3::AMActionRunnerCurrentModel3(AMActionRunner3 *actionRunner, QObject *parent) : QAbstractItemModel(parent)
{
	actionRunner_ = actionRunner;
	currentAction_ = 0;

	connect(actionRunner_, SIGNAL(currentActionChanged(AMAction3*)), this, SLOT(onCurrentActionChanged(AMAction3*)));
}

QModelIndex AMActionRunnerCurrentModel3::index(int row, int column, const QModelIndex &parent) const
{
	if(column != 0)
		return QModelIndex();

	// top-level: index for the current action.
	if(!parent.isValid()) {
		if(row != 0)
			return QModelIndex();
		if(!currentAction_)
			return QModelIndex();
		return createIndex(0, 0, currentAction_);
	}
	// There is a parent, so this is an index for a sub-action inside an AMNestedAction.
	else {
		AMListAction3* parentAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
		if(!parentAction) {
			AMErrorMon::debug(this, AMACTIONRUNNERCURRENTMODEL3_REQUESTED_CHILD_INDEX_WITH_INVALID_PARENT_ACTION, "Requested child index with invalid parent action.");
			return QModelIndex();
		}
		if(row < 0 || row >= parentAction->subActionCount()) {
			AMErrorMon::debug(this, AMACTIONRUNNERCURRENTMODEL3_SUBACTION_NOT_FOUND_AT_CHILD_INDEX_ROW, QString("Sub-action not found at child index row %1 with parent row %2").arg(row).arg(parent.row()) );
			return QModelIndex();
		}
		return createIndex(row, 0, parentAction->subActionAt(row));
	}
}

QModelIndex AMActionRunnerCurrentModel3::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	AMAction3* childAction = actionAtIndex(child);
	if(!childAction)
		return QModelIndex();

	// if childAction->parentAction() returns 0 (ie: no parent -- its at the top level) then this will return an invalid index, meaning it has no parent.
	return indexForAction(childAction->parentAction());
}

int AMActionRunnerCurrentModel3::rowCount(const QModelIndex &parent) const
{
	// top level: the row count is 1 if the action runner has a current action, otherwise 0.
	if(!parent.isValid()) {
		return currentAction_ ? 1 : 0;
	}

	// otherwise, parent must represent an AMNestedAction
	AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
	if(listAction)
		return listAction->subActionCount();
	else
		return 0;
}

int AMActionRunnerCurrentModel3::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 1;
}

QVariant AMActionRunnerCurrentModel3::data(const QModelIndex &index, int role) const
{
	AMAction3* action = actionAtIndex(index);
	if(!action) {
		AMErrorMon::debug(this, AMACTIONRUNNERCURRENTMODEL3_NO_ACTION_AT_INDEX, QString("No action at index row %1 column %2").arg(index.row()).arg(index.column()) );
		return QVariant();
	}

	if(role == Qt::DisplayRole) {
		return action->info()->shortDescription();
	}
	else if(role == Qt::DecorationRole) {
		QPixmap p;
		QString iconFileName = action->info()->iconFileName();
		if(QPixmapCache::find("AMActionIcon" % iconFileName, &p))
			return p;
		else {
			p.load(iconFileName);
			p = p.scaledToHeight(32, Qt::SmoothTransformation);
			QPixmapCache::insert("AMActionIcon" % iconFileName, p);
			return p;
		}
	}
	else if(role == Qt::SizeHintRole) {
		return QSize(-1, 48);
	}

	return QVariant();
}

Qt::ItemFlags AMActionRunnerCurrentModel3::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}

bool AMActionRunnerCurrentModel3::hasChildren(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return true;	// top level: must have children.
	else {
		// other levels: have children if its a list action, and the list action has children.
		AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
		return (listAction && listAction->subActionCount() > 0);
	}
}

AMAction3 * AMActionRunnerCurrentModel3::actionAtIndex(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;
	return static_cast<AMAction3*>(index.internalPointer());
}

QModelIndex AMActionRunnerCurrentModel3::indexForAction(AMAction3 *action) const
{
	if(!action)
		return QModelIndex();

	AMAction3* parentAction = action->parentAction();
	if(!parentAction) {
		// action is in the top-level. It must be the current action.
		if(action == currentAction_)
			return createIndex(0, 0, action);
		else {
			AMErrorMon::debug(this, AMACTIONRUNNERCURRENTMODEL3_ACTION_NOT_FOUND_AS_CURRENT_ACTION, "Action not found as the current action." );
			return QModelIndex();
		}
	}
	else {
		// we do a have parent action. Do a linear search for ourself in the parent AMNestedAction to find our row.
		int row = ((AMListAction3 *)parentAction)->indexOfSubAction(action);
		if(row == -1) {
			AMErrorMon::debug(this, AMACTIONRUNNERCURRENTMODEL3_ACTION_NOT_FOUND_IN_LIST_ACTION, "Action not found in list action." );
			return QModelIndex();
		}
		return createIndex(row, 0, action);
	}
}

void AMActionRunnerCurrentModel3::onCurrentActionChanged(AMAction3 *newCurrentAction)
{
	if(currentAction_) {
		beginRemoveRows(QModelIndex(), 0, 0);
		currentAction_ = 0;
		endRemoveRows();
	}

	if(newCurrentAction) {
		beginInsertRows(QModelIndex(), 0, 0);
		currentAction_ = newCurrentAction;
		endInsertRows();
	}
}

// AMActionRunnerCurrentItemDelegate
////////////////////////////////////////////////////////////////////

QWidget *AMActionRunnerCurrentItemDelegate3::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)

	const AMActionRunnerCurrentModel3* model = qobject_cast<const AMActionRunnerCurrentModel3*>(index.model());
	if(!model)
		return 0;

	AMAction3* action = model->actionAtIndex(index);
	if(!action)
		return 0;

	QWidget* rv = 0;
	if (index.row() == 0 && index.column() == 0 && qobject_cast<AMLoopAction3 *>(action)){

		rv = new AMLiveLoopActionEditor3(qobject_cast<AMLoopAction3 *>(action));
		rv->setParent(parent);
		rv->setFocusPolicy(Qt::StrongFocus);
		rv->setBackgroundRole(QPalette::Window);
		rv->setAutoFillBackground(true);
	}

	return rv;
}

bool AMActionRunnerCurrentItemDelegate3::eventFilter(QObject *object, QEvent *event)
{
	QWidget* widget = qobject_cast<QWidget*>(object);

	if(widget && event->type() == QEvent::KeyRelease) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if(keyEvent->key() == Qt::Key_Escape)
			emit closeEditor(widget);

	}
	return QObject::eventFilter(object, event);
}

AMActionRunnerCurrentItemDelegate3::~AMActionRunnerCurrentItemDelegate3(){}
