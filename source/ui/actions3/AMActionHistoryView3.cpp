#include "AMActionHistoryView3.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/AMActionRegistry3.h"

#include "ui/actions3/AMActionHistoryModel.h"
#include "ui/actions3/AMActionHistoryTreeView.h"

#include "dataman/AMRun.h"
#include "dataman/AMUser.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include <QBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QToolButton>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QStringBuilder>
#include <QHeaderView>
#include <QDebug>

// AMActionHistoryView
////////////////////////////

AMActionHistoryView3::AMActionHistoryView3(AMActionRunner3 *actionRunner, AMDatabase *db, QWidget *parent) : QWidget(parent)
{
	actionRunner_ = actionRunner;
	db_ = db;
	shiftModifierUsed_ = false;

	model_ = new AMActionHistoryModel3(db_, this);
	model_->setMaximumActionsToDisplay(100);
	//QDateTime fourHoursAgo = QDateTime::currentDateTime().addSecs(-4*60*60);
	//model_->setVisibleDateTimeRange(fourHoursAgo);
	QDateTime everAgo = QDateTime();
	model_->setVisibleDateTimeRange(everAgo);

	// Setup UI
	//////////////////////
	isCollapsed_ = false;

	QFrame* topFrame = new QFrame();
	topFrame->setObjectName("topFrame");
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setStyleSheet("QFrame#topFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout(topFrame);
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	QToolButton* hideButton = new QToolButton();
	hideButton->setStyleSheet("QToolButton {\nborder: none;\nbackground-color: rgba(255, 255, 255, 0);\ncolor: white;\n image: url(:/22x22/arrow-white-down.png)} \nQToolButton::checked {\n	image: url(:/22x22/arrow-white-right.png);\n}\n");
	hideButton->setCheckable(true);
	hideButton->setChecked(false);
	hl->addWidget(hideButton);
	hl->addSpacing(10);

	QVBoxLayout* vl2 = new QVBoxLayout();
	vl2->setContentsMargins(0,0,0,0);
	vl2->setSpacing(0);
	headerTitle_ = new QLabel("Completed Actions");
	headerTitle_->setStyleSheet("color: white;\nfont: " AM_FONT_XLARGE_ "pt \"Lucida Grande\"");
	headerSubTitle_ = new QLabel("No actions to show.");
	headerSubTitle_->setStyleSheet("color: rgb(204, 204, 204);\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	vl2->addWidget(headerTitle_);
	vl2->addWidget(headerSubTitle_);
	hl->addLayout(vl2);
	hl->addSpacing(10);

	showMoreActionsButton_ = new QPushButton("Show More");
	hl->addWidget(showMoreActionsButton_);
	hl->addSpacing(40);
	hl->addStretch(0);

	reRunActionButton_ = new QPushButton("Re-run this action");
	reRunActionButton_->setDisabled(true);
	hl->addWidget(reRunActionButton_);
	hl->addSpacing(40);
	QLabel* showLabel = new QLabel("Show: ");
	showLabel->setStyleSheet("color: rgb(204, 204, 204);\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	hl->addWidget(showLabel);
	rangeComboBox_ = new QComboBox();
	rangeComboBox_->addItem("Last Hour");
	rangeComboBox_->addItem("Last 4 Hours");
	rangeComboBox_->addItem("Last Day");
	rangeComboBox_->addItem("This Run");
	rangeComboBox_->addItem("Ever");
	//rangeComboBox_->setCurrentIndex(1);
	rangeComboBox_->setCurrentIndex(4);
	hl->addWidget(rangeComboBox_);

	//treeView_ = new QTreeView();
	treeView_ = new AMActionHistoryTreeView3();
	treeView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	treeView_->setModel(model_);
	//treeView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView_->setSelectionBehavior(QAbstractItemView::SelectItems);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	treeView_->setAlternatingRowColors(true);
	//treeView_->setRootIsDecorated(false);	// gets rid of indentation at top level.
	//treeView_->setHeaderHidden(true);
	treeView_->scrollToBottom();
	scrolledToBottom_ = true;
	treeView_->header()->setStretchLastSection(false);
	treeView_->header()->setResizeMode(0, QHeaderView::Stretch);
	treeView_->header()->setResizeMode(1, QHeaderView::Fixed);
	treeView_->header()->setResizeMode(2, QHeaderView::Fixed);
	treeView_->header()->setResizeMode(3, QHeaderView::Fixed);
	treeView_->header()->resizeSection(1, 48);
	treeView_->header()->resizeSection(2, 180);
	treeView_->header()->resizeSection(3, 160);
	treeView_->setAttribute(Qt::WA_MacShowFocusRect, false);

	treeView_->setItemDelegate(new AMActionLogItemDelegate3(treeView_, this));

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);
	vl->addWidget(topFrame);
	vl->addWidget(treeView_);

	treeView_->setContextMenuPolicy(Qt::CustomContextMenu);

	// Make connections:
	////////////////

	connect(hideButton, SIGNAL(toggled(bool)), this, SLOT(collapse(bool)));

	connect(model_, SIGNAL(modelAboutToBeRefreshed()), this, SLOT(onModelAboutToBeRefreshed()));
	connect(model_, SIGNAL(modelRefreshed()), this, SLOT(onModelRefreshed()));

	connect(rangeComboBox_, SIGNAL(activated(int)), this, SLOT(onRangeComboBoxActivated(int)));
	connect(showMoreActionsButton_, SIGNAL(clicked()), this, SLOT(onShowMoreActionsButtonClicked()));

	connect(reRunActionButton_, SIGNAL(clicked()), this, SLOT(onReRunActionButtonClicked()));
	//connect(treeView_, SIGNAL(clicked(QModelIndex)), this, SLOT(onTreeViewClicked(QModelIndex)));
	connect(treeView_, SIGNAL(actuallySelectedByClicking(QModelIndex, bool, bool)), this, SLOT(onSelectedByClicked(QModelIndex, bool, bool)));
	connect(treeView_, SIGNAL(actuallyDeselectedByClicking(QModelIndex, bool, bool)), this, SLOT(onDeselectedByClicked(QModelIndex, bool, bool)));
	connect(treeView_, SIGNAL(clearedByClicking()), this, SLOT(onClearedByClicked()));
	connect(treeView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged()));

	connect(treeView_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

void AMActionHistoryView3::onCustomContextMenuRequested(QPoint point)
{
	QMenu popup(treeView_);
	QAction *temp = popup.addAction("");

	if (treeView_->selectionModel()->selectedIndexes().size() == 0){

		temp->setText("Re-run this action");
		temp->setEnabled(false);
	}
	else if (treeView_->selectionModel()->selectedIndexes().size() == 1)
		temp->setText("Re-run this action");
	else
		temp->setText("Re-run these actions");

	temp = popup.exec(treeView_->mapToGlobal(point));

	if (temp && (temp->text() == "Re-run this action" || temp->text() == "Re-run these actions"))
		onReRunActionButtonClicked();
}

void AMActionHistoryView3::onModelAboutToBeRefreshed()
{
	// we're scrolled to the bottom if the vertical scroll bar value is at maximum. Remember that so we can go back to the bottom after the refresh.
	scrolledToBottom_ = (treeView_->verticalScrollBar()->value() == treeView_->verticalScrollBar()->maximum());
	// qDebug() << "Scrolled to bottom:" << scrolledToBottom_;
}

void AMActionHistoryView3::onModelRefreshed()
{
	// if we were scrolled to the most recent (bottom) action before the refresh, let's scroll back there.
	if(scrolledToBottom_) {
		// doesn't work: treeView_->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);

		// also doesn't work... Sometimes only goes most of the way there.
		treeView_->scrollToBottom();
		// looks like if the tree view is not visible at this time, this won't take effect. Adding another check in our showEvent() to scroll the treeView_ to the bottom if should be scrolledToBottom_
		// qDebug() << "ScrollING to bottom";
	}

	// update subtitle text with how many, of how many total.
	QString filterText;
	switch(rangeComboBox_->currentIndex()) {
	case 0: filterText = " in the last hour."; break;
	case 1: filterText = " in the last 4 hours."; break;
	case 2: filterText = " in the last day."; break;
	case 3: filterText = " in this run."; break;
	case 4: filterText = " ever done."; break;
	}

	int actionsShown = model_->rowCount();
	int totalActions = model_->visibleActionsCount();
	if(actionsShown == 0) {
		showMoreActionsButton_->hide();
		headerSubTitle_->setText("No actions" % filterText);
	}
	else if(actionsShown == totalActions) {
		showMoreActionsButton_->hide();
		headerSubTitle_->setText("Showing " % QString::number(actionsShown) % " actions" % filterText);
	}
	else {
		showMoreActionsButton_->show();
		headerSubTitle_->setText("Showing " % QString::number(actionsShown) % " of " % QString::number(totalActions) % " actions" % filterText);
	}
}

void AMActionHistoryView3::onShowMoreActionsButtonClicked()
{
	model_->setMaximumActionsToDisplay(model_->maximumActionsToDisplay()*2);
}

void AMActionHistoryView3::onRangeComboBoxActivated(int rangeIndex)
{
	QDateTime oldest = QDateTime::currentDateTime();

	switch(rangeIndex) {
	case 0: // last hour
		oldest = oldest.addSecs(-60*60);
		break;
	case 1: // last 4 hours
		oldest = oldest.addSecs(-4*60*60);
		break;
	case 2: // last day
		oldest = oldest.addDays(-1);
		break;
	case 3: // this run
	{
		AMRun currentRun;
		currentRun.loadFromDb(AMDatabase::database("user"), AMUser::user()->currentRunId());
		oldest = currentRun.dateTime();
	}
		break;
	case 4: // ever
	default:
		oldest = QDateTime();
		break;
	}

	model_->setVisibleDateTimeRange(oldest);
}



void AMActionHistoryView3::onReRunActionButtonClicked()
{
	// we need a valid action runner to re-queue actions. This button shouldn't be enabled if there is no actionRunner_, but just in case:
	if(!actionRunner_)
		return;

	// go through all selected rows.
	//foreach(QModelIndex i, treeView_->selectionModel()->selectedRows(0)) {
	foreach(QModelIndex i, treeView_->selectionModel()->selectedIndexes()) {
		AMActionLogItem3* item = model_->logItem(i);
		if(!item)
			continue;
		qDebug() << "Found item " << item->id() << " index row " << i.row();
		/*
		// load the full actionLog.
		AMActionLog3 actionLog;
		if(!actionLog.loadFromDb(item->database(), item->id())) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -57, "Could not load the action log for id " % QString::number(item->id()) % " from the database. Please report this problem to the Acquaman developers."));
			continue;
		}
		if(!actionLog.info()) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -58, "Could not load the action info for the action log with id " % QString::number(item->id()) % " from the database. It is likely because the action info class hasn't been registered with the database system. Please report this problem to the Acquaman developers."));
			continue;
		}
		// make a copy of the AMActionInfo.
		AMActionInfo3* info = actionLog.info()->createCopy();

		// make an action (assuming the actionInfo is registered with a corresponding action)
		AMAction3* action = AMActionRegistry3::s()->createActionFromInfo(info);
		if(!action) {
			QMessageBox::warning(this, "Cannot re-run this action", "Could not re-run this action because running the '" % info->typeDescription() %  "' action isn't enabled for your beamline's version of Acquaman. If you don't think this should be the case, please report this to the Acquaman developers.");
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -59, "Could not re-run this action because running '" % actionLog.name() % "' isn't enabled for your beamline's version of Acquaman. If you don't think this should be the case, please report this to the Acquaman developers."));
			delete info;
			continue;
		}
		actionRunner_->addActionToQueue(action);
		*/
	}
}

void AMActionHistoryView3::onSelectionChanged()
{
	// can only re-run actions if we have a valid actionRunner_
	if(actionRunner_) {
		QModelIndexList selectedIndices = treeView_->selectionModel()->selectedIndexes();
		for(int x = 0; x < selectedIndices.count(); x++){

		}

		int numSelected = treeView_->selectionModel()->selectedIndexes().count();
		if(numSelected == 0) {
			reRunActionButton_->setDisabled(true);
		}
		else if(numSelected == 1) {
			reRunActionButton_->setEnabled(true);
			reRunActionButton_->setText("Re-run this action");
		}
		else {
			reRunActionButton_->setEnabled(true);
			reRunActionButton_->setText("Re-run these actions");
		}
	}
}

void AMActionHistoryView3::onSelectedByClicked(const QModelIndex &index, bool othersCleared, bool shiftModifierWasUsed){
	if(othersCleared)
		onClearedByClicked();
	if(index.isValid()){
		shiftModifierUsed_ = shiftModifierWasUsed;
		actuallyBeenClicked_.append(index);
		model_->markIndexAsSelected(index, treeView_);
	}
	treeView_->setActuallySelectedByClickingCount(actuallyBeenClicked_.count());

	QString clickedIds;
	for(int x = 0; x < actuallyBeenClicked_.count(); x++)
		clickedIds.append(QString(" %1").arg(model_->logItem(actuallyBeenClicked_.at(x))->id()));
}

void AMActionHistoryView3::onDeselectedByClicked(const QModelIndex &index, bool othersCleared, bool shiftModifierWasUsed){
	if(othersCleared)
		onClearedByClicked();
	if(index.isValid()){
		shiftModifierUsed_ = shiftModifierWasUsed;
		actuallyBeenClicked_.removeOne(index);
		model_->markIndexAsDeselected(index, treeView_);
	}
	treeView_->setActuallySelectedByClickingCount(actuallyBeenClicked_.count());

	QString clickedIds;
	for(int x = 0; x < actuallyBeenClicked_.count(); x++)
		clickedIds.append(QString(" %1").arg(model_->logItem(actuallyBeenClicked_.at(x))->id()));
}

void AMActionHistoryView3::onClearedByClicked(){
	for(int x = 0; x < actuallyBeenClicked_.count(); x++)
		model_->markIndexAsDeselected(actuallyBeenClicked_.at(x), treeView_);
	actuallyBeenClicked_.clear();
}

void AMActionHistoryView3::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);

	if(scrolledToBottom_)
		treeView_->scrollToBottom();
}


void AMActionHistoryView3::collapse(bool doCollapse)
{
	bool wasCollapsed = isCollapsed();
	treeView_->setHidden(doCollapse);
	isCollapsed_ = doCollapse;

	if(isCollapsed_ != wasCollapsed)
		emit collapsed(doCollapse);
}


