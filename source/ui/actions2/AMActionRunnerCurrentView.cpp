#include "AMActionRunnerCurrentView.h"
#include "actions2/AMActionRunner.h"
#include "actions2/AMAction.h"
#include "actions2/AMNestedAction.h"

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

#include <QDebug>

AMActionRunnerCurrentView::AMActionRunnerCurrentView(AMActionRunner* actionRunner, QWidget *parent) :
    QWidget(parent)
{
	actionRunner_ = actionRunner;

	// setup UI
	QFrame* topFrame = new QFrame();
	topFrame->setObjectName("topFrame");
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setStyleSheet("QFrame#topFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout(topFrame);
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	QVBoxLayout* vl2 = new QVBoxLayout();
	vl2->setContentsMargins(0,0,0,0);
	vl2->setSpacing(0);
	headerTitle_ = new QLabel("Current Action");
	headerTitle_->setStyleSheet("color: white;\nfont: 20pt \"Lucida Grande\"");
	headerSubTitle_ = new QLabel("No action running.");
	headerSubTitle_->setStyleSheet("color: rgb(204, 204, 204);\nfont: 12pt \"Lucida Grande\"");
	vl2->addWidget(headerTitle_);
	vl2->addWidget(headerSubTitle_);
	hl->addLayout(vl2);
	hl->addSpacing(40);
	hl->addStretch(0);

	timeElapsedLabel_ = new QLabel("0:00");
	timeElapsedLabel_->setStyleSheet("color: white;\nfont: 18pt \"Lucida Grande\"");
	hl->addWidget(timeElapsedLabel_);
	hl->addSpacing(10);
	progressBar_ = new QProgressBar();
	progressBar_->setMaximumWidth(600);
	hl->addWidget(progressBar_, 1);
	hl->addSpacing(10);
	timeRemainingLabel_ = new QLabel("0:00");
	timeRemainingLabel_->setStyleSheet("color: white;\nfont: 18pt \"Lucida Grande\"");
	hl->addWidget(timeRemainingLabel_);
	hl->addSpacing(20);

	pauseButton_ = new QPushButton(QIcon(":/22x22/media-playback-pause.png"), "Pause");
	pauseButton_->setEnabled(false);
	hl->addWidget(pauseButton_);
	cancelButton_ = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	cancelButton_->setEnabled(false);
	hl->addWidget(cancelButton_);



	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);
	vl->addWidget(topFrame);

	currentActionView_ = new QTreeView();
	currentActionView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	currentActionView_->setMaximumHeight(48);
	currentActionView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	vl->addWidget(currentActionView_);

	currentActionView_->setModel(new AMActionRunnerCurrentModel(actionRunner_, this));
	currentActionView_->setSelectionMode(QAbstractItemView::NoSelection);
	currentActionView_->setHeaderHidden(true);
	currentActionView_->setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(actionRunner_, SIGNAL(currentActionChanged(AMAction*)), this, SLOT(onCurrentActionChanged(AMAction*)));
	connect(cancelButton_, SIGNAL(clicked()), actionRunner_, SLOT(cancelCurrentAction()));
	connect(pauseButton_, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));

	connect(actionRunner_, SIGNAL(currentActionStatusTextChanged(QString)), this, SLOT(onStatusTextChanged(QString)));
	connect(actionRunner_, SIGNAL(currentActionExpectedDurationChanged(double)), this, SLOT(onExpectedDurationChanged(double)));
	connect(actionRunner_, SIGNAL(currentActionProgressChanged(double,double)), this, SLOT(onProgressChanged(double,double)));
	connect(actionRunner_, SIGNAL(currentActionStateChanged(int,int)), this, SLOT(onStateChanged(int,int)));

	QTimer* timeUpdateTimer = new QTimer(this);
	connect(timeUpdateTimer, SIGNAL(timeout()), this, SLOT(onTimeUpdateTimer()));
	timeUpdateTimer->start(1000);
}

void AMActionRunnerCurrentView::onCurrentActionChanged(AMAction* nextAction)
{
	cancelButton_->setDisabled((nextAction == 0));
	if(nextAction && nextAction->state() == AMAction::Paused) {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
		pauseButton_->setText("Resume");
	}
	else {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
		pauseButton_->setText("Pause");
	}

	// model will handle the tree view on its own. But... we want to make some more visible room if it's a nested action, to show the sub-actions.
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(nextAction);
	if(nestedAction) {
		currentActionView_->setMaximumHeight(qMin(168, int((nestedAction->subActionCount()+1)*48)));
		QTimer::singleShot(0, currentActionView_, SLOT(expandAll()));
	}
	else {
		currentActionView_->setMaximumHeight(48);
	}

	if(nextAction) {
		headerTitle_->setText("Current Action: " % nextAction->info()->typeDescription());
		headerSubTitle_->setText("<b>Status</b>: " % nextAction->statusText());
		timeElapsedLabel_->setText("0:00");
		double expectedDuration = nextAction->expectedDuration();
		timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration) : "?:??");
	}
	else {
		headerTitle_->setText("Current Action");
		headerSubTitle_->setText("No action running.");
		timeElapsedLabel_->setText("-:--");
		timeRemainingLabel_->setText("-:--");
		progressBar_->setRange(0,1);
		progressBar_->setValue(0);
	}
}


void AMActionRunnerCurrentView::onStatusTextChanged(const QString &newStatus)
{
	headerSubTitle_->setText("<b>Status</b>: " % newStatus);
}

void AMActionRunnerCurrentView::onExpectedDurationChanged(double totalSeconds)
{
	double elapsed = actionRunner_->currentAction()->elapsedTime();
	qDebug() << "In on expected duration changed: " << totalSeconds;
	if(totalSeconds > 0)
		timeRemainingLabel_->setText(formatSeconds(totalSeconds-elapsed));
	else
		timeRemainingLabel_->setText("?:??");
}

void AMActionRunnerCurrentView::onProgressChanged(double numerator, double denominator)
{
	progressBar_->setRange(0, int(denominator*100));
	progressBar_->setValue(int(numerator*100));
}

void AMActionRunnerCurrentView::onTimeUpdateTimer()
{
	AMAction* currentAction = actionRunner_->currentAction();
	if(currentAction) {
		double elapsed = currentAction->runningTime();
		double expectedDuration = currentAction->expectedDuration();
		qDebug() << "In time update timer: expected duration:" << expectedDuration;
		timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration-elapsed) : "?:??");
		timeElapsedLabel_->setText(formatSeconds(elapsed));
	}
}

QString AMActionRunnerCurrentView::formatSeconds(double seconds)
{
	if(seconds < 0)
		return QString("?:??");

	QString rv;

	if(seconds > 60*60*24) {
		QDateTime now = QDateTime::currentDateTime();
		QDateTime later = now.addSecs(qRound(seconds));

		rv.append(QString("%1 days, ").arg(now.daysTo(later)));
	}

	QTime t(0,0,0);
	t = t.addSecs(qRound(seconds));
	if(seconds > 60*60)
		rv.append(t.toString("h:m:ss"));
	else
		rv.append(t.toString("m:ss"));

	return rv;
}

void AMActionRunnerCurrentView::onPauseButtonClicked()
{
	AMAction* currentAction = actionRunner_->currentAction();
	if(!currentAction)
		return;

	if(currentAction->state() == AMAction::Paused) {
		currentAction->resume();
		return;
	}

	if((currentAction->state() == AMAction::Running || currentAction->state() == AMAction::WaitingForPrereqs) && currentAction->pause())
		;	// successfully paused; do nothing.
	else
		QMessageBox::warning(this, "This action can't be paused", QString("This '%1' action cannot be paused right now.\n\n(Some actions just can't be paused, and others can't be paused at certain points in time.)").arg(currentAction->info()->typeDescription()), QMessageBox::Ok);
}

void AMActionRunnerCurrentView::onStateChanged(int state, int previousState)
{
	if(state == AMAction::Paused) {
		pauseButton_->setText("Resume");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
	}

	if(previousState == AMAction::Resuming) {
		pauseButton_->setText("Pause");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
	}

	// Can pause or resume from only these states:
	pauseButton_->setEnabled(state == AMAction::Running || state == AMAction::WaitingForPrereqs || state == AMAction::Paused);
}



AMActionRunnerCurrentModel::AMActionRunnerCurrentModel(AMActionRunner *actionRunner, QObject *parent) : QAbstractItemModel(parent)
{
	actionRunner_ = actionRunner;
	currentAction_ = 0;

	connect(actionRunner_, SIGNAL(currentActionChanged(AMAction*)), this, SLOT(onCurrentActionChanged(AMAction*)));
}

QModelIndex AMActionRunnerCurrentModel::index(int row, int column, const QModelIndex &parent) const
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
		AMNestedAction* parentAction = qobject_cast<AMNestedAction*>(actionAtIndex(parent));
		if(!parentAction) {
			qWarning() << "AMActionRunnerCurrentModel: Warning: Requested child index with invalid parent action.";
			return QModelIndex();
		}
		if(row < 0 || row >= parentAction->subActionCount()) {
			qWarning() << "AMActionRunnerCurrentModel: Warning: sub-action not found at child index row" << row << "with parent" << parent;
			return QModelIndex();
		}
		return createIndex(row, 0, parentAction->subActionAt(row));
	}
}

QModelIndex AMActionRunnerCurrentModel::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	AMAction* childAction = actionAtIndex(child);
	if(!childAction)
		return QModelIndex();

	// if childAction->parentAction() returns 0 (ie: no parent -- its at the top level) then this will return an invalid index, meaning it has no parent.
	return indexForAction(childAction->parentAction());
}

int AMActionRunnerCurrentModel::rowCount(const QModelIndex &parent) const
{
	// top level: the row count is 1 if the action runner has a current action, otherwise 0.
	if(!parent.isValid()) {
		return currentAction_ ? 1 : 0;
	}

	// otherwise, parent must represent an AMNestedAction
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(actionAtIndex(parent));
	if(nestedAction)
		return nestedAction->subActionCount();
	else
		return 0;
}

int AMActionRunnerCurrentModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 1;
}

QVariant AMActionRunnerCurrentModel::data(const QModelIndex &index, int role) const
{
	AMAction* action = actionAtIndex(index);
	if(!action) {
		qWarning() << "AMActionRunnerQueueModel: Warning: No action at index " << index;
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

Qt::ItemFlags AMActionRunnerCurrentModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}

bool AMActionRunnerCurrentModel::hasChildren(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return true;	// top level: must have children.
	else {
		// other levels: have children if its a nested action, and the nested action has children.
		AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(actionAtIndex(parent));
		return (nestedAction && nestedAction->subActionCount() > 0);
	}
}

AMAction * AMActionRunnerCurrentModel::actionAtIndex(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;
	return static_cast<AMAction*>(index.internalPointer());
}

QModelIndex AMActionRunnerCurrentModel::indexForAction(AMAction *action) const
{
	if(!action)
		return QModelIndex();

	AMNestedAction* parentAction = action->parentAction();
	if(!parentAction) {
		// action is in the top-level. It must be the current action.
		if(action == currentAction_)
			return createIndex(0, 0, action);
		else {
			qWarning() << "AMActionRunnerCurrentModel: Warning: Action not found as the current action.";
			return QModelIndex();
		}
	}
	else {
		// we do a have parent action. Do a linear search for ourself in the parent AMNestedAction to find our row.
		int row = parentAction->indexOfSubAction(action);
		if(row == -1) {
			qWarning() << "AMActionRunnerCurrentModel: Warning: action not found in nested action.";
			return QModelIndex();
		}
		return createIndex(row, 0, action);
	}
}

void AMActionRunnerCurrentModel::onCurrentActionChanged(AMAction *newCurrentAction)
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
