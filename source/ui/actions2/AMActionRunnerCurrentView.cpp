#include "AMActionRunnerCurrentView.h"
#include "actions2/AMActionRunner.h"
#include "actions2/AMAction.h"

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
	vl->addWidget(currentActionView_);

	currentActionModel_ = new QStandardItemModel(this);
	currentActionView_->setModel(currentActionModel_);
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

	currentActionModel_->removeRow(0); // harmless if no row yet... Will just return false.

	if(nextAction) {
		currentActionModel_->appendRow(new AMActionQueueModelItem(nextAction));
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



AMActionQueueModelItem::AMActionQueueModelItem(AMAction *action) : QStandardItem()
{
	action_ = action;

//	// is it a nested action? then include drop-enabled flag.
//	if(qobject_cast<AMNestedAction*>(action))
//		setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
//	else
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

}


QVariant AMActionQueueModelItem::data(int role) const
{
	if(role == Qt::DisplayRole) {
		return action_->info()->shortDescription();
	}
	else if(role == Qt::DecorationRole) {
		QPixmap p;
		QString iconFileName = action_->info()->iconFileName();
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
	return QStandardItem::data(role);
}
