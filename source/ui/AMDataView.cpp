#include "AMDataView.h"

#include "dataman/AMDatabase.h"
#include "dataman/AMDatabaseDefinition.h"
#include "AMErrorMonitor.h"

#include <QDebug>

AMDataView::AMDataView(AMDatabase* database, QWidget *parent) :
		QWidget(parent)
{
	db_ = database;
	runId_ = experimentId_ = -1;

	setupUi(this);

	// add additional UI components: the QGraphicsView and QGraphicsScene
	gview_ = new QGraphicsView();
	verticalLayout->addWidget(gview_);

	gscene_ = new QGraphicsScene(this);
	gview_->setScene(gscene_);
	gwidget_ = new QGraphicsWidget();
	gscene_->addItem(gwidget_);
	sectionLayout_ = new QGraphicsLinearLayout(Qt::Vertical);
	sectionLayout_->setContentsMargins(0,0,0,0);
	sectionLayout_->setSpacing(0);
	gwidget_->setLayout(sectionLayout_);
	// this refers to the QObject tree parent, not the QGraphicsItem tree parent
	gwidget_->setParent(this);

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

	// install organize mode options? Nope... we'll do that when showRun or showExperiment is called, in order to install the right ones.

	// change this to change the default first view. (In this case, we show all runs)
	viewMode_ = AMDataViews::ListView;
	organizeMode_ = AMDataViews::OrganizeRuns;
	showRun();

	onResize();


}


void AMDataView::retrieveUserName() {
	// Personalize the name:
	QSqlQuery findName = db_->query();
	findName.prepare("SELECT value FROM DatabaseInformation WHERE key = 'userName'");
	if(findName.exec() && findName.next()) {
		userName_ = findName.value(0).toString();
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
	runId_ = runId;
	runOrExp_ = true;
	if(runId_ < 0)
		organizeMode_ = AMDataViews::OrganizeRuns;
	if(runId_ >= 0 && organizeMode_ == AMDataViews::OrganizeRuns)
		organizeMode_ = AMDataViews::OrganizeNone;
	refreshView();
}

/// setup this view to show a specific experiment (or use \c experimentId = -1 to see all experiments)
void AMDataView::showExperiment(int experimentId) {
	experimentId_ = experimentId;
	runOrExp_ = false;
	if(experimentId_ < 0)
		organizeMode_ = AMDataViews::OrganizeExperiments;
	if(experimentId_ >= 0 && organizeMode_ == AMDataViews::OrganizeExperiments)
		organizeMode_ = AMDataViews::OrganizeNone;
	refreshView();
}

/// Set the headers used to separate and organize scans within a view. The \c mode is one of AMDataViews::OrganizeMode.
void AMDataView::setOrganizeMode(int mode) {

	if(organizeMode_ == mode)
		return;

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

	viewMode_ = (AMDataViews::ViewMode)mode;
	viewModeButtonGroup_->button(viewMode_)->setChecked(true);

	/// \todo optimization: changing the view mode doesn't necessarily need a refreshView, which would re-do all the sections. Instead, we could have a AMDataViewSection::setViewMode().
	refreshView();
}

/// called when the combo box is used to change the organizeMode
void AMDataView::onOrganizeModeBoxCurrentIndexChanged(int newIndex) {
	setOrganizeMode(organizeModeBox_->itemData(newIndex).toInt());
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
						viewMode_, db_, true, gwidget_);
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
				findRunIds.prepare(QString("SELECT id, dateTime, name FROM Runs"));
				if(findRunIds.exec()) {
					while(findRunIds.next()) {
						found = true;
						int runId = findRunIds.value(0).toInt();
						QString runName = findRunIds.value(2).toString();
						QDateTime dateTime = findRunIds.value(1).toDateTime();
						AMDataViewSection* section = new AMDataViewSection(
								runName + dateTime.toString(" MMM d (yyyy)"),
								"Showing all data from this run",
								QString("runId = '%1'").arg(runId),
								viewMode_, db_, true, gwidget_);
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
				findExperiments.prepare(QString("SELECT id, name FROM Experiments"));
				if(findExperiments.exec()) {
					while(findExperiments.next()) {
						found = true;
						int expId = findExperiments.value(0).toInt();
						QString expName = findExperiments.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								expName,
								"Showing all data from this experiment",
								QString("id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1')").arg(expId),
								viewMode_, db_, true, gwidget_);
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
				findTypes.prepare(QString("SELECT id, description FROM ObjectTypes WHERE id IN (SELECT typeId FROM Objects)"));
				if(findTypes.exec()) {
					while(findTypes.next()) {
						found = true;
						int typeId = findTypes.value(0).toInt();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								"Showing all " + typeDescription,
								QString("typeId = '%1'").arg(typeId),
								viewMode_, db_, true, gwidget_);
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
				findSamples.prepare("SELECT id, dateTime, name FROM Samples");	/// \todo thumbnail for samples
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								"Sample created on: " + dt.toString("h:mmap MMM d (yyyy)"),
								QString("sampleId = '%1'").arg(sampleId),
								viewMode_, db_, true, gwidget_);
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
								viewMode_, db_, true, gwidget_);
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
		QString runName;
		QDateTime runTime;
		QSqlQuery runInfo = db_->query();
		runInfo.prepare("SELECT name, dateTime FROM Runs where id = ?");
		runInfo.bindValue(0, runId_);
		if(runInfo.exec() && runInfo.next()) {
			runName = runInfo.value(0).toString();
			runTime = runInfo.value(1).toDateTime();
			headingLabel_->setText(userName_ + "Runs: " + runName + runTime.toString(" MMM d (yyyy)"));
		}
		else
			headingLabel_->setText(userName_ + "Data");


		switch(organizeMode_) {
		case AMDataViews::OrganizeRuns:
			{
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "This view is showing a single run, but the organize mode is set to organize by runs. This doesn't make sense and should never happen. Handling as OrganizeNone."));
			case AMDataViews::OrganizeNone:
				AMDataViewSection* section = new AMDataViewSection(
						runName + runTime.toString(" MMM d (yyyy)"),
						"Showing all data from this run",
						QString("runId = '%1'").arg(runId_),
						viewMode_, db_, true, gwidget_);
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
				findExperiments.prepare(QString("SELECT id, name FROM Experiments WHERE id IN (SELECT experimentId FROM ObjectExperimentEntries WHERE objectId IN (SELECT id FROM Objects WHERE runId = '%1'))").arg(runId_));
				if(findExperiments.exec()) {
					while(findExperiments.next()) {
						found = true;
						int expId = findExperiments.value(0).toInt();
						QString expName = findExperiments.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								expName,
								QString("Showing all data from this experiment in the <i>%1</i> run").arg(runName + runTime.toString(" MMM d (yyyy)")),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(runId_).arg(expId),
								viewMode_, db_, true, gwidget_);
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
				findTypes.prepare(QString("SELECT id, description FROM ObjectTypes WHERE id IN (SELECT DISTINCT typeId FROM Objects WHERE runId = ?)"));
				findTypes.bindValue(0, runId_);
				if(findTypes.exec()) {
					while(findTypes.next()) {
						found = true;
						int typeId = findTypes.value(0).toInt();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> run").arg(runName + runTime.toString(" MMM d (yyyy)")),
								QString("typeId = '%1' AND runId = '%2'").arg(typeId).arg(runId_),
								viewMode_, db_, true, gwidget_);
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
				findSamples.prepare("SELECT id, dateTime, name FROM Samples WHERE id IN (SELECT sampleId FROM Objects WHERE runId = ?)");	/// \todo add thumbnail icon!
				findSamples.bindValue(0, runId_);
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created at %1.  Showing all data from this sample in run <i>%2</i>").arg(dt.toString("h:mmap, MMM d (yyyy)")).arg(runName + runTime.toString(" MMM d (yyyy)")),
								QString("sampleId = '%1' AND runId = '%2'").arg(sampleId).arg(runId_),
								viewMode_, db_, true, gwidget_);
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
				findElements.prepare("SELECT id, symbol, name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries WHERE sampleId IN (SELECT sampleId FROM Objects WHERE runId = ?))");
				findElements.bindValue(0, runId_);
				if(findElements.exec()) {
					while(findElements.next()) {
						found = true;
						int elementId = findElements.value(0).toInt();
						QString symbol = findElements.value(1).toString();
						QString name = findElements.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								symbol + ": " + name,
								QString("Showing all data from samples containing %1 in the <i>%2</i> run").arg(name).arg(runName + runTime.toString(" MMM d (yyyy)")),
								QString("sampleId IN (SELECT sampleId FROM SampleElementEntries WHERE elementId = '%1') AND runId = '%2'").arg(elementId).arg(runId_),
								viewMode_, db_, true, gwidget_);
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
		expInfo.prepare("SELECT name FROM Experiments where id = ?");
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
					viewMode_, db_, true, gwidget_);
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
				findRuns.prepare(QString("SELECT id, name, dateTime FROM Runs WHERE id IN (SELECT runId FROM Objects WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%1'))").arg(experimentId_));
				if(findRuns.exec()) {
					while(findRuns.next()) {
						found = true;
						int runId = findRuns.value(0).toInt();
						QString runName = findRuns.value(1).toString();
						QDateTime runTime = findRuns.value(2).toDateTime();
						AMDataViewSection* section = new AMDataViewSection(
								runName + runTime.toString(" MMM d (yyyy)"),
								QString("Showing all data from this run in the <i>%1</i> experiment").arg(expName),
								QString("runId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2');").arg(runId).arg(experimentId_),
								viewMode_, db_, true, gwidget_);
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
				findTypes.prepare(QString("SELECT ObjectTypes.id, ObjectTypes.description, Objects.typeId, Objects.id, ObjectExperimentEntries.objectId, ObjectExperimentEntries.experimentId "
										  "FROM ObjectTypes, Objects, ObjectExperimentEntries "
										  "WHERE ObjectExperimentEntries.experimentId = ? AND ObjectExperimentEntries.objectId = Objects.id AND ObjectTypes.id = Objects.typeId;"));
				findTypes.bindValue(0, experimentId_);
				if(findTypes.exec()) {
					while(findTypes.next()) {
						found = true;
						int typeId = findTypes.value(0).toInt();
						QString typeDescription = findTypes.value(1).toString();
						AMDataViewSection* section = new AMDataViewSection(
								typeDescription,
								QString("Showing all data of this type in the <i>%1</i> experiment").arg(expName),
								QString("typeId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(typeId).arg(experimentId_),
								viewMode_, db_, true, gwidget_);
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
				findSamples.prepare("SELECT id,dateTime,name FROM Samples WHERE id IN (SELECT sampleId FROM Objects WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?));");	/// \todo add thumbnail icon!
				findSamples.bindValue(0, experimentId_);
				if(findSamples.exec()) {
					while(findSamples.next()) {
						found = true;
						int sampleId = findSamples.value(0).toInt();
						QDateTime dt = findSamples.value(1).toDateTime();
						QString name = findSamples.value(2).toString();
						AMDataViewSection* section = new AMDataViewSection(
								name,
								QString("Sample created at %1.  Showing all data from this sample in the <i>%2</i> experiment").arg(dt.toString("h:mmap, MMM d (yyyy)")).arg(expName),
								QString("sampleId = '%1' AND id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = '%2')").arg(sampleId).arg(experimentId_),
								viewMode_, db_, true, gwidget_);
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
				findElements.prepare("SELECT id,symbol,name FROM Elements WHERE id IN (SELECT elementId FROM SampleElementEntries WHERE sampleId IN (SELECT sampleId FROM Objects WHERE id IN (SELECT objectId FROM ObjectExperimentEntries WHERE experimentId = ?)));");
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
								viewMode_, db_, true, gwidget_);
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



	// set the proper width on all the sections...
	foreach(AMAbstractDataViewSection* s, sections__) {
		s->setWidthConstraint(this->width() - 20);
	}
	// and show all the sections in the qgraphicsview layout area:
	foreach(QGraphicsLayoutItem* s, sections_) {
		sectionLayout_->addItem(s);
	}



}

void AMDataView::refreshOrganizeModeBox() {

	// install the correct organizeMode options in the comboBox.

	// The full choices are { OrganizeNone, OrganizeRuns, OrganizeExperiments, OrganizeScanTypes, OrganizeSamples, OrganizeElements }.

	// However, OrganizeRuns makes no sense when showing a single run, and organizeExperiments makes no sense when showing a single experiment.
	organizeModeBox_->blockSignals(true);

	organizeModeBox_->clear();

	organizeModeBox_->addItem(QString(), AMDataViews::OrganizeNone);
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

	// QTimer::singleShot(0, this, SLOT(onResize()));
	onResize();

}


void AMDataView::onResize() {
	// Here we set the size of the graphics widget INSIDE the scene/scroll area to match the size if this widget (OUTSIDE the scroll area).
	// The width needs to be the width of this widget, less a scroll bar (so that the horizontal scroll bar doesn't appear)
	// It doesn't matter what we use for the height, because the vertical layout manager will increase the height as required based on the minimum heights of things inside it.
	int width = this->width() - 20;

	// Instead of gwidget_->resize(width, 100); we need to set the minimum and maximum width. This will cause the sectionLayout() to include that width as a constraint when asking sub-layouts for sizeHints().  This is required for the AMFlowGraphicsLayout to generate a correct sizeHint().  NOPE! Doesn't work! See next...
	gwidget_->setMaximumWidth(width);
	gwidget_->setMinimumWidth(width);
	gwidget_->layout()->setMaximumWidth(width);
	gwidget_->layout()->setMinimumWidth(width);
	gwidget_->layout()->setPreferredWidth(width);

	// suggestion to qt + qgraphicsview devs: when layouts are overridden with a setMaximumWidth and setMinimumWidth, they should actually pass on this constraint to their sub-layouts when asking for sizeHints().  Instead, we need to use this hack:
	foreach(AMAbstractDataViewSection* s, sections__) {
		s->setWidthConstraint(width);
	}

	// This will cause the layout (and sub-layouts) to re-calculate and re-layout their objects immediately. For performance reasons, you could replace this with sectionLayout_.invalidate(), which will only do one re-layout when control returns to the event loop.
	sectionLayout_->activate();

	// Finally, we need to decide how much of the graphics scene should be visible within the view's scroll area.
	gview_->setSceneRect( QRectF( QPointF(0,0), sectionLayout_->effectiveSizeHint(Qt::MinimumSize, QSizeF(width, -1)) ) );
}


AMDataViewSection::AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded, QGraphicsItem* parent) : QGraphicsWidget(parent) {

	proxiedWidget_ = new QWidget();
	setupUi(proxiedWidget_);
	QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
	proxy->setWidget(proxiedWidget_);

	subview_ = 0;
	subview__ = 0;

	layout_ = new QGraphicsLinearLayout(Qt::Vertical);
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(0);
	layout_->addItem(proxy);
	setLayout(layout_);


	title_ = title;
	subtitle_ = subtitle;
	titleLabel_->setText(title);
	subtitleLabel_->setText(subtitle);

	whereClause_ = whereClause;
	viewMode_ = viewMode;
	db_ = db;
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
			AMDataViewSectionThumbnailView* v = new AMDataViewSectionThumbnailView(db_, AMDatabaseDefinition::objectTableName(), whereClause_, this);
			subview_ = v;
			subview__ = v;
		}
		layout_->addItem(subview_);
		subview__->setWidthConstraint(widthConstraint_);
	}
	else {
		expandButton_->setArrowType(Qt::RightArrow);
		expandButton_->setChecked(false);
		delete subview_;
	}
}


int AMDataViewSection::countResults() {
	QSqlQuery q = db_->query();
	QString query = QString("SELECT COUNT(1) FROM %1").arg(AMDatabaseDefinition::objectTableName());
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

AMDataViewSectionThumbnailView::AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent)
	: QGraphicsWidget(parent) {

	db_ = db;
	dbTableName_ = dbTableName;
	whereClause_ = whereClause;

	thumbnailWidth_ = 240;

	layout_ = new AMFlowGraphicsLayout();
	layout_->setSpacing(Qt::Vertical, 30);
	layout_->setSpacing(Qt::Horizontal, 30);
	// layout_ = new QGraphicsLinearLayout(Qt::Vertical);
	setLayout(layout_);

	populate();
}

/*
#include <QResizeEvent>
#include <QTimer>
void AMDataViewSectionThumbnailView::resizeEvent(QResizeEvent *event) {

	QTimer::singleShot(0, this, SLOT(onResize()));

}

void AMDataViewSectionThumbnailView::onResize() {

	int internalWidth = this->width() - 5;

	if( int(gWidget_->geometry().width()) != internalWidth) {
		gWidget_->setMinimumWidth(internalWidth);
		gWidget_->setMaximumWidth(internalWidth);
		gWidget_->resize(internalWidth, gWidget_->geometry().height());

		gWidget_->layout()->invalidate();
		gWidget_->layout()->activate();
	}

	int fullHeight = gWidget_->layout()->effectiveSizeHint(Qt::MinimumSize, QSizeF(internalWidth, -1)).height();

	if(this->height() != fullHeight + 20) {
		this->setMaximumHeight(fullHeight + 20);
		this->setMinimumHeight(fullHeight + 20);
	}

	if( int(gWidget_->geometry().height()) != fullHeight ) {
		gWidget_->setGeometry(0,0,internalWidth, fullHeight);
		scene_->setSceneRect(0,0,this->width(), fullHeight);
		setSceneRect(0,0,internalWidth, fullHeight);
	}
}*/

#include "ui/AMThumbnailScrollViewer.h"

void AMDataViewSectionThumbnailView::setThumbnailWidth(double width) {
	thumbnailWidth_ = width;
	foreach(AMThumbnailScrollGraphicsWidget* w, thumbs_)
		w->setWidth(thumbnailWidth_);
}


#include <QPushButton>


void AMDataViewSectionThumbnailView::populate() {

	/// \todo This won't work for samples, because they don't have a number column. Generalize or specificized.
	QSqlQuery q = db_->query();
	QString query = QString("SELECT thumbnailFirstId,thumbnailCount,name,number,dateTime,id FROM %1").arg(dbTableName_);
	if(!whereClause_.isEmpty())
		query.append(" WHERE ").append(whereClause_);
	q.prepare( query );
	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, QString("Error executing database query '%1'. The error was %2").arg(q.executedQuery()).arg(q.lastError().text())));

	while(q.next()) {

		AMThumbnailScrollGraphicsWidget* w = new AMThumbnailScrollGraphicsWidget(this);
		w->setSource(db_, dbTableName_, q.value(5).toInt(), q.value(0).toInt(), q.value(1).toInt());
		QString caption1 = q.value(2).toString();
		if(q.value(3).toInt() != 0)
			caption1.append(QString(" #%1").arg(q.value(3).toInt()));
		w->setCaption1(caption1);
		w->setCaption2(q.value(4).toDateTime().toString("MMM d (yyyy)  h:mmap"));
		w->setWidth(thumbnailWidth_);
		layout_->addItem(w);

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
