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


#include "AMDataView.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDateTimeUtils.h"
#include "dataman/AMUser.h"
#include "dataman/AMScan.h"


#include <QScrollBar>
#include <QApplication>
#include <QStringBuilder>
#include <QPushButton>

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QApplication>

#include "util/AMFontSizes.h"

AMDataView::AMDataView(AMDatabase* database, QWidget *parent) :
	QWidget(parent)
{
	db_ = database;
	runId_ = experimentId_ = -1;
	scansTableName_ = AMDbObjectSupport::s()->tableNameForClass<AMScan>();
	selectedUrlsUpdateRequired_ = true;

	itemSize_ = 50;

	setupUi(this);
	headingLabel_->setStyleSheet("font: " AM_FONT_XLARGE_ "pt \"Lucida Grande\";color: rgb(79, 79, 79);");

	// add additional UI components: the QGraphicsView and QGraphicsScene
	////////////////////////////
	gview_ = this->graphicsView;
	gview_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	gview_->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	gscene_ = new QGraphicsScene(this);
	// This is necessary to avoid Qt bug https://bugreports.qt.nokia.com/browse/QTBUG-18021
	gscene_->setItemIndexMethod(QGraphicsScene::NoIndex);
	// We want to filter out some scene events (mouse press, etc.) so that the scene doesn't mess with our handling of selection in thumbnail view.
	gscene_->installEventFilter(this);

	connect(gscene_, SIGNAL(selectionChanged()), this, SLOT(onScanItemsSelectionChanged()));
	connect(gview_, SIGNAL(clicked(QPoint)), this, SLOT(onViewClicked(QPoint)));
	connect(gview_, SIGNAL(doubleClicked(QPoint)), this, SLOT(onViewDoubleClicked(QPoint)));
	connect(gview_, SIGNAL(dragStarted(QPoint, QPoint)), this, SLOT(onDragStarted(QPoint, QPoint)));
	connect(gview_, SIGNAL(dragMoved(QPoint,QPoint)), this, SLOT(onDragMoved(QPoint,QPoint)));
	connect(gview_, SIGNAL(dragEnded(QPoint,QPoint)), this, SLOT(onDragEnded(QPoint,QPoint)));

	connect(gview_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	gview_->setContextMenuPolicy(Qt::CustomContextMenu);

	gview_->setScene(gscene_);
	gwidget_ = new AMLayoutControlledGraphicsWidget();
	gscene_->addItem(gwidget_);
	sectionLayout_ = new QGraphicsLinearLayout(Qt::Vertical);
	sectionLayout_->setContentsMargins(0,0,0,0);
	sectionLayout_->setSpacing(0);
	gwidget_->setLayout(sectionLayout_);
	gwidget_->setParent(this); 	// this refers to the QObject tree parent, not the QGraphicsItem tree parent
	connect(gwidget_, SIGNAL(resized()), this, SLOT(adjustViewScrollableArea()));

	rubberBand_ = 0;

	// retrieve user name and set heading:
	///////////////////////
	retrieveUserName();
	headingLabel_->setText(userName_ + "Data");

	// setup button group and make viewmode buttons checkable:
	viewModeButtonGroup_ = new QButtonGroup(this);
	viewModeButtonGroup_->addButton(viewModeB1_, AMDataViews::ThumbnailView);
	viewModeButtonGroup_->addButton(viewModeB2_, AMDataViews::ListView);
	viewModeButtonGroup_->addButton(viewModeB3_, AMDataViews::FlowView);
	viewModeButtonGroup_->addButton(viewModeB4_, AMDataViews::DetailView);
	viewModeB1_->setChecked(true);

	// connect buttons
	/////////////////////////////
	connect(viewModeButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(setViewMode(int)));
	connect(organizeModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onOrganizeModeBoxCurrentIndexChanged(int)));

	// pick up database changes...
	//////////////////////////////
	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseItemCreatedOrRemoved(QString,int)), Qt::QueuedConnection);
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseItemCreatedOrRemoved(QString,int)), Qt::QueuedConnection);
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseItemCreatedOrRemoved(QString,int)), Qt::QueuedConnection);
	connect(&afterDbChangedFnCall_, SIGNAL(executed()), this, SLOT(afterDatabaseItemChanged()));


	// change this to change the default first view. (In this case, we show all runs)
	viewMode_ = AMDataViews::ThumbnailView;
	organizeMode_ = AMDataViews::OrganizeRuns;
	runId_ = -3;
	experimentId_ = -3;
	viewRequiresRefresh_ = true;

	// Default: show all runs:
	showRun();

	// size properly:
	onResize();
}


void AMDataView::retrieveUserName() {

	userName_ = AMUser::user()->name();

	if(!userName_.isEmpty()) {

		if(userName_.endsWith('s'))
			userName_.append("' ");
		else
			userName_.append("'s ");
	}
	else
		userName_ = "My ";
}

/// setup this view to show a specific run (or use \c runId = -1 to see all runs)
void AMDataView::showRun(int runId) {

	if(runId_ == runId && runOrExp_ && !viewRequiresRefresh_)
		return;

	gview_->verticalScrollBar()->setValue(0);

	runId_ = runId;
	runOrExp_ = true;
	if(runId_ < 0)
		organizeMode_ = AMDataViews::OrganizeRuns;
	if(runId_ >= 0 && (organizeMode_ == AMDataViews::OrganizeRuns || organizeMode_ == AMDataViews::OrganizeExperiments))
		organizeMode_ = AMDataViews::OrganizeNone;
	refreshView();
}


/// setup this view to show a specific experiment (or use \c experimentId = -1 to see all experiments)
void AMDataView::showExperiment(int experimentId) {

	if(experimentId == experimentId_ && !runOrExp_ && !viewRequiresRefresh_)
		return;

	gview_->verticalScrollBar()->setValue(0);

	experimentId_ = experimentId;
	runOrExp_ = false;
	if(experimentId_ < 0)
		organizeMode_ = AMDataViews::OrganizeExperiments;
	if(experimentId_ >= 0 && (organizeMode_ == AMDataViews::OrganizeRuns || organizeMode_ == AMDataViews::OrganizeExperiments))
		organizeMode_ = AMDataViews::OrganizeNone;
	refreshView();
}

/// Set the headers used to separate and organize scans within a view. The \c mode is one of AMDataViews::OrganizeMode.
void AMDataView::setOrganizeMode(int mode) {

	if(organizeMode_ == mode)
		return;

	gview_->verticalScrollBar()->setValue(0);

	if(mode < AMDataViews::OrganizeNone || mode > AMDataViews::OrganizeElements)
		return;

	// one condition not allowed: can't set organizeMode to OrganizeRuns when we're only showing one run:
	if( !(runOrExp_ && runId_ >= 0 && mode == AMDataViews::OrganizeRuns)
			&&
			!(!runOrExp_ && experimentId_ >= 0 && mode == AMDataViews::OrganizeExperiments)
			) {
		organizeMode_ = (AMDataViews::OrganizeMode)mode;
		refreshView();
	}

	organizeModeBox_->setCurrentIndex(organizeModeBox_->findData(organizeMode_));

}

/// Set the view mode, where \c mode is one of AMDataViews::ViewMode (ThumbnailView, ListView, FlowView, or DetailView)
void AMDataView::setViewMode(int mode) {

	if(mode < AMDataViews::ThumbnailView || mode > AMDataViews::DetailView)
		return;

	if(viewMode_ == mode)
		return;

	gview_->verticalScrollBar()->setValue(0);

	viewMode_ = (AMDataViews::ViewMode)mode;
	viewModeButtonGroup_->button(viewMode_)->setChecked(true);


	/// \todo optimization: changing the view mode doesn't necessarily need a refreshView, which would delete all the sections. Instead, we could have a AMDataViewSection::setViewMode().
	refreshView(); // if removing this, make sure to set selectedUrlsUpdateRequired_ .
}

/// called when the combo box is used to change the organizeMode
void AMDataView::onOrganizeModeBoxCurrentIndexChanged(int newIndex) {
	setOrganizeMode(organizeModeBox_->itemData(newIndex).toInt());
}

#include "ui/AMThumbnailScrollViewer.h"

// Called when the scene selection changes (useful for AMDataViewSectionThumbnailView)
void AMDataView::onScanItemsSelectionChanged() {
	selectedUrlsUpdateRequired_ = true;
	emit selectionChanged();
}

void AMDataView::updateSelectedUrls() const {

	if(!selectedUrlsUpdateRequired_)
		return;

	selectedUrlsUpdateRequired_ = false;
	selectedUrls_.clear();


	// Unfortunately we have a different method of measuring the selection, depending on the view mode.
	switch(viewMode_) {

	case AMDataViews::ThumbnailView: {

		QString prefix = "amd://" % db_->connectionName() % "/";
		QList<QGraphicsItem*> selectedItems = gscene_->selectedItems();

		foreach(QGraphicsItem* i, selectedItems) {
			AMThumbnailScrollGraphicsWidget* gw = qgraphicsitem_cast<AMThumbnailScrollGraphicsWidget*>(i);

			if(gw && gw->database() == db_ && gw->objectId() > 0) {
				selectedUrls_ << QUrl(prefix % gw->tableName() % "/" % QString::number(gw->objectId()));
			}
		}

	}
		break;

	case AMDataViews::ListView: {
		foreach(AMAbstractDataViewSection* s, sections_) {
			selectedUrls_.append(s->selectedItems());
		}

	}
		break;


	default:
		break;
	}
}

int AMDataView::numberOfSelectedItems() const
{
	if(!selectedUrlsUpdateRequired_)
		return selectedUrls_.count();

	int rv = 0;

	// selectedUrls_ isn't up to date.
	switch(viewMode_) {
	case AMDataViews::ThumbnailView:
		rv = gscene_->selectedItems().count();
		break;

	case AMDataViews::ListView:
		foreach(const AMAbstractDataViewSection* s, sections_)
			rv += s->numberOfSelectedItems();
		break;

	default:
		break;
	}

	return rv;
}



/// This function runs everytime showRun() or showExperiment() is called, or a change is made to the OrganizeMode or ViewMode.  It re-populates the view from scratch.
void AMDataView::refreshView() {

	viewRequiresRefresh_ = false;

	// delete the old views:
	// this disconnect is necessary so that selection changes while we're deleting don't trigger an unnecessary loop through the selected items.
	disconnect(gscene_, SIGNAL(selectionChanged()), this, SLOT(onScanItemsSelectionChanged()));
	while(!sections_.isEmpty())
		delete sections_.takeLast();
	connect(gscene_, SIGNAL(selectionChanged()), this, SLOT(onScanItemsSelectionChanged()));

	selectedUrlsUpdateRequired_ = true;
	emit selectionChanged();

	refreshOrganizeModeBox();

	// what we do from here depends on what we're showing (ie: run or experiment? all or just one?), as well as the organize mode.

	// Showing runs - all runs, or experiments - all experiments
	if( (runOrExp_ && runId_<0) || (!runOrExp_ && experimentId_<0) ) {

		if(runOrExp_)
			headingLabel_->setText(userName_ + "Data: All Runs");
		else
			headingLabel_->setText(userName_ + "Data: All Experiments");

		switch(organizeMode_) {
		case AMDataViews::OrganizeNone:
		{
			AMDataViewSection* section = new AMDataViewSection(
						userName_ + "Data",
						"Showing all data",
						QString(),
						viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
			connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

			sections_ << section;
			break;
		}

		case AMDataViews::OrganizeRuns:
		{
			// What runs do we have?
			bool found = false;
			QSqlQuery findRunIds = db_->query();
			findRunIds.setForwardOnly(true);
			findRunIds.prepare(QString("SELECT id, dateTime, name FROM AMRun_table"));
			if(findRunIds.exec()) {
				QVector<int> runIds;
				QVector<QString> runNames;
				QVector<QDateTime> runDateTimes;
				while(findRunIds.next()) {
					runIds << findRunIds.value(0).toInt();
					runNames << findRunIds.value(2).toString();
					runDateTimes << findRunIds.value(1).toDateTime();
				}
				findRunIds.finish();	// to avoid long db locks, let's finish this query before doing all the section queries
				for(int i=0, cc=runIds.count(); i<cc; i++) {
					found = true;
					int runId = runIds.at(i);
					QString runName = runNames.at(i);
					QDateTime dateTime = runDateTimes.at(i);
					QString fullRunName = runName + " (" + AMDateTimeUtils::prettyDate(dateTime) + ")";
					AMDataViewSection* section = new AMDataViewSection(
								fullRunName,
								"Showing all data from this run",
								QString("runId = '%1'").arg(runId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No runs found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for runs. The database might be corrupted."));
			}
		}

			break;

		case AMDataViews::OrganizeExperiments:
		{
			// What experiments do we have?
			bool found = false;
			QSqlQuery findExperiments = db_->query();
			findExperiments.setForwardOnly(true);
			findExperiments.prepare(QString("SELECT id, name FROM AMExperiment_table"));
			if(findExperiments.exec()) {
				QVector<int> expIds;
				QVector<QString> expNames;
				while(findExperiments.next()) {
					expIds << findExperiments.value(0).toInt();
					expNames << findExperiments.value(1).toString();
				}
				findExperiments.finish();	// to avoid long db locks, let's finish this query before doing all the section queries
				for(int i=0, cc=expIds.count(); i<cc; i++) {
					found = true;
					int expId = expIds.at(i);
					QString expName = expNames.at(i);

					AMDataViewSection* section = new AMDataViewSection(
								expName,
								"Showing all data from this experiment",
								QString("id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1')").arg(expId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No experiments found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for experiments. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeScanTypes:
		{
			// What scan types do we have?
			bool found = false;
			QSqlQuery findTypes = db_->query();
			findTypes.setForwardOnly(true);
			findTypes.prepare(QString("SELECT AMDbObjectType, description FROM AMDbObjectTypes_table WHERE AMDbObjectType IN (SELECT AMDbObjectType FROM AMScan_table)"));
			if(findTypes.exec()) {
				QVector<QString> classNames;
				QVector<QString> typeDescriptions;
				while(findTypes.next()) {
					classNames << findTypes.value(0).toString();
					typeDescriptions << findTypes.value(1).toString();
				}
				findTypes.finish();
				for(int i=0, cc=classNames.count(); i<cc; i++) {
					found = true;
					QString className = classNames.at(i);
					QString typeDescription = typeDescriptions.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								"Showing all " + typeDescription,
								QString("AMDbObjectType = '%1'").arg(className),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No data types found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for scan types. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeSamples:
		{
			// What samples do we have?
			bool found = false;
			QSqlQuery findSamples = db_->query();
			findSamples.setForwardOnly(true);
			findSamples.prepare("SELECT id, dateTime, name FROM AMSamplePre2013_table");	/// \todo thumbnail for samples
			if(findSamples.exec()) {
				QVector<int> sampleIds;
				QVector<QDateTime> sampleDateTimes;
				QVector<QString> sampleNames;
				while(findSamples.next()) {
					sampleIds << findSamples.value(0).toInt();
					sampleDateTimes << findSamples.value(1).toDateTime();
					sampleNames << findSamples.value(2).toString();
				}
				findSamples.finish();
				for(int i=0, cc=sampleIds.count(); i<cc; i++) {
					found = true;
					int sampleId = sampleIds.at(i);
					QDateTime dt = sampleDateTimes.at(i);
					QString name = sampleNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								name,
								"Sample created " + AMDateTimeUtils::prettyDateTime(dt),
								QString("sampleId = '%1'").arg(sampleId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No samples found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for samples. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeElements:
		{
			// What elements do we have?
			bool found = false;
			QSqlQuery findElements = db_->query();
			findElements.setForwardOnly(true);
			findElements.prepare("SELECT id, symbol, name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries)");
			if(findElements.exec()) {
				QVector<int> elementIds;
				QVector<QString> elementSymbols;
				QVector<QString> elementNames;
				while(findElements.next()) {
					elementIds << findElements.value(0).toInt();
					elementSymbols << findElements.value(1).toString();
					elementNames << findElements.value(2).toString();
				}
				findElements.finish();
				for(int i=0, cc=elementIds.count(); i<cc; i++) {
					found = true;
					int elementId = elementIds.at(i);
					QString symbol = elementSymbols.at(i);
					QString name = elementNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1").arg(name),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1')").arg(elementId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No elements found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for elements in samples. The database might be corrupted."));
			}
		}
			break;

		}
	} // end of: showing all runs or all experiments




	// showing a specific run:
	if(runOrExp_ && runId_ >= 0) {

		// get run name:
		QString runName, fullRunName;
		QDateTime runTime, runEndTime;
		QSqlQuery runInfo = db_->query();
		runInfo.prepare("SELECT name, dateTime FROM AMRun_table where id = ?");
		runInfo.bindValue(0, runId_);
		if(runInfo.exec() && runInfo.next()) {
			runName = runInfo.value(0).toString();
			runTime = runInfo.value(1).toDateTime();
			fullRunName = runName + " (" + AMDateTimeUtils::prettyDate(runTime) + ")";
			headingLabel_->setText(userName_ + "Runs: " + fullRunName);
		}
		else
			headingLabel_->setText(userName_ + "Data");
		runInfo.finish();

		switch(organizeMode_) {
		case AMDataViews::OrganizeRuns:
		{
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "This view is showing a single run, but the organize mode is set to organize by runs. This doesn't make sense and should never happen. Handling as OrganizeNone."));
		case AMDataViews::OrganizeNone:
				AMDataViewSection* section = new AMDataViewSection(
							fullRunName,
							"Showing all data from this run",
							QString("runId = '%1'").arg(runId_),
							viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
				connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

				sections_ << section;
				break;
			}


		case AMDataViews::OrganizeExperiments:
		{
			// What experiments do we have?
			bool found = false;
			QSqlQuery findExperiments = db_->query();
			findExperiments.setForwardOnly(true);
			findExperiments.prepare(QString("SELECT id, name FROM AMExperiment_table WHERE id IN (SELECT experimentId FROM ObjectExperimentEntries WHERE objectId IN (SELECT id FROM AMScan_table WHERE runId = '%1'))").arg(runId_));
			if(findExperiments.exec()) {
				QVector<int> expIds;
				QVector<QString> expNames;
				while(findExperiments.next()) {
					expIds << findExperiments.value(0).toInt();
					expNames << findExperiments.value(1).toString();
				}
				findExperiments.finish();	// to avoid long db locks, let's finish this query before doing all the section queries
				for(int i=0, cc=expIds.count(); i<cc; i++) {
					found = true;
					int expId = expIds.at(i);
					QString expName = expNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								expName,
								QString("Showing all data from this experiment in the <i>%1</i> run").arg(fullRunName),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(runId_).arg(expId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No experiments found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for experiments. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeScanTypes:
		{
			// What scan types do we have?
			bool found = false;
			QSqlQuery findTypes = db_->query();
			findTypes.setForwardOnly(true);
			findTypes.prepare(QString("SELECT AMDbObjectType, description FROM AMDbObjectTypes_table WHERE AMDbObjectType IN (SELECT DISTINCT AMDbObjectType FROM AMScan_table WHERE runId = ?)"));
			findTypes.bindValue(0, runId_);
			if(findTypes.exec()) {
				QVector<QString> classNames;
				QVector<QString> typeDescriptions;
				while(findTypes.next()) {
					classNames << findTypes.value(0).toString();
					typeDescriptions << findTypes.value(1).toString();
				}
				findTypes.finish();
				for(int i=0, cc=classNames.count(); i<cc; i++) {
					found = true;
					QString className = classNames.at(i);
					QString typeDescription = typeDescriptions.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> run").arg(fullRunName),
								QString("AMDbObjectType = '%1' AND runId = '%2'").arg(className).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No data types found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for scan types. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeSamples:
		{
			// what samples do we have?
			bool found = false;
			QSqlQuery findSamples = db_->query();
			findSamples.setForwardOnly(true);
			findSamples.prepare("SELECT id, dateTime, name FROM AMSamplePre2013_table WHERE id IN (SELECT sampleId FROM AMScan_table WHERE runId = ?)");	/// \todo add thumbnail icon!
			findSamples.bindValue(0, runId_);
			if(findSamples.exec()) {
				QVector<int> sampleIds;
				QVector<QDateTime> sampleDateTimes;
				QVector<QString> sampleNames;
				while(findSamples.next()) {
					sampleIds << findSamples.value(0).toInt();
					sampleDateTimes << findSamples.value(1).toDateTime();
					sampleNames << findSamples.value(2).toString();
				}
				findSamples.finish();
				for(int i=0, cc=sampleIds.count(); i<cc; i++) {
					found = true;
					int sampleId = sampleIds.at(i);
					QDateTime dt = sampleDateTimes.at(i);
					QString name = sampleNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created %1.  Showing all data from this sample in the <i>%2</i> run").arg(AMDateTimeUtils::prettyDateTime(dt)).arg(fullRunName),
								QString("sampleId = '%1' AND runId = '%2'").arg(sampleId).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No samples found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for samples. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeElements:
		{
			// What elements do we have?
			bool found = false;
			QSqlQuery findElements = db_->query();
			findElements.setForwardOnly(true);
			findElements.prepare("SELECT id, symbol, name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries WHERE sampleId IN (SELECT sampleId FROM AMScan_table WHERE runId = ?))");
			findElements.bindValue(0, runId_);
			if(findElements.exec()) {
				QVector<int> elementIds;
				QVector<QString> elementSymbols;
				QVector<QString> elementNames;
				while(findElements.next()) {
					elementIds << findElements.value(0).toInt();
					elementSymbols << findElements.value(1).toString();
					elementNames << findElements.value(2).toString();
				}
				findElements.finish();
				for(int i=0, cc=elementIds.count(); i<cc; i++) {
					found = true;
					int elementId = elementIds.at(i);
					QString symbol = elementSymbols.at(i);
					QString name = elementNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1 in the <i>%2</i> run").arg(name).arg(fullRunName),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1') AND runId = '%2'").arg(elementId).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No elements found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for elements in samples. The database might be corrupted."));
			}
		}
			break;

		}
	} // end of showing a specific run





	// Showing a specific experiment
	if(!runOrExp_ && experimentId_ >= 0) {

		// get experiment name:
		QString expName;
		QSqlQuery expInfo = db_->query();
		expInfo.prepare("SELECT name FROM AMExperiment_table where id = ?");
		expInfo.bindValue(0, experimentId_);
		if(expInfo.exec() && expInfo.next()) {
			expName = expInfo.value(0).toString();
			headingLabel_->setText(userName_ + "Experiments: " + expName);
		}
		else
			headingLabel_->setText(userName_ + "Data");

		expInfo.finish();

		switch(organizeMode_) {
		case AMDataViews::OrganizeExperiments:
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "This view is showing a single experiment, but the organize mode is set to organize by experiments. This doesn't make sense and should never happen. Handling as OrganizeNone."));
		case AMDataViews::OrganizeNone:
		{
			AMDataViewSection* section = new AMDataViewSection(
						expName,
						"Showing all data from this experiment",
						QString("id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1')").arg(experimentId_),
						viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
			connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

			sections_ << section;
		}
			break;

		case AMDataViews::OrganizeRuns:
		{
			// What runs do we have?
			bool found = false;
			QSqlQuery findRuns = db_->query();
			findRuns.setForwardOnly(true);
			findRuns.prepare(QString("SELECT id, name, dateTime FROM AMRun_table WHERE id IN (SELECT runId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1'))").arg(experimentId_));
			if(findRuns.exec()) {
				QVector<int> runIds;
				QVector<QString> runNames;
				QVector<QDateTime> runDateTimes;
				while(findRuns.next()) {
					runIds << findRuns.value(0).toInt();
					runNames << findRuns.value(1).toString();
					runDateTimes << findRuns.value(2).toDateTime();
				}
				findRuns.finish();	// to avoid long db locks, let's finish this query before doing all the section queries
				for(int i=0, cc=runIds.count(); i<cc; i++) {
					found = true;
					int runId = runIds.at(i);
					QString runName = runNames.at(i);
					QDateTime runTime = runDateTimes.at(i);
					QString fullRunName = runName + " (" + AMDateTimeUtils::prettyDate(runTime) + ")";
					AMDataViewSection* section = new AMDataViewSection(
								fullRunName,
								QString("Showing all data from this run in the <i>%1</i> experiment").arg(expName),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(runId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No runs found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for runs. The database might be corrupted."));
			}
		}
			break;


		case AMDataViews::OrganizeScanTypes:
		{
			// What scan types do we have?
			bool found = false;
			QSqlQuery findTypes = db_->query();
			findTypes.setForwardOnly(true);
			findTypes.prepare(QString("SELECT AMDbObjectTypes_table.AMDbObjectType, AMDbObjectTypes_table.description, AMScan_table.AMDbObjectType, AMScan_table.id, ObjectExperimentEntries.objectId, ObjectExperimentEntries.experimentId "
									  "FROM AMDbObjectTypes_table, AMScan_table, ObjectExperimentEntries "
									  "WHERE ObjectExperimentEntries.experimentId = ? AND ObjectExperimentEntries.objectId = AMScan_table.id AND AMDbObjectTypes_table.AMDbObjectType = AMScan_table.AMDbObjectType"));
			findTypes.bindValue(0, experimentId_);
			if(findTypes.exec()) {
				QVector<QString> classNames;
				QVector<QString> typeDescriptions;
				while(findTypes.next()) {
					classNames << findTypes.value(0).toString();
					typeDescriptions << findTypes.value(1).toString();
				}
				findTypes.finish();
				for(int i=0, cc=classNames.count(); i<cc; i++) {
					found = true;
					QString className = classNames.at(i);
					QString typeDescription = typeDescriptions.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> experiment").arg(expName),
								QString("AMDbObjectType = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(className).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No data types found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for scan types. The database might be corrupted."));
			}

		}
			break;

		case AMDataViews::OrganizeSamples:
		{
			// what samples do we have?
			bool found = false;
			QSqlQuery findSamples = db_->query();
			findSamples.setForwardOnly(true);
			findSamples.prepare("SELECT id,dateTime,name FROM AMSamplePre2013_table WHERE id IN (SELECT sampleId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?))");	/// \todo add thumbnail icon!
			findSamples.bindValue(0, experimentId_);
			if(findSamples.exec()) {
				QVector<int> sampleIds;
				QVector<QDateTime> sampleDateTimes;
				QVector<QString> sampleNames;
				while(findSamples.next()) {
					sampleIds << findSamples.value(0).toInt();
					sampleDateTimes << findSamples.value(1).toDateTime();
					sampleNames << findSamples.value(2).toString();
				}
				findSamples.finish();
				for(int i=0, cc=sampleIds.count(); i<cc; i++) {
					found = true;
					int sampleId = sampleIds.at(i);
					QDateTime dt = sampleDateTimes.at(i);
					QString name = sampleNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created %1.  Showing all data from this sample in the <i>%2</i> experiment").arg(AMDateTimeUtils::prettyDateTime(dt)).arg(expName),
								QString("sampleId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(sampleId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No samples found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for samples. The database might be corrupted."));
			}
		}
			break;

		case AMDataViews::OrganizeElements:
		{
			// What elements do we have?
			bool found = false;
			QSqlQuery findElements = db_->query();
			findElements.setForwardOnly(true);
			findElements.prepare("SELECT id,symbol,name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries WHERE sampleId IN (SELECT sampleId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?)))");
			findElements.bindValue(0, experimentId_);
			if(findElements.exec()) {
				QVector<int> elementIds;
				QVector<QString> elementSymbols;
				QVector<QString> elementNames;
				while(findElements.next()) {
					elementIds << findElements.value(0).toInt();
					elementSymbols << findElements.value(1).toString();
					elementNames << findElements.value(2).toString();
				}
				findElements.finish();
				for(int i=0, cc=elementIds.count(); i<cc; i++) {
					found = true;
					int elementId = elementIds.at(i);
					QString symbol = elementSymbols.at(i);
					QString name = elementNames.at(i);
					AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1 in the <i>%2</i> experiment").arg(name).arg(expName),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1') AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(elementId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), itemSize_);
					connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));

					sections_ << section;
				}
				if(!found)
					sections_ << new AMDataViewEmptyHeader("No elements found.", effectiveWidth());
			}
			else {
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for elements in samples. The database might be corrupted."));
			}
		}
			break;

		}
	} // end of showing just one run




	// and show all the sections in the qgraphicsview layout area:
	foreach(AMAbstractDataViewSection* s, sections_) {
		sectionLayout_->addItem(s);
		sectionLayout_->setAlignment(s, Qt::AlignTop | Qt::AlignLeft);

		connect(s, SIGNAL(selectionChanged()), this, SLOT(onScanItemsSelectionChanged()));
	}

	// finally, set the visualized area of the scene to be exactly as big as we need it...
	// removed: different path of resize signalling now. adjustViewScrollHeight();

}

void AMDataView::adjustViewScrollableArea() {
	gview_->setSceneRect( gwidget_->rect() );
}

void AMDataView::onScanSearchEditScanRequested(const QList<QUrl> &scanUrls)
{
	emit editScansFromDb(scanUrls);
}

void AMDataView::onScanSearchLaunchScanConfigurationRequested(const QList<QUrl> &scanUrls)
{
	emit launchScanConfigurationsFromDb(scanUrls);
}


void AMDataView::onScanSearchExportScanRequested(const QList<QUrl> &scanUrls)
{
	emit exportScansFromDb(scanUrls);
}

void AMDataView::refreshOrganizeModeBox() {

	// install the correct organizeMode options in the comboBox.

	// The full choices are { OrganizeNone, OrganizeRuns, OrganizeExperiments, OrganizeScanTypes, OrganizeSamples, OrganizeElements }.

	// However, OrganizeRuns makes no sense when showing a single run, and organizeExperiments makes no sense when showing a single experiment.
	organizeModeBox_->blockSignals(true);

	organizeModeBox_->clear();

	organizeModeBox_->addItem("Chronological", AMDataViews::OrganizeNone);
	if(!(runOrExp_ && runId_ >= 0))
		organizeModeBox_->addItem("by Run", AMDataViews::OrganizeRuns);
	if(!(!runOrExp_ && experimentId_ >= 0))
		organizeModeBox_->addItem("by Experiment", AMDataViews::OrganizeExperiments);
	organizeModeBox_->addItem("by Scan Type", AMDataViews::OrganizeScanTypes);
	organizeModeBox_->addItem("by Sample", AMDataViews::OrganizeSamples);
	organizeModeBox_->addItem("by Element", AMDataViews::OrganizeElements);

	organizeModeBox_->setCurrentIndex(organizeModeBox_->findData(organizeMode_));

	organizeModeBox_->blockSignals(false);

}

#include <QResizeEvent>
#include <QTimer>

/// Overidden so that we can notify the contents of the scroll area to change width with us.
void AMDataView::resizeEvent(QResizeEvent *event) {

	Q_UNUSED(event);

	QWidget::resizeEvent(event);

	onResize();

}

void AMDataView::onResize() {
	int width = int(effectiveWidth());	// how much room we have available in the view area.

	// set the width constraint on every section in the main vertical layout.
	foreach(AMAbstractDataViewSection* s, sections_) {
		s->setWidthConstraint(width);
	}

	// These will invalidate the main layout, posting a LayoutEvent to the gwidget_. We'll handle the rest inside gwidget_'s event().
}

void AMDataView::setItemSize(int newItemSize)
{
	itemSize_ = newItemSize;
	foreach(AMAbstractDataViewSection* s, sections_) {
		s->setItemSize(newItemSize);
	}
}

void AMDataView::expandAll()
{
	foreach(AMAbstractDataViewSection* s, sections_) {
		s->expand();
	}
}

void AMDataView::collapseAll()
{
	foreach(AMAbstractDataViewSection* s, sections_) {
		s->collapse();
	}
}



AMDataViewSection::AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded, QGraphicsItem* parent, double initialWidthConstraint, int initialItemSize) : AMAbstractDataViewSection(parent) {

	proxiedWidget_ = new QFrame();
	setupUi(proxiedWidget_);
	titleLabel_->setStyleSheet("color: white;\nfont: " AM_FONT_XLARGE_ "pt \"Lucida Grande\";");
	subtitleLabel_->setStyleSheet("color: rgb(204, 204, 204);\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\";");
	proxyWidget_ = new QGraphicsProxyWidget(this);
	proxyWidget_->setWidget(proxiedWidget_);
	proxyWidget_->setZValue(30000);

	subview_ = 0;

	title_ = title;
	subtitle_ = subtitle;
	titleLabel_->setText(title);
	subtitleLabel_->setText(subtitle);

	whereClause_ = whereClause;
	viewMode_ = viewMode;
	db_ = db;
	widthConstraint_ = initialWidthConstraint;
	itemSize_ = initialItemSize;

	expanded_ = !expanded;
	expand(expanded);


	connect(expandButton_, SIGNAL(clicked(bool)), this, SLOT(expand(bool)));

	subtitleLabel_->setText(subtitle_ + QString("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>(%1 scans)</i>").arg(countResults()));
}


void AMDataViewSection::expand(bool expanded) {

	// be efficient. don't do if already done done.
	if(expanded_ == expanded)
		return;

	if( (expanded_ = expanded) ) {

		expandButton_->setArrowType(Qt::DownArrow);
		expandButton_->setChecked(true);

		switch(viewMode_) {
		case AMDataViews::ListView:
			subview_ = new AMDataViewSectionListView(db_, AMDbObjectSupport::s()->tableNameForClass<AMScan>(), whereClause_, this, widthConstraint_, itemSize_);
			break;

		case AMDataViews::ThumbnailView:
		default:
			subview_ = new AMDataViewSectionThumbnailView(db_, AMDbObjectSupport::s()->tableNameForClass<AMScan>(), whereClause_, this, widthConstraint_, itemSize_);
			break;
		}
		connect(subview_, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
		updateGeometry();
	}
	else {
		expandButton_->setArrowType(Qt::RightArrow);
		expandButton_->setChecked(false);
		delete subview_;
		subview_ = 0;
		updateGeometry();
	}
}


QSizeF AMDataViewSection::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {

	QSizeF actualConstraint = constraint;

	if(actualConstraint.width() < 0)
		actualConstraint.setWidth(widthConstraint_);

	QSizeF headerSize = proxyWidget_->effectiveSizeHint(which, actualConstraint);

	if(!expanded_) {
		return headerSize + QSizeF(0, 10);
	}
	else {
		QSizeF subviewSize = subview_->effectiveSizeHint(which, actualConstraint);
		return QSizeF(qMax(headerSize.width(), subviewSize.width()), headerSize.height() + subviewSize.height());
	}
}

void AMDataViewSection::setGeometry(const QRectF &rect) {

	// qdebug() << "calling section setGeometry with rect" << rect;

	QGraphicsWidget::setGeometry(rect);

	layoutHeaderItem();
	if(expanded_)
		layoutContents();
}

void AMDataViewSection::layoutHeaderItem() {

	/// \todo Optimization: if this is being called from above on all, get rid of calling proxyWidget_->effectiveSizeHint() on each scroll.
	QRectF entireRect = rect();
	// qdebug() << "Calling Layout Header. entireRect = " << entireRect;

	qreal headerHeight = qMin(entireRect.height(),
							  proxyWidget_->effectiveSizeHint(Qt::MinimumSize, QSizeF(entireRect.width(), -1)).height());

	qreal yOffset = 0;
	///////////////
	QGraphicsScene* gscene = scene();
	if(expanded_ && gscene && gscene->views().count() == 1) {

		QGraphicsView* gview = gscene->views().at(0);

		// convert entireRect to viewport coordinates. (Note: assuming no rotation of the scene in the view)
		QRectF entireRect_scene = mapRectToScene(entireRect);
		QPoint entireRect_topLeft_viewport = gview->mapFromScene(entireRect_scene.topLeft());
		QPoint entireRect_bottomLeft_viewport = gview->mapFromScene(entireRect_scene.topLeft()+QPointF(0, entireRect_scene.height()));

		// if this section extends both above and below the top edge of the view... We want to place the header item at the top edge of the view.
		if(entireRect_topLeft_viewport.y() < 0 && entireRect_bottomLeft_viewport.y() >0) {

			QPointF viewTopLeft_scene = gview->mapToScene(QPoint(0,0));
			QPointF viewTopLeft_local = mapFromScene(viewTopLeft_scene);

			yOffset = viewTopLeft_local.y();
			qreal yOffsetMax = entireRect.height() - headerHeight;
			if(yOffset > yOffsetMax)
				yOffset = yOffsetMax;
		}
	}

	///////////////

	QRectF headerRect = QRectF(QPointF(entireRect.topLeft())+QPointF(0, yOffset),
							   QSizeF(entireRect.width(), headerHeight));
	// qdebug() << "Calling proxyWidget setGeometry with rect" << headerRect;
	proxyWidget_->setGeometry(headerRect);
}

void AMDataViewSection::layoutContents() {
	if(!expanded_)
		return;

	qreal verticalOffset = proxyWidget_->rect().height();
	QSizeF size = rect().size() - QSizeF(0, verticalOffset);
	// qdebug() << "calling contents setGeometry with rect" << QRectF(QPointF(0,verticalOffset), size);
	subview_->setGeometry(QRectF(QPointF(0,verticalOffset), size));
}

int AMDataViewSection::countResults() {
	QSqlQuery q = db_->query();
	QString query = QString("SELECT COUNT(1) FROM %1").arg(AMDbObjectSupport::s()->tableNameForClass<AMScan>());
	if(!whereClause_.isEmpty())
		query.append(" WHERE ").append(whereClause_);
	q.prepare(query);
	if(q.exec() && q.first()) {
		return q.value(0).toInt();
	}

	return 0;
}

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include "ui/dataman/AMFlowGraphicsLayout.h"

AMDataViewSectionThumbnailView::AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent, double initialWidthConstraint, int initialItemSize)
	: AMAbstractDataViewSection(parent) {

	QTime t;
	t.start();

	db_ = db;
	dbTableName_ = dbTableName;
	whereClause_ = whereClause;

	layout_ = new AMFlowGraphicsLayout();
	layout_->setSpacing(Qt::Vertical, 30);
	layout_->setSpacing(Qt::Horizontal, 30);
	layout_->setUniformItemSizes(true);

	layout_->setDefaultWidth(initialWidthConstraint);
	int initialItemWidth = int((double)initialItemSize/50.0*160.0 + 80);
	layout_->setItemSizeConstraint(QSizeF(initialItemWidth, -1));

	populate();

	setLayout(layout_);
}



#include "ui/AMThumbnailScrollViewer.h"

//#define AMDATAVIEWSECTIONTHUMBNAILVIEW_PROCESS_EVENTS_EVERY_N_ITEMS 50

void AMDataViewSectionThumbnailView::populate() {

	setVisible(false);

	// qdebug() << "AMDataViewSectionThumbnailView::populate(): \n   starting at " << QTime::currentTime().toString("mm:ss.zzz");

	/// \todo This won't work for samples, because they don't have a number column. Generalize or specificized.
	QSqlQuery q = db_->query();
	QString query = QString("SELECT thumbnailFirstId,thumbnailCount,name,number,dateTime,id FROM %1").arg(dbTableName_);
	if(!whereClause_.isEmpty())
		query.append(" WHERE ").append(whereClause_);
	query.append(" ORDER BY dateTime");
	q.prepare( query );

	//qdebug() << "   prior to executing database query: " << QTime::currentTime().toString("mm:ss.zzz");

	if(!q.exec()) {
		q.finish();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, QString("Error executing database query '%1'. The error was %2").arg(q.executedQuery()).arg(q.lastError().text())));
	}

	// qdebug() << "   after executing database query: " << QTime::currentTime().toString("mm:ss.zzz");

	// int processEventsBreakCounter = 0;

	else {
		while(q.next()) {

			AMThumbnailScrollGraphicsWidget* w = new AMThumbnailScrollGraphicsWidget(this);
			w->setSource(db_, dbTableName_, q.value(5).toInt(), q.value(0).toInt(), q.value(1).toInt());
			QString caption1 = q.value(2).toString();
			if(q.value(3).toInt() != 0)
				caption1.append(QString(" #%1").arg(q.value(3).toInt()));
			w->setCaption1(caption1);
			w->setCaption2(AMDateTimeUtils::prettyDateTime(q.value(4).toDateTime()));

			layout_->addItem(w);

			/*if(processEventsBreakCounter++ == AMDATAVIEWSECTIONTHUMBNAILVIEW_PROCESS_EVENTS_EVERY_N_ITEMS) {
   processEventsBreakCounter = 0;
   QApplication::processEvents();
  }*/

			/*
  AMThumbnailScrollViewer* w = new AMThumbnailScrollViewer();
  w->setSource(db_, q.value(0).toInt(), q.value(1).toInt());
  QGraphicsProxyWidget* p = new QGraphicsProxyWidget(gWidget_);
  w->setMinimumSize(240, 180);
  p->setWidget(w);
  layout_->addItem(p);
  */

		}
	}

	// qdebug() << "   ending at " << QTime::currentTime().toString("mm:ss.zzz") << "\n";
	// layout_->activate();

	setVisible(true);
}

 AMLayoutControlledGraphicsWidget::~AMLayoutControlledGraphicsWidget(){}
AMLayoutControlledGraphicsWidget::AMLayoutControlledGraphicsWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QGraphicsWidget(parent, wFlags)
{
}

bool AMLayoutControlledGraphicsWidget::event(QEvent *event)
{
	if(event->type() == QEvent::LayoutRequest) {
		if(layout()) {
			QSizeF layoutSizeHint = layout()->effectiveSizeHint(Qt::PreferredSize);
			if(size() != layoutSizeHint) {
				//				qdebug() << "AMLayoutControlledGraphicsWidget: Catching a LayoutRequest and resizing ourself to match instead.";
				resize(layoutSizeHint);
				return true;	// don't pass this on like usual. Our resize() will trigger another LayoutRequest, but by then the size will match, so it'll be passed on through to the layout (below).
			}
		}
	}

	return QGraphicsWidget::event(event);
}

void AMLayoutControlledGraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
	//	qdebug() << "AMLayoutControlledGraphicsWidget: Getting resized, and emitting resized signal.";
	QGraphicsWidget::resizeEvent(event);
	emit resized();
}

#include <QAbstractItemModel>
#include "dataman/AMSamplePre2013.h"
#include "dataman/AMSample.h"
#include "dataman/AMRun.h"

AMDataViewSectionListView::AMDataViewSectionListView(AMDatabase *db, const QString &dbTableName, const QString &whereClause, QGraphicsItem *parent, double initialWidthConstraint, int initialItemSize)
	: AMAbstractDataViewSection(parent)
{
	Q_UNUSED(initialItemSize)
	setVisible(false);

	db_ = db;
	tableName_ = dbTableName;
	whereClause_ = whereClause;

	tableView_ = new AMIgnoreScrollTableView();
	tableView_->setAlternatingRowColors(true);
	tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tableView_->setDragEnabled(true);
	QFont font = tableView_->font();
	font.setPointSize(font.pointSize()-1);
	tableView_->setFont(font);

	tableView_->verticalHeader()->hide();
	QFontMetrics fm(font);
	tableView_->verticalHeader()->setDefaultSectionSize(fm.height()+6);
	tableView_->horizontalHeader()->setStretchLastSection(true);
	// ugly but practical: (doesn't line up between sections): tableView_->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableView_->setSortingEnabled(true);


	tableModel_ = new AMQueryTableModel(db_,
									   this,
									   tableName_,
									   "id",
									   whereClause,
									   "dateTime ASC",
									   QList<AMQueryTableModelColumnInfo>()
										   << AMQueryTableModelColumnInfo("Row", "id")
										   << AMQueryTableModelColumnInfo("Name", "name")
										   << AMQueryTableModelColumnInfo("#", "number")
										   << AMQueryTableModelColumnInfo("When", "dateTime")
										   // << AMQueryTableModelColumnInfo("About", "scanInfo")
										   // << AMQueryTableModelColumnInfo("Sample", "sampleId", true, AMDbObjectSupport::s()->tableNameForClass<AMSamplePre2013>(), "name")
										   << AMQueryTableModelColumnInfo("Technique", "AMDbObjectType", true, "AMDbObjectTypes_table", "description", "AMDbObjectType")
										   // << AMQueryTableModelColumnInfo("Where", "facilityId", true, AMDbObjectSupport::s()->tableNameForClass<AMFacility>(), "description")
										   << AMQueryTableModelColumnInfo("Where", "runId", true, AMDbObjectSupport::s()->tableNameForClass<AMRun>(), "name")
										   << AMQueryTableModelColumnInfo("Notes", "notes")
									   );
	tableModel_->refreshQuery();
	tableView_->setModel(tableModel_);

	widthConstraint_ = initialWidthConstraint;

	// For now, we know what's in the columns, so we'll resize the headers appropriately. In the future, it's possible that the columns are configurable, using AMScanQueryModel with non-default constructor.
	tableView_->setColumnHidden(0, true);
	tableView_->setColumnWidth(1, 120);
	tableView_->setColumnWidth(2, 40);
	tableView_->setColumnWidth(3, 180);
	tableView_->setColumnWidth(4, 120);
	tableView_->setColumnWidth(5, 120);
	tableView_->setColumnWidth(6, 120);

	tableView_->sortByColumn(3, Qt::AscendingOrder);

	proxyWidget_ = new QGraphicsProxyWidget(this);
	proxyWidget_->setWidget(tableView_);

	layout_ = new QGraphicsLinearLayout(Qt::Vertical);
	layout_->setContentsMargins(0,0,0,0);
	layout_->addItem(proxyWidget_);
	setLayout(layout_);

	connect(tableModel_, SIGNAL(rowsInserted (QModelIndex, int, int)), this, SLOT(onRowsAddedOrRemoved()));
	connect(tableModel_, SIGNAL(rowsRemoved (QModelIndex, int, int)), this, SLOT(onRowsAddedOrRemoved()));
	// tableView_->installEventFilter(this);

	setVisible(true);

	connect(tableView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(selectionChanged()));
}

void AMDataViewSectionListView::setWidthConstraint(double widthConstraint)
{
	if(widthConstraint_ == widthConstraint)
		return;

	widthConstraint_ = widthConstraint;
	updateGeometry();// note: this doesn't propagate all the way up, but we don't need it to.
}



QSizeF AMDataViewSectionListView::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
	Q_UNUSED(which)
	Q_UNUSED(constraint)

	return QSizeF(widthConstraint_, (tableModel_->rowCount()+1)*tableView_->verticalHeader()->defaultSectionSize() + tableView_->horizontalHeader()->height());
}

void AMDataViewSectionListView::onRowsAddedOrRemoved()
{

	// must call because our sizeHint has changed.
	updateGeometry();
	// problem: this doesn't propagate all the way up, because we're not in a layout. Do this as well:
	AMAbstractDataViewSection* parent = qobject_cast<AMAbstractDataViewSection*>(parentWidget());
	if(parent)
		parent->notifyThatChildSizeHintChanged();

}

QList<QUrl> AMDataViewSectionListView::selectedItems() const
{
	QList<QUrl> rv;
	QString prefix = "amd://" % db_->connectionName() % "/";

	QModelIndexList selected = tableView_->selectionModel()->selectedRows();
	foreach(const QModelIndex& index, selected) {
		rv << QUrl(prefix % tableName_ % "/" % index.data().toString());
	}

	return rv;
}



bool AMIgnoreScrollTableView::event(QEvent *event)
{
	if(event->type() == QEvent::Wheel) {
		event->ignore();
		return false;
	}
	if(event->type() == QEvent::KeyPress) {
		int key = ((QKeyEvent*)event)->key();
		if(key == Qt::Key_PageDown || key == Qt::Key_PageUp) {
			event->ignore();
			return false;
		}
	}
	return QTableView::event(event);
}

int AMDataViewSectionListView::numberOfSelectedItems() const
{
	return tableView_->selectionModel()->selectedRows().count();
}


void AMDataView::afterDatabaseItemChanged()
{
	if(viewRequiresRefresh_) {
		refreshView();
	}
}


void AMDataView::onDatabaseItemCreatedOrRemoved(const QString &tableName, int id)
{
	Q_UNUSED(id)

	if(tableName != scansTableName_)
		return;

	viewRequiresRefresh_ = true;
	if(isVisible()) {
		afterDbChangedFnCall_.runLater(5000);
	}
}

void AMDataView::onDragStarted(const QPoint &startPos, const QPoint &currentPos)
{
	// qdebug() << "Drag started...";

	// we use this to indicate if we're tracking a rubber-band selection drag.
	if(rubberBand_) {
		delete rubberBand_;
		rubberBand_ = 0;
	}

	// convert to scene pos:
	QPointF scenePos = gview_->mapToScene(startPos);

	// Possibility 1: is there a selected item under us? Then we should start a drag with all the selected items
	QGraphicsItem* item = gscene_->itemAt(scenePos);
	if(item && item->isSelected()) {
		// start a QDrag with ALL the selected items. Do we have a pixmap to use to represent the drag? We do if we're in thumbnail mode and there's an AMThumbnailScrollGraphicsWidget under us.
		AMThumbnailScrollGraphicsWidget* gw = qgraphicsitem_cast<AMThumbnailScrollGraphicsWidget*>(item);
		if(gw)
			startDragWithSelectedItems(gw->pixmap().scaledToHeight(100, Qt::SmoothTransformation));
		else
			startDragWithSelectedItems();
	}

	// Possibility 2: Else, start drawing a rubber band to select some items.
	else {
		QRectF selectionRect  = QRectF(scenePos, gview_->mapToScene(currentPos)).normalized();

		rubberBand_ = gscene_->addRect(selectionRect,
									   QPen(QColor(167, 167, 167)),
									   QBrush(QColor(167,167,167,80)));

		// Clear the selection area, unless the command (mac)/control key or shift key is pressed.
		Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
		if(!((mods&Qt::ShiftModifier) || (mods&Qt::ControlModifier)))
			selectionArea_ = QPainterPath();

		QPainterPath selectionArea = selectionArea_;
		selectionArea.addRect(selectionRect);
		gscene_->setSelectionArea(selectionArea);
	}
}

void AMDataView::onDragMoved(const QPoint &startPos, const QPoint &currentPos)
{
	if(rubberBand_) {
		QRectF selectionRect = QRectF(gview_->mapToScene(startPos), gview_->mapToScene(currentPos)).normalized();
		rubberBand_->setRect(selectionRect);

		QPainterPath selectionArea = selectionArea_;
		selectionArea.addRect(selectionRect);
		gscene_->setSelectionArea(selectionArea);
		/// \todo TODOZ: if outside of view...scroll into view
	}
}

void AMDataView::onDragEnded(const QPoint &startPos, const QPoint &endPos)
{
	if(rubberBand_) {
		QRectF selectionRect = QRectF(gview_->mapToScene(startPos), gview_->mapToScene(endPos)).normalized();

		// now that we're done... actually remember this selection area for good (in case future shift-drags and control-drags need to add to it)
		selectionArea_.addRect(selectionRect);
		gscene_->setSelectionArea(selectionArea_);

		delete rubberBand_;
		rubberBand_ = 0;
	}
}

void AMDataView::startDragWithSelectedItems(const QPixmap& optionalPixmap)
{
	QList<QUrl> urls = selectedItems();
	if(urls.count() == 0)
		return;

	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();
	drag->setMimeData(mime);
	mime->setUrls(urls);

	drag->setHotSpot(QPoint(15,15));

	// generate a pixmap to represent this drag. Draw text "n scans" on top of the one we were provided (if we were).
	///////////////////////////////////
	QPixmap pixmap;
	if(optionalPixmap.isNull()) {
		pixmap = QPixmap(100, 30);
		pixmap.fill(Qt::transparent);
	}
	else
		pixmap = optionalPixmap;
	QRectF textRect;
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(Qt::white, 2));
	painter.setBrush(Qt::red);
	painter.drawText(QRectF(10,10,qMax(1,pixmap.width()-20),qMax(1,pixmap.height()-20)),
					 Qt::AlignRight | Qt::AlignBottom,
					 urls.count() == 1 ? QString("1 scan") : QString("%1 scans").arg(urls.count()),
					 &textRect);
	QRectF borderBox = textRect.adjusted(-5,-5,5,5);
	painter.setPen(QPen(QColor(200,200,200), 2));
	painter.setBrush(QColor(200,200,200));
	qreal radius = borderBox.height()/2;
	painter.drawRoundedRect(borderBox.translated(-1,-1), radius, radius);
	painter.drawRoundedRect(borderBox.translated(2,2), radius, radius);
	painter.setPen(QPen(Qt::white, 2));
	painter.setBrush(QColor(100,100,100));
	painter.drawRoundedRect(borderBox, radius, radius);
	painter.drawText(QRectF(10,10,qMax(1,pixmap.width()-20),qMax(1,pixmap.height()-20)),
					 Qt::AlignRight | Qt::AlignBottom,
					 urls.count() == 1 ? QString("1 scan") : QString("%1 scans").arg(urls.count()),
					 &textRect);
	painter.end();
	drag->setPixmap(pixmap);
	//////////////////////////////////

	drag->exec(Qt::CopyAction); // todo: also offer move action if dropping onto runs
}

#include <QMenu>
#include <QAction>
#include <QMessageBox>

void AMDataView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);
	int numSelectedItems = numberOfSelectedItems();

	QAction *temp = popup.addAction("Edit");
	if (numSelectedItems == 0)
		temp->setEnabled(false);

	temp = popup.addAction("Compare");
	if (numSelectedItems == 0)
		temp->setEnabled(false);

	temp = popup.addAction("Export");
	if (numSelectedItems == 0)
		temp->setEnabled(false);

	temp = popup.addAction("");
	switch(numSelectedItems){

	case 0:
		temp->setText("Show Scan Configuration");
		temp->setEnabled(false);
		break;
	case 1:
		temp->setText("Show Scan Configuration");
		break;
	default:
		temp->setText("Show Scan Configurations");
		break;
	}

	temp = popup.addAction("Fix CDF file");
	if (numSelectedItems != 1)
		temp->setEnabled(false);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Edit")
			emit editScansFromDb();

		else if (temp->text() == "Compare")
			emit compareScansFromDb();

		else if (temp->text() == "Export")
			emit exportScansFromDb();

		else if (temp->text() == "Show Scan Configuration" || temp->text() == "Show Scan Configurations"){

			if (numSelectedItems > 5){

				QMessageBox::StandardButton button = QMessageBox::question(this, "Opening multiple scan configurations",
																			"You are about to open more than 5 scan configurations at once.  Are you sure?",
																			QMessageBox::Ok | QMessageBox::Cancel,
																			QMessageBox::Cancel);
				if (button == QMessageBox::Ok)
					emit launchScanConfigurationsFromDb();
			}
			else
				emit launchScanConfigurationsFromDb();
		}

		else if (temp->text() == "Fix CDF file"){

			QMessageBox::StandardButton button = QMessageBox::question(this, "Fixing CDF files",
																		"You are about fix your scan that uses CDF files.  This may take a couple minutes.  Do you wish to proceed?",
																		QMessageBox::Ok | QMessageBox::Cancel,
																		QMessageBox::Cancel);
			if (button == QMessageBox::Ok)
				emit fixCDF();
		}
	}
}

void AMDataView::onViewClicked(const QPoint &clickPos)
{
	// here we manage scene selection in our own special way.  Command-click/control-click adds the clicked item to the selection.  Shift-click selects all items between the current selection and the clicked item. Normal click clears the selection and selects the item underneath.
	Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
	QPointF scenePos = gview_->mapToScene(clickPos);

	QGraphicsItem* item = gscene_->itemAt(scenePos);

	if(mods & Qt::ShiftModifier) {
		// shift-selection: select all items in the rectangle containing the existing selected items and the clicked item.
		if(item && (item->flags() & QGraphicsItem::ItemIsSelectable)) {

			QList<QGraphicsItem*> selectedItems = gscene_->selectedItems();
			bool otherItems = false;

			for(int i=0; i < selectedItems.count(); i++) {

				QGraphicsItem* otherItem = selectedItems.at(i);

				if(otherItem != item)
					otherItems = true;
			}

			if (otherItems){

				QList<QGraphicsItem *> items = gscene_->items();
				int clickedItemIndex = items.indexOf(item);
				int startIndex = 0;
				int endIndex = 0;
				int indexOfSelectedItem = 0;
				int firstSelectedIndex = items.indexOf(selectedItems.first());
				int lastSelectedIndex = items.indexOf(selectedItems.first());

				for (int i = 0; i < selectedItems.size(); i++){

					indexOfSelectedItem = items.indexOf(selectedItems.at(i));

					if (firstSelectedIndex > indexOfSelectedItem)
						firstSelectedIndex = indexOfSelectedItem;

					if (lastSelectedIndex < indexOfSelectedItem)
						lastSelectedIndex = indexOfSelectedItem;
				}

				// If the clicked item is before any selected items.  Highlight all in between.
				if (clickedItemIndex < firstSelectedIndex){

					startIndex = clickedItemIndex;
					endIndex = firstSelectedIndex;
				}

				// If the clicked item is after all other selected items.  Highlight all in between.
				else if (clickedItemIndex > lastSelectedIndex){

					startIndex = lastSelectedIndex;
					endIndex = clickedItemIndex + 1;	// Adding 1 to include the clicked item as well.
				}

				// If the clicked item falls between two selected items, select all of the items between the clicked item and the previous (lower index) item.
				else {

					int tempIndex = 0;

					for (int i = 0; i < selectedItems.size(); i++){

						indexOfSelectedItem = items.indexOf(selectedItems.at(i));

						if ((indexOfSelectedItem < clickedItemIndex) && (tempIndex < indexOfSelectedItem))
							tempIndex = indexOfSelectedItem;
					}

					startIndex = tempIndex;
					endIndex = clickedItemIndex + 1;	// Adding 1 to include the clicked item as well.
				}

				// Select all items from the start to the end.
				for (int i = startIndex; i < endIndex; i++)
					items.at(i)->setSelected(true);
			}

			else {
				// no selected items. Just select this item.
				item->setSelected(true);
			}
		}
	}
	else if(mods & Qt::ControlModifier) {	// command (mac) or control selection: add clicked item to selection
		if(item && (item->flags() & QGraphicsItem::ItemIsSelectable)) {
			// Toggle the selection of this item, but don't clear the selection
			item->setSelected(!item->isSelected());
		}
	}
	else {
		// no mods. clear selection
		gscene_->clearSelection();
		selectionArea_ = QPainterPath();
		// if there's an item here, add it to the selection.
		if(item && (item->flags() & QGraphicsItem::ItemIsSelectable)) {
			item->setSelected(true);
		}
	}
}

#include <QGraphicsSceneMouseEvent>
bool AMDataView::eventFilter(QObject* object, QEvent* event)
{
	// for now, this filter only applies when we are in thumbnail view mode.
	if(object == gscene_ && viewMode_ == AMDataViews::ThumbnailView) {
		if(event->type() == QEvent::GraphicsSceneMouseDoubleClick
				|| event->type() == QEvent::GraphicsSceneMousePress
				|| event->type() == QEvent::GraphicsSceneMouseRelease) {

			// We need to filter the clicks to prevent the scene from messing with our selection system.
				// Exception: However, if this is a click on the header bar widget, we need to allow that to go through (for ex: for collapsing/expanding sections)
			QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
			QGraphicsItem* item = gscene_->itemAt(mouseEvent->buttonDownScenePos(mouseEvent->button()));
			if(qgraphicsitem_cast<QGraphicsProxyWidget*>(item))
				return false;	// need to let the proxy widget receive this event. (This only works while we're using that proxy widget for the header bars in AMDataViewSection).

			// Note: If we end up adding a lot more exceptions here, we might need to take a second look at how this is designed.

			return true;
		}
	}

	return false;
}

void AMDataView::onViewDoubleClicked(const QPoint &clickPos)
{
	// one possible use-case is that multiple items are selected, and the user cmd/control-doubleclicks to open all selected items. However, the first single cmd-click will have deselected the item under the cursor, so we want to re-enable that guy.
	Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();

	if((mods & Qt::ControlModifier)) {
		QPointF scenePos = gview_->mapToScene(clickPos);
		QGraphicsItem* item = gscene_->itemAt(scenePos);
		if(item && (item->flags() & QGraphicsItem::ItemIsSelectable)) {
			item->setSelected(true);
		}
	}
	// Now we let the regular double-click mechanism run its course.
	emit viewDoubleClicked();
}




AMDataViewEmptyHeader::AMDataViewEmptyHeader(const QString &message, double initialWidthConstraint, QGraphicsItem *parent) :
	AMAbstractDataViewSection(parent)
{
	widthConstraint_ = initialWidthConstraint;
	proxiedWidget_ = new QFrame();
	setupUi(proxiedWidget_);
	messageText_->setStyleSheet("color: rgb(25, 25, 25);\nfont: italic " AM_FONT_REGULAR_ "pt \"Lucida Grande\";");
	QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
	proxy->setWidget(proxiedWidget_);
	layout_ = new QGraphicsLinearLayout(Qt::Vertical);
	layout_->setContentsMargins(0,0,0,0);
	setLayout(layout_);
	layout_->addItem(proxy);

	messageText_->setText(message);
}



 AMAbstractDataViewSection::~AMAbstractDataViewSection(){}
 AMIgnoreScrollTableView::~AMIgnoreScrollTableView(){}
