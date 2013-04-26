/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDatamanAppControllerForActions2.h"
#include "ui/AMBottomBar.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMRunExperimentInsert.h"
#include "ui/dataman/AMDataViewWithActionButtons.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"
#include "actions2/AMActionInfo.h"
#include "actions2/AMActionLog.h"
#include "actions2/actions/AMWaitActionInfo.h"
#include "actions2/actions/AMControlMoveActionInfo.h"
#include "actions2/actions/AMScanControllerActionInfo.h"
#include "actions2/actions/AMChangeRunActionInfo.h"

AMDatamanAppControllerForActions2::AMDatamanAppControllerForActions2(QObject *parent) :
	AMDatamanAppController(parent)
{
}


bool AMDatamanAppControllerForActions2::startupRegisterDatabases()
{
	if(!AMDatamanAppController::startupRegisterDatabases())
		return false;

	AMDbObjectSupport::s()->registerClass<AMActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMActionLog>();
	AMDbObjectSupport::s()->registerClass<AMWaitActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMControlMoveActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMScanControllerActionInfo>();
	AMDbObjectSupport::s()->registerClass<AMChangeRunActionInfo>();

	return true;
}

void AMDatamanAppControllerForActions2::onProgressUpdated(double elapsed, double total){
	bottomBar_->updateScanProgress(elapsed, total);
}

bool AMDatamanAppControllerForActions2::startupCreateUserInterface()
{
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, "Acquaman Startup: Populating User Interface");
	qApp->processEvents();
	settingsMasterView_ = 0;
	issueSubmissionView_ = 0;
	bottomBar_ = new AMBottomBar();
	// These buttons are never used.  Hiding them.
	bottomBar_->fullScreenButton->hide();
	bottomBar_->adjustScanFinishButton->hide();
	bottomBar_->restartScanButton->hide();
	mw_->addBottomWidget(bottomBar_);
	connect(bottomBar_, SIGNAL(addButtonClicked()), this, SLOT(onAddButtonClicked()));
	connect(bottomBar_, SIGNAL(pauseScanIssued()), this, SIGNAL(pauseScanIssued()));
	connect(bottomBar_, SIGNAL(resumeScanIssued()), this, SIGNAL(resumeScanIssued()));
	connect(bottomBar_, SIGNAL(stopScanIssued()), this, SIGNAL(stopScanIssued()));

	// Create panes in the main window:
	////////////////////////////////////

	// A heading for the scan editors
	scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans");


	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". See AMMainWindowModel for more information.
	////////////////////////////////////
	dataView_ = new AMDataViewWithActionButtons();
	dataView_->setWindowTitle("Data");

	QStandardItem* dataViewItem = new QStandardItem();
	dataViewItem->setData(qVariantFromValue((QWidget*)dataView_), AM::WidgetRole);
	dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	dataViewItem->setFont(font);
	dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);

	mw_->windowPaneModel()->appendRow(dataViewItem);

	runsParentItem_ = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");
	mw_->windowPaneModel()->initAliasItem(runsParentItem_, dataViewItem, "Runs", -1);
	dataViewItem->appendRow(runsParentItem_);
	experimentsParentItem_ = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	mw_->windowPaneModel()->initAliasItem(experimentsParentItem_, dataViewItem, "Experiments", -1);
	dataViewItem->appendRow(experimentsParentItem_);

	// Hook into the sidebar and add Run and Experiment links below these headings.
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::database("user"), runsParentItem_, experimentsParentItem_, this);
	connect(runExperimentInsert_, SIGNAL(newExperimentAdded(QModelIndex)), this, SLOT(onNewExperimentAdded(QModelIndex)));

	// connect the activated signal from the dataview to our own slot
	connect(dataView_, SIGNAL(selectionActivated(QList<QUrl>)), this, SLOT(onDataViewItemsActivated(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionActivatedSeparateWindows(QList<QUrl>)), this, SLOT(onDataViewItemsActivatedSeparateWindows(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionExported(QList<QUrl>)), this, SLOT(onDataViewItemsExported(QList<QUrl>)));
	connect(dataView_, SIGNAL(launchScanConfigurationsFromDb(QList<QUrl>)), this, SLOT(onLaunchScanConfigurationsFromDb(QList<QUrl>)));
	connect(dataView_, SIGNAL(fixCDF(QUrl)), this, SLOT(fixCDF(QUrl)));

	// When 'alias' links are clicked in the main window sidebar, we might need to notify some widgets of the details
	connect(mw_, SIGNAL(aliasItemActivated(QWidget*,QString,QVariant)), this, SLOT(onMainWindowAliasItemActivated(QWidget*,QString,QVariant)));
	/////////////////////////

	// Make connections:
	//////////////////////////////

	connect(mw_, SIGNAL(currentPaneChanged(QWidget*)), this, SLOT(onCurrentPaneChanged(QWidget*)));

	// show main window
	mw_->show();

	return true;
}
