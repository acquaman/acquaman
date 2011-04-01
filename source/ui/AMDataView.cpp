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


#include "AMDataView.h"

#include "dataman/AMDatabase.h"
#include "dataman/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"
#include "ui/AMDateTimeUtils.h"
#include "dataman/AMUser.h"
#include "dataman/AMScan.h"

#include <QScrollBar>
#include <QApplication>

#include <QDebug>



AMDataView::AMDataView(AMDatabase* database, QWidget *parent) :
		QWidget(parent)
{
	db_ = database;
	runId_ = experimentId_ = -1;

	setupUi(this);

	// add additional UI components: the QGraphicsView and QGraphicsScene
	gview_ = this->graphicsView;
	gview_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	gscene_ = new AMSignallingGraphicsScene(this);
	connect(gscene_, SIGNAL(selectionChanged()), this, SLOT(onSceneSelectionChanged()));
	connect(gscene_, SIGNAL(doubleClicked(QPointF)), this, SLOT(onSceneDoubleClicked()));
	gview_->setScene(gscene_);
	gview_->setDragMode(QGraphicsView::RubberBandDrag);
	gwidget_ = new AMLayoutControlledGraphicsWidget();
	gscene_->addItem(gwidget_);
	sectionLayout_ = new QGraphicsLinearLayout(Qt::Vertical);
	sectionLayout_->setContentsMargins(0,0,0,0);
	sectionLayout_->setSpacing(0);
	gwidget_->setLayout(sectionLayout_);
	// this refers to the QObject tree parent, not the QGraphicsItem tree parent
	gwidget_->setParent(this);

	connect(gwidget_, SIGNAL(resized()), this, SLOT(adjustViewScrollableArea()));

	// sectionLayout_->setAlignment();

	retrieveUserName();
	headingLabel_->setText(userName_ + "Data");

	// setup button group and make viewmode buttons checkable:
	viewModeButtonGroup_ = new QButtonGroup(this);
	viewModeButtonGroup_->addButton(viewModeB1_, AMDataViews::ThumbnailView);
	viewModeButtonGroup_->addButton(viewModeB2_, AMDataViews::ListView);
	viewModeButtonGroup_->addButton(viewModeB3_, AMDataViews::FlowView);
	viewModeButtonGroup_->addButton(viewModeB4_, AMDataViews::DetailView);
	connect(viewModeButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(setViewMode(int)));

	connect(organizeModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onOrganizeModeBoxCurrentIndexChanged(int)));

	connect(sizeSlider, SIGNAL(valueChanged(int)), this, SLOT(onItemSizeSliderChanged(int)));

	// install organize mode options? Nope... we'll do that when showRun or showExperiment is called, in order to install the right ones.

	// change this to change the default first view. (In this case, we show all runs)
	viewMode_ = AMDataViews::ListView;
	organizeMode_ = AMDataViews::OrganizeRuns;

	runId_ = -3;
	experimentId_ = -3;
	showRun();


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

	if(runId_ == runId && runOrExp_)
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

	if(experimentId == experimentId_ && !runOrExp_)
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

	/// \todo optimization: changing the view mode doesn't necessarily need a refreshView, which would re-do all the sections. Instead, we could have a AMDataViewSection::setViewMode().
	refreshView();
}

/// called when the combo box is used to change the organizeMode
void AMDataView::onOrganizeModeBoxCurrentIndexChanged(int newIndex) {
	setOrganizeMode(organizeModeBox_->itemData(newIndex).toInt());
}

#include "ui/AMThumbnailScrollViewer.h"

// Called when the scene selection changes
void AMDataView::onSceneSelectionChanged() {
	selectedIds_.clear();

	QList<QGraphicsItem*> selectedItems = gscene_->selectedItems();
	foreach(QGraphicsItem* i, selectedItems) {
		/// \badcode Is there a way to handle this generally, so that we don't need to specifically know the type of the item class? Right now we only have AMThumbnailScrollGraphicsWidget's... but eventually there could be a whole bunch of QGraphicsItems used to represent scans
		AMThumbnailScrollGraphicsWidget* gw = qgraphicsitem_cast<AMThumbnailScrollGraphicsWidget*>(i);

		/// \todo Make this work for thumbnail scroll graphics widgets that are representing AMDbThumbnails as well.
		if(gw && gw->database() == db_ && gw->objectId() > 0) {
			selectedIds_ << gw->objectId();
			selectedTableNames_ << gw->tableName();
		}

	}
	processNewSelectedIds();
}

void AMDataView::processNewSelectedIds() {
	selectedUrls_.clear();

	QString prefix = "amd://" + db_->connectionName() + "/";

	for(int i=0; i<selectedIds_.count(); i++) {
		QString url = prefix;
		url.append(selectedTableNames_.at(i));
		url.append(QString("/%1").arg(selectedIds_.at(i)));
		selectedUrls_ << QUrl(url);
	}

	emit selected(selectedUrls_);
}

void AMDataView::onSceneDoubleClicked() {
	qDebug() << "Activated this many urls:" << selectedUrls_.count();

	if(selectedUrls_.count() > 0)
		emit activated(selectedUrls_);
}

/// This function runs everytime showRun() or showExperiment() is called, or a change is made to the OrganizeMode or ViewMode.  It re-populates the view from scratch.
void AMDataView::refreshView() {

	// delete the old views:
	foreach(QGraphicsLayoutItem* s, sections_)
		delete s;
	sections_.clear();
	sections__.clear();

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
						viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
				connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
				sections_ << section;
				sections__ << section;
				break;
			}

		case AMDataViews::OrganizeRuns:
			{
				// What runs do we have?
				bool found = false;
				QSqlQuery findRunIds = db_->query();
				findRunIds.setForwardOnly(true);
				findRunIds.prepare(QString("SELECT id, dateTime, name, endDateTime FROM AMRun_table"));
				if(findRunIds.exec()) {
					while(findRunIds.next()) {
						found = true;
						int runId = findRunIds.value(0).toInt();
						QString runName = findRunIds.value(2).toString();
						QDateTime dateTime = findRunIds.value(1).toDateTime();
						QDateTime endDateTime = findRunIds.value(3).toDateTime();
						QString fullRunName = runName + " (" + AMDateTimeUtils::prettyDateRange(dateTime, endDateTime) + ")";
						AMDataViewSection* section = new AMDataViewSection(
								fullRunName,
								"Showing all data from this run",
								QString("runId = '%1'").arg(runId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No runs found.");
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
					while(findExperiments.next()) {
						found = true;
						int expId = findExperiments.value(0).toInt();
						QString expName = findExperiments.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								expName,
								"Showing all data from this experiment",
								QString("id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1')").arg(expId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No experiments found.");
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
					while(findTypes.next()) {
						found = true;
						QString className = findTypes.value(0).toString();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								"Showing all " + typeDescription,
								QString("AMDbObjectType = '%1'").arg(className),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No data types found.");
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
				findSamples.prepare("SELECT id, dateTime, name FROM AMSample_table");	/// \todo thumbnail for samples
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								"Sample created " + AMDateTimeUtils::prettyDateTime(dt),
								QString("sampleId = '%1'").arg(sampleId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No samples found.");
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
					while(findElements.next()) {
						found = true;
						int elementId = findElements.value(0).toInt();
						QString symbol = findElements.value(1).toString();
						QString name = findElements.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1").arg(name),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1')").arg(elementId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No elements found.");
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
		runInfo.prepare("SELECT name, dateTime, endDateTime FROM AMRun_table where id = ?");
		runInfo.bindValue(0, runId_);
		if(runInfo.exec() && runInfo.next()) {
			runName = runInfo.value(0).toString();
			runTime = runInfo.value(1).toDateTime();
			runEndTime = runInfo.value(2).toDateTime();
			fullRunName = runName + " (" + AMDateTimeUtils::prettyDateRange(runTime, runEndTime) + ")";
			headingLabel_->setText(userName_ + "Runs: " + fullRunName);
		}
		else
			headingLabel_->setText(userName_ + "Data");


		switch(organizeMode_) {
		case AMDataViews::OrganizeRuns:
			{
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "This view is showing a single run, but the organize mode is set to organize by runs. This doesn't make sense and should never happen. Handling as OrganizeNone."));
			case AMDataViews::OrganizeNone:
				AMDataViewSection* section = new AMDataViewSection(
						fullRunName,
						"Showing all data from this run",
						QString("runId = '%1'").arg(runId_),
						viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
				connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
				sections_ << section;
				sections__ << section;
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
					while(findExperiments.next()) {
						found = true;
						int expId = findExperiments.value(0).toInt();
						QString expName = findExperiments.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								expName,
								QString("Showing all data from this experiment in the <i>%1</i> run").arg(fullRunName),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(runId_).arg(expId),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No experiments found.");
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
					while(findTypes.next()) {
						found = true;
						QString className = findTypes.value(0).toString();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> run").arg(fullRunName),
								QString("AMDbObjectType = '%1' AND runId = '%2'").arg(className).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No data types found.");
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
				findSamples.prepare("SELECT id, dateTime, name FROM AMSample_table WHERE id IN (SELECT sampleId FROM AMScan_table WHERE runId = ?)");	/// \todo add thumbnail icon!
				findSamples.bindValue(0, runId_);
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created %1.  Showing all data from this sample in the <i>%2</i> run").arg(AMDateTimeUtils::prettyDateTime(dt)).arg(fullRunName),
								QString("sampleId = '%1' AND runId = '%2'").arg(sampleId).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No samples found.");
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
					while(findElements.next()) {
						found = true;
						int elementId = findElements.value(0).toInt();
						QString symbol = findElements.value(1).toString();
						QString name = findElements.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1 in the <i>%2</i> run").arg(name).arg(fullRunName),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1') AND runId = '%2'").arg(elementId).arg(runId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No elements found.");
				}
				else {
					AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for elements in samples. The database might be corrupted."));
				}
			}
			break;

		}
	} // end of showing a specific run






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

		switch(organizeMode_) {
		case AMDataViews::OrganizeExperiments:
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "This view is showing a single experiment, but the organize mode is set to organize by experiments. This doesn't make sense and should never happen. Handling as OrganizeNone."));
		case AMDataViews::OrganizeNone:
			{
			AMDataViewSection* section = new AMDataViewSection(
					expName,
					"Showing all data from this experiment",
					QString("id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1')").arg(experimentId_),
					viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
			connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
			sections_ << section;
			sections__ << section;
		}
			break;

		case AMDataViews::OrganizeRuns:
			{
				// What runs do we have?
				bool found = false;
				QSqlQuery findRuns = db_->query();
				findRuns.setForwardOnly(true);
				findRuns.prepare(QString("SELECT id, name, dateTime, endDateTime FROM AMRun_table WHERE id IN (SELECT runId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1'))").arg(experimentId_));
				if(findRuns.exec()) {
					while(findRuns.next()) {
						found = true;
						int runId = findRuns.value(0).toInt();
						QString runName = findRuns.value(1).toString();
						QDateTime runTime = findRuns.value(2).toDateTime();
						QDateTime runEndTime = findRuns.value(3).toDateTime();
						QString fullRunName = runName + " (" + AMDateTimeUtils::prettyDateRange(runTime, runEndTime) + ")";
						AMDataViewSection* section = new AMDataViewSection(
								fullRunName,
								QString("Showing all data from this run in the <i>%1</i> experiment").arg(expName),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2');").arg(runId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No runs found.");
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
										  "WHERE ObjectExperimentEntries.experimentId = ? AND ObjectExperimentEntries.objectId = AMScan_table.id AND AMDbObjectTypes_table.AMDbObjectType = AMScan_table.AMDbObjectType;"));
				findTypes.bindValue(0, experimentId_);
				if(findTypes.exec()) {
					while(findTypes.next()) {
						found = true;
						QString className = findTypes.value(0).toString();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> experiment").arg(expName),
								QString("AMDbObjectType = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(className).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No data types found.");
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
				findSamples.prepare("SELECT id,dateTime,name FROM AMSample_table WHERE id IN (SELECT sampleId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?));");	/// \todo add thumbnail icon!
				findSamples.bindValue(0, experimentId_);
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created %1.  Showing all data from this sample in the <i>%2</i> experiment").arg(AMDateTimeUtils::prettyDateTime(dt)).arg(expName),
								QString("sampleId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(sampleId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No samples found.");
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
				findElements.prepare("SELECT id,symbol,name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries WHERE sampleId IN (SELECT sampleId FROM AMScan_table WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?)));");
				findElements.bindValue(0, experimentId_);
				if(findElements.exec()) {
					while(findElements.next()) {
						found = true;
						int elementId = findElements.value(0).toInt();
						QString symbol = findElements.value(1).toString();
						QString name = findElements.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1 in the <i>%2</i> experiment").arg(name).arg(expName),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1') AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(elementId).arg(experimentId_),
								viewMode_, db_, true, gwidget_, effectiveWidth(), sizeSlider->value());
						connect(gview_->verticalScrollBar(), SIGNAL(valueChanged(int)), section, SLOT(layoutHeaderItem()));
						sections_ << section;
						sections__ << section;
					}
					if(!found)
						sections_ << new AMDataViewEmptyHeader("No elements found.");
				}
				else {
					AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 0, "Error while searching database for elements in samples. The database might be corrupted."));
				}
			}
			break;

		}
	} // end of showing just one run




	// and show all the sections in the qgraphicsview layout area:
	foreach(QGraphicsLayoutItem* s, sections_) {
		sectionLayout_->addItem(s);
		sectionLayout_->setAlignment(s, Qt::AlignTop | Qt::AlignLeft);
	}

	// finally, set the visualized area of the scene to be exactly as big as we need it...
	// removed: different path of resize signalling now. adjustViewScrollHeight();

}

void AMDataView::adjustViewScrollableArea() {
	gview_->setSceneRect( gwidget_->rect() );
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
	int width = effectiveWidth();	// how much room we have available in the view area.

	// set the width constraint on every section in the main vertical layout.
	foreach(AMAbstractDataViewSection* s, sections__) {
		s->setWidthConstraint(width);
	}

	// These will invalidate the main layout, posting a LayoutEvent to the gwidget_. We'll handle the rest inside gwidget_'s event().
}

void AMDataView::onItemSizeSliderChanged(int newItemSize)
{
	foreach(AMAbstractDataViewSection* s, sections__) {
		s->setItemSize(newItemSize);
	}
}


AMDataViewSection::AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded, QGraphicsItem* parent, double initialWidthConstraint, int initialItemSize) : QGraphicsWidget(parent) {

	proxiedWidget_ = new QWidget();
	setupUi(proxiedWidget_);
	proxyWidget_ = new QGraphicsProxyWidget(this);
	proxyWidget_->setWidget(proxiedWidget_);
	proxyWidget_->setZValue(30000);

	subview_ = 0;
	subview__ = 0;

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
		case AMDataViews::ThumbnailView:
		default:
			/// \bug cannot presume all found in objectTableName.  Need a way of knowing where to look for these objects.
			AMDataViewSectionThumbnailView* v = new AMDataViewSectionThumbnailView(db_, AMDbObjectSupport::tableNameForClass<AMScan>(), whereClause_, this, widthConstraint_, itemSize_);
			subview_ = v;
			subview__ = v;
			/// \todo Set geometry here first, or wait until layout event?
			updateGeometry();
		}
	}
	else {
		expandButton_->setArrowType(Qt::RightArrow);
		expandButton_->setChecked(false);
		delete subview_;
		subview_ = 0;
		subview__ = 0;
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

	// qDebug() << "calling section setGeometry with rect" << rect;

	QGraphicsWidget::setGeometry(rect);

	layoutHeaderItem();
	if(expanded_)
		layoutContents();
}

void AMDataViewSection::layoutHeaderItem() {

	/// \todo Optimization: if this is being called from above on all, get rid of calling proxyWidget_->effectiveSizeHint() on each scroll.
	QRectF entireRect = rect();
	// qDebug() << "Calling Layout Header. entireRect = " << entireRect;

	qreal headerHeight = qMin(entireRect.height(),
						proxyWidget_->effectiveSizeHint(Qt::MinimumSize, QSize(entireRect.width(), -1)).height());

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
	// qDebug() << "Calling proxyWidget setGeometry with rect" << headerRect;
	proxyWidget_->setGeometry(headerRect);
}

void AMDataViewSection::layoutContents() {
	if(!expanded_)
		return;

	qreal verticalOffset = proxyWidget_->rect().height();
	QSizeF size = rect().size() - QSizeF(0, verticalOffset);
	// qDebug() << "calling contents setGeometry with rect" << QRectF(QPointF(0,verticalOffset), size);
	subview_->setGeometry(QRectF(QPointF(0,verticalOffset), size));
}

int AMDataViewSection::countResults() {
	QSqlQuery q = db_->query();
	QString query = QString("SELECT COUNT(1) FROM %1").arg(AMDbObjectSupport::tableNameForClass<AMScan>());
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
#include "ui/AMFlowGraphicsLayout.h"

AMDataViewSectionThumbnailView::AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent, double initialWidthConstraint, int initialItemSize)
	: QGraphicsWidget(parent) {

	db_ = db;
	dbTableName_ = dbTableName;
	whereClause_ = whereClause;

	layout_ = new AMFlowGraphicsLayout();
	layout_->setSpacing(Qt::Vertical, 30);
	layout_->setSpacing(Qt::Horizontal, 30);
	layout_->setUniformItemSizes(true);

	setWidthConstraint(initialWidthConstraint);
	setItemSize(initialItemSize);
	setLayout(layout_);

	populate();
}



#include "ui/AMThumbnailScrollViewer.h"

//#define AMDATAVIEWSECTIONTHUMBNAILVIEW_PROCESS_EVENTS_EVERY_N_ITEMS 50

void AMDataViewSectionThumbnailView::populate() {

	setVisible(false);

	// qDebug() << "AMDataViewSectionThumbnailView::populate(): \n   starting at " << QTime::currentTime().toString("mm:ss.zzz");

	/// \todo This won't work for samples, because they don't have a number column. Generalize or specificized.
	QSqlQuery q = db_->query();
	QString query = QString("SELECT thumbnailFirstId,thumbnailCount,name,number,dateTime,id FROM %1").arg(dbTableName_);
	if(!whereClause_.isEmpty())
		query.append(" WHERE ").append(whereClause_);
	query.append(" ORDER BY dateTime");
	q.prepare( query );

	//qDebug() << "   prior to executing database query: " << QTime::currentTime().toString("mm:ss.zzz");

	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, QString("Error executing database query '%1'. The error was %2").arg(q.executedQuery()).arg(q.lastError().text())));

	// qDebug() << "   after executing database query: " << QTime::currentTime().toString("mm:ss.zzz");

	// int processEventsBreakCounter = 0;

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

	// qDebug() << "   ending at " << QTime::currentTime().toString("mm:ss.zzz") << "\n";
	// layout_->activate();

	setVisible(true);
}

AMLayoutControlledGraphicsWidget::AMLayoutControlledGraphicsWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QGraphicsWidget(parent, wFlags)
{
}

bool AMLayoutControlledGraphicsWidget::event(QEvent *event)
{
	if(event->type() == QEvent::LayoutRequest) {
		if(layout()) {
			QSizeF layoutSizeHint = layout()->effectiveSizeHint(Qt::PreferredSize);
			if(size() != layoutSizeHint) {
//				qDebug() << "AMLayoutControlledGraphicsWidget: Catching a LayoutRequest and resizing ourself to match instead.";
				resize(layoutSizeHint);
				return true;	// don't pass this on like usual. Our resize() will trigger another LayoutRequest, but by then the size will match, so it'll be passed on through to the layout (below).
			}
		}
	}

	return QGraphicsWidget::event(event);
}

void AMLayoutControlledGraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
//	qDebug() << "AMLayoutControlledGraphicsWidget: Getting resized, and emitting resized signal.";
	QGraphicsWidget::resizeEvent(event);
	emit resized();
}




