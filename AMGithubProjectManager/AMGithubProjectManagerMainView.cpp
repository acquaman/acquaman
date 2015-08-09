#include "AMGithubProjectManagerMainView.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QDebug>
#include <QScrollArea>

#include "AMGitHubMilestone.h"
#include "AMGitHubIssueFamilyView.h"
#include "AMGitHubComplexityManager.h"
#include "AMGitHubRepository.h"
#include "AMGitHubProject.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotRectangle.h"

AMGithubProjectManagerMainView::AMGithubProjectManagerMainView(QWidget *parent)
	: QWidget(parent)
{
	initiateButton_ = new QPushButton("Initiate");
	connect(initiateButton_, SIGNAL(clicked()), this, SLOT(onInitiateButtonClicked()));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(initiateButton_);

	setLayout(vl);
}

AMGithubProjectManagerMainView::~AMGithubProjectManagerMainView()
{

}

void AMGithubProjectManagerMainView::onInitiateButtonClicked(){
	manager_ = new QNetworkAccessManager(this);

	repository_ = new AMGitHubRepository("acquaman", "acquaman", manager_, "token 2f8e7e362e5c0a5ea065255ccfdc369e70f4327b");

	allIssues_ = repository_->allIssues();
	allMilestones_ = repository_->allMilestones();
	allIssueFamilies_ = repository_->allIssueFamilies();

	connect(repository_, SIGNAL(repositoryLoaded()), this, SLOT(onGetAllZenhubEstimatesSucceeded()));
	repository_->initiateRepositoryLoading();
}

void AMGithubProjectManagerMainView::onGetAllZenhubEstimatesSucceeded()
{
	qDebug() << "Repository is loaded";

	qDebug() << "All Issues:                     " << repository_->issueCount();
	qDebug() << "Open Issues:                    " << repository_->openIssueCount();
	qDebug() << "Closed Issues:                  " << repository_->closedIssueCount();
	qDebug() << "\n";
	qDebug() << "Open and Specified:             " << repository_->fullySpecifiedOpenIssueCount();
	qDebug() << "Open but Missing Estimate:      " << repository_->missingEstimateOpenIssueCount();
	qDebug() << "\n";
	qDebug() << "Closed and Untracked:           " << repository_->completeUntrackedIssueCount();
	qDebug() << "Closed and Partially Tracked:   " << repository_->trackedWithoutEstimateIssueCount();
	qDebug() << "Closed and Fully Tracked:       " << repository_->fullyTrackedIssueCount();
	qDebug() << "\n";
	qDebug() << "Closed and Complete:            " << repository_->completeIssueCount();
	qDebug() << "Closed but Missing Estimate:    " << repository_->missingEstimateClosedIssueCount();
	qDebug() << "Closed but Missing Actual:      " << repository_->missingActualClosedIssueCount();
	qDebug() << "Closed but Missing Time Report: " << repository_->missingTimeClosedIssueCount();
	qDebug() << "Closed and Specified:           " << repository_->fullySpecifiedClosedIssueCount();
	qDebug() << "\n\n\n";

	AMGitHubComplexityManager *complexityManager = new AMGitHubComplexityManager();
	QMap<int, AMGitHubIssue*>::const_iterator ia = allIssues_->constBegin();
	while (ia != allIssues_->constEnd()) {
		if(ia.value()->completeIssue())
			complexityManager->addMapping(ia.value());
		ia++;
	}
	qDebug() << "Analyzed Issues: " << complexityManager->analyzedIssues();
	qDebug() << "Full Mapping Matrix";
	qDebug() << complexityManager->fullMatrixString();

	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	for(int x = 0, size = maxEstimate; x < size; x++){
		AMGitHubIssue::EstimatedComplexityValue oneEstimatedComplexity = AMGitHubIssue::EstimatedComplexityValue(x);
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		qDebug() << "\n";
		qDebug() << "Average Time For Estimate " << AMGitHubIssue::integerFromEstimatedComplexity(oneEstimatedComplexity) << complexityManager->averageTimeForEstimatedComplexity(oneEstimatedComplexity);
		qDebug() << "Average Time For Actual " << AMGitHubIssue::integerFromActualComplexity(oneActualComplexity) << complexityManager->averageTimeForActualComplexity(oneActualComplexity);
		qDebug() << "Probable Time For Estimate " << AMGitHubIssue::integerFromEstimatedComplexity(oneEstimatedComplexity) << complexityManager->probableTimeForEstimatedComplexity(oneEstimatedComplexity);
		qDebug() << complexityManager->probableTimeStringForEstimatedComplexity(oneEstimatedComplexity);
	}

	qDebug() << "\n\n";
	qDebug() << "All Families:             " << repository_->familyCount();
	qDebug() << "Fully Specified Families: " << repository_->fullySpecifiedFamilyCount();
	qDebug() << "Completed Families:       " << repository_->completedFamilyCount();

	qDebug() << "\n\n";
	qDebug() << "All Milestones:           " << repository_->milestoneCount();



	AMGitHubIssueValueMap normalizedEstimateMap;
	AMGitHubIssueValueMap normalizedCurrentMap;
	AMGitHubIssueValueMap normalizedCompletedMap;
	AMGitHubIssueValueMap normalizedTimeReportingMap;
	AMGitHubIssueValueMap normalizedOpenMap;
	AMGitHubIssueValueMap normalizedClosedMap;

	QList<AMGitHubIssue*> workingIssues;
	QMap<int, AMGitHubIssue*>::const_iterator ib = allIssues_->constBegin();
	while (ib != allIssues_->constEnd()) {
		if(ib.value()->issueFullySpecifiedForState() && !ib.value()->issueTrackedWithoutEstimates())
			workingIssues.append(ib.value());
		ib++;
	}

	for(int x = 0, size = workingIssues.count(); x < size; x++){
		AMGitHubIssue *oneIssue = workingIssues.at(x);
		AMGitHubIssue::EstimatedComplexityValue oneEstimatedComplexity = oneIssue->estimatedComplexityValue();
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = oneIssue->actualComplexityValue();
		double oneTimeReportingValue = oneIssue->normalizedTimeEstimate();

		if(oneEstimatedComplexity != AMGitHubIssue::EstimatedComplexityInvalid){
			normalizedEstimateMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(oneEstimatedComplexity));
			if(oneIssue->isOpen())
				normalizedOpenMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(oneEstimatedComplexity));
		}
		if(oneActualComplexity != AMGitHubIssue::ActualComplexityInvalid){
			normalizedCompletedMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::correspondingEstimatedComplexity(oneActualComplexity)));
			if(oneIssue->isClosed())
				normalizedClosedMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::correspondingEstimatedComplexity(oneActualComplexity)));
		}
		if(oneTimeReportingValue > 0){
			normalizedTimeReportingMap.insertMapping(oneIssue, oneTimeReportingValue);
		}

		if(oneIssue->isClosed())
			normalizedCurrentMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::correspondingEstimatedComplexity(oneActualComplexity)));
		else if(oneIssue->isOpen())
			normalizedCurrentMap.insertMapping(oneIssue, complexityManager->probableTimeForEstimatedComplexity(oneEstimatedComplexity));
	}

	qDebug() << QString("Normalized Estimates [%1] Total: %2").arg(normalizedEstimateMap.valueMap()->count()).arg(normalizedEstimateMap.total());
	qDebug() << QString("Normalized Current   [%1] Total: %2").arg(normalizedCurrentMap.valueMap()->count()).arg(normalizedCurrentMap.total());
	qDebug() << QString("Normalized Completed [%1] Total: %2").arg(normalizedCompletedMap.valueMap()->count()).arg(normalizedCompletedMap.total());
	qDebug() << QString("Normalized Times     [%1] Total: %2").arg(normalizedTimeReportingMap.valueMap()->count()).arg(normalizedTimeReportingMap.total());

	qDebug() << "\n\n";
	qDebug() << QString("Normalized Open      [%1] Total: %2").arg(normalizedOpenMap.valueMap()->count()).arg(normalizedOpenMap.total());
	qDebug() << QString("Normalized Closed    [%1] Total: %2").arg(normalizedClosedMap.valueMap()->count()).arg(normalizedClosedMap.total());

	return;
}

void AMGithubProjectManagerMainView::onOneZenhubEstimateUpdateSucceeded()
{

	return;


	/*
	AMGitHubMilestone *bioxasSideMilestone = allMilestones_.value(10);
	QList<double> normalizedEstimatedList;
	double normalizedEstimated = 0;
	QList<double> normalizedCurrentList;
	double normalizedCurrent = 0;
	QList<double> normalizedCompletedList;
	double normalizedCompleted = 0;
	QList<double> normalizedValueList;
	double normalizedValue = 0;

	QList<double> normalizedOpenList;
	double normalizedOpen = 0;
	QList<double> normalizedClosedList;
	double normalizedClosed = 0;

	QWidget *bioXASSideMasterWidget = new QWidget();
	QVBoxLayout *bioXASSideMasterVL = new QVBoxLayout();
	QScrollArea *bioXASSideScrollArea = new QScrollArea();
	QWidget *bioXASSideWidget = new QWidget();
	QVBoxLayout *bioXASSideVL = new QVBoxLayout();

	//		QList<AMGitHubIssueFamily*> workingIssueFamilies = bioxasSideMilestone->associatedFamilies();
	QList<AMGitHubIssueFamily*> workingIssueFamilies;
	QMap<int, AMGitHubIssueFamily*>::const_iterator rr = allIssueFamilies_.constBegin();
	while(rr != allIssueFamilies_.constEnd()){
		qDebug() << "Adding " << rr.value()->originatingIssueNumber() << "as" << workingIssueFamilies.count();
		workingIssueFamilies.append(rr.value());
		rr++;
	}

	for(int x = 0, size = workingIssueFamilies.count(); x < size; x++){
		//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(bioxasSideMilestone->associatedFamilies().at(x));
		//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(workingIssueFamilies.at(x));
		//			bioXASSideVL->addWidget(oneFamilyView);

		//			qDebug() << "Issue " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber() << " created at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->createdDate() << " and closed at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->closedDate();

		//			qDebug() << "Family " << x << " orginates from " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber();


		qDebug() << "About to look into " << x << workingIssueFamilies.at(x)->originatingIssueNumber();
		if(workingIssueFamilies.at(x)->estimatedComplexity() != AMGitHubIssue::ActualComplexityInvalid){
			normalizedEstimatedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
			if(workingIssueFamilies.at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
				normalizedOpenList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
		}
		if(workingIssueFamilies.at(x)->actualComplexity() != AMGitHubIssue::ActualComplexityInvalid){
			normalizedCompletedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
			if(workingIssueFamilies.at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
				normalizedClosedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
		}
		if(workingIssueFamilies.at(x)->normalizedTimeEstiamte() > 0){
			normalizedValueList.append(workingIssueFamilies.at(x)->normalizedTimeEstiamte());
		}

		if(workingIssueFamilies.at(x)->originatingIssue()->isClosed())
			normalizedCurrentList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
		else if(workingIssueFamilies.at(x)->originatingIssue()->isOpen())
			normalizedCurrentList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
	}

	bioXASSideWidget->setLayout(bioXASSideVL);
	bioXASSideScrollArea->setWidget(bioXASSideWidget);

	bioXASSideMasterVL->addWidget(bioXASSideScrollArea);
	bioXASSideMasterWidget->setLayout(bioXASSideMasterVL);
	bioXASSideMasterWidget->show();

	for(int x = 0, size = normalizedEstimatedList.count(); x < size; x++)
		normalizedEstimated += normalizedEstimatedList.at(x);
	for(int x = 0, size = normalizedCurrentList.count(); x < size; x++)
		normalizedCurrent += normalizedCurrentList.at(x);
	for(int x = 0, size = normalizedCompletedList.count(); x < size; x++)
		normalizedCompleted += normalizedCompletedList.at(x);
	for(int x = 0, size = normalizedValueList.count(); x < size; x++)
		normalizedValue += normalizedValueList.at(x);
	for(int x = 0, size = normalizedOpenList.count(); x < size; x++)
		normalizedOpen += normalizedOpenList.at(x);
	for(int x = 0, size = normalizedClosedList.count(); x < size; x++)
		normalizedClosed += normalizedClosedList.at(x);

	qDebug() << "\n\n\n\n";

	//		qDebug() << "Normalized Estimated List: " << normalizedEstimatedList;
	//		qDebug() << "Normalized Current List: " << normalizedCurrentList;
	//		qDebug() << "Normalized Completed List: " << normalizedCompletedList;
	//		qDebug() << "Normalized Value List: " << normalizedValueList;

	qDebug() << "Normalized Estimated Total: " << normalizedEstimated;
	qDebug() << "Normalized Current Total: " << normalizedCurrent;
	qDebug() << "Normalized Completed Total: " << normalizedCompleted;
	qDebug() << "Normalized Value Total: " << normalizedValue;

	qDebug() << "\n\n";
	//		qDebug() << "Normalized Open List: " << normalizedOpenList;
	//		qDebug() << "Normalized Closed List: " << normalizedClosedList;

	qDebug() << "Normalized Open Total: " << normalizedOpen;
	qDebug() << "Normalized Closed Total: " << normalizedClosed;


	QList<QList<double> > allOutstandingEstimatesLists;
	QList<double> allOutstandingEstimatesTotals;
	QList<QList<double> > allCompletedEstimatesLists;
	QList<double> allCompletedEstimatesTotals;
	QList<QList<double> > allWithdrawnEstimatesLists;
	QList<double> allWithdrawnEstimatesTotals;

	QList<QList<double> > allComplexityMappedCompetedWorkLists;
	QList<double> allComplexityMappedCompetedWorkTotals;
	QList<QList<double> > allReportedCompetedWorkLists;
	QList<double> allReportedCompetedWorkTotals;

	QList<QDateTime> dateList;
	QDateTime oneDateTime = QDateTime::currentDateTime();
	dateList.append(oneDateTime);
	allOutstandingEstimatesLists.append(QList<double>());
	allCompletedEstimatesLists.append(QList<double>());
	allWithdrawnEstimatesLists.append(QList<double>());
	allOutstandingEstimatesTotals.append(0);
	allCompletedEstimatesTotals.append(0);
	allWithdrawnEstimatesTotals.append(0);

	allComplexityMappedCompetedWorkLists.append(QList<double>());
	allReportedCompetedWorkLists.append(QList<double>());
	allComplexityMappedCompetedWorkTotals.append(0);
	allReportedCompetedWorkTotals.append(0);

	//		for(int x = 0, size = 10; x < size; x++){
	for(int x = 0, size = 12; x < size; x++){
		oneDateTime = oneDateTime.addDays(-7);
		dateList.append(oneDateTime);
		allOutstandingEstimatesLists.append(QList<double>());
		allCompletedEstimatesLists.append(QList<double>());
		allWithdrawnEstimatesLists.append(QList<double>());
		allOutstandingEstimatesTotals.append(0);
		allCompletedEstimatesTotals.append(0);
		allWithdrawnEstimatesTotals.append(0);

		allComplexityMappedCompetedWorkLists.append(QList<double>());
		allReportedCompetedWorkLists.append(QList<double>());
		allComplexityMappedCompetedWorkTotals.append(0);
		allReportedCompetedWorkTotals.append(0);
	}


	//		for(int x = 0, xSize = bioxasSideMilestone->associatedFamilies().count(); x < xSize; x++){
	for(int x = 0, xSize = workingIssueFamilies.count(); x < xSize; x++){
		for(int y = 0, ySize = dateList.count(); y < ySize; y++){
			allOutstandingEstimatesLists[y].append(complexityManager->outstandingEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));
			allCompletedEstimatesLists[y].append(complexityManager->completedEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));
			allWithdrawnEstimatesLists[y].append(complexityManager->withdrawnEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));

			allComplexityMappedCompetedWorkLists[y].append(complexityManager->complexityMappedCompletedWorkAtDate(workingIssueFamilies.at(x), dateList.at(y)));
			allReportedCompetedWorkLists[y].append(complexityManager->reportedCompletedWorkAtDate(workingIssueFamilies.at(x), dateList.at(y)));
		}
	}

	for(int x = 0, xSize = dateList.count(); x < xSize; x++){
		for(int y = 0, ySize = allOutstandingEstimatesLists.at(x).count(); y < ySize; y++)
			allOutstandingEstimatesTotals[x] = allOutstandingEstimatesTotals.at(x)+allOutstandingEstimatesLists.at(x).at(y);
		for(int y = 0, ySize = allCompletedEstimatesLists.at(x).count(); y < ySize; y++)
			allCompletedEstimatesTotals[x] = allCompletedEstimatesTotals.at(x)+allCompletedEstimatesLists.at(x).at(y);
		for(int y = 0, ySize = allWithdrawnEstimatesLists.at(x).count(); y < ySize; y++)
			allWithdrawnEstimatesTotals[x] = allWithdrawnEstimatesTotals.at(x)+allWithdrawnEstimatesLists.at(x).at(y);

		for(int y = 0, ySize = allComplexityMappedCompetedWorkLists.at(x).count(); y < ySize; y++)
			allComplexityMappedCompetedWorkTotals[x] = allComplexityMappedCompetedWorkTotals.at(x)+allComplexityMappedCompetedWorkLists.at(x).at(y);
		for(int y = 0, ySize = allReportedCompetedWorkLists.at(x).count(); y < ySize; y++)
			allReportedCompetedWorkTotals[x] = allReportedCompetedWorkTotals.at(x)+allReportedCompetedWorkLists.at(x).at(y);
	}

	QList<double> weeklyEstimatedVelocity;
	QList<double> weeklyComplexityMappedVelocity;
	QList<double> weeklyReportedVelocity;

	weeklyEstimatedVelocity.append(0);
	weeklyComplexityMappedVelocity.append(0);
	weeklyReportedVelocity.append(0);
	for(int x = 1, size = dateList.count(); x < size; x++){
		weeklyEstimatedVelocity.append(allCompletedEstimatesTotals.at(x-1)-allCompletedEstimatesTotals.at(x));
		weeklyComplexityMappedVelocity.append(allComplexityMappedCompetedWorkTotals.at(x-1)-allComplexityMappedCompetedWorkTotals.at(x));
		weeklyReportedVelocity.append(allReportedCompetedWorkTotals.at(x-1)-allReportedCompetedWorkTotals.at(x));
	}

	double averageEstimatedVelocityTotal = 0;
	double averageComplexityMappedVelocityTotal = 0;
	double averageReportedVelocityTotal = 0;

	for(int x = 0, size = dateList.count(); x < size; x++){
		averageEstimatedVelocityTotal += weeklyEstimatedVelocity.at(x);
		averageComplexityMappedVelocityTotal += weeklyComplexityMappedVelocity.at(x);
		averageReportedVelocityTotal += weeklyReportedVelocity.at(x);
	}

	double averageCalendarEstimatedVelocity  = averageEstimatedVelocityTotal/(dateList.count()-1);
	double averageCalendarComplexityMappedVelocity = averageComplexityMappedVelocityTotal/(dateList.count()-1);
	double averageCalendarReportedVelocity = averageReportedVelocityTotal/(dateList.count()-1);
	double availablePersonWeeks = 48-(3+2+2+4); //10 weeks, 4 full time people, Sheldon away for 3 weeks vacation, Sheldon half time for one month, Darren gone for 2 weeks, Iain didn't start until May
	//		double availablePersonWeeks = 16;
	double averageAvailabilityEstimatedVelocity = averageEstimatedVelocityTotal/availablePersonWeeks;
	double averageAvailabilityComplexityMappedVelocity = averageComplexityMappedVelocityTotal/availablePersonWeeks;
	double averageAvailabilityReportedVelocity = averageReportedVelocityTotal/availablePersonWeeks;

	//		for(int x = 0, xSize = dateList.count(); x < xSize; x++){
	//			qDebug() << "At date: " << dateList.at(x);
	//			qDebug() << "Outstanding Estimate List: " << allOutstandingEstimatesLists.at(x);
	//			qDebug() << "Completed Estimate List: " << allCompletedEstimatesLists.at(x);
	//			qDebug() << "Withdrawn Estimate List: " << allWithdrawnEstimatesLists.at(x);
	//			qDebug() << "Outstanding Estimate Total: " << allOutstandingEstimatesTotals.at(x);
	//			qDebug() << "Completed Estimate Total: " << allCompletedEstimatesTotals.at(x);
	//			qDebug() << "Withdrawn Estimate Total: " << allWithdrawnEstimatesTotals.at(x);

	//			qDebug() << "Complexity Mapped Completed Work List: " << allComplexityMappedCompetedWorkLists.at(x);
	//			qDebug() << "Reported Completed Work List: " << allReportedCompetedWorkLists.at(x);
	//			qDebug() << "Complexity Mapped Completed Work Total: " << allComplexityMappedCompetedWorkTotals.at(x);
	//			qDebug() << "Reported Completed Work Total: " << allReportedCompetedWorkTotals.at(x);

	//			qDebug() << "\n\n";
	//		}

	//		qDebug() << "Weekly Estimated Velocities: " << weeklyEstimatedVelocity;
	//		qDebug() << "Weekly Complexity Mapped Velocities: " << weeklyComplexityMappedVelocity;
	//		qDebug() << "Weekly Reported Velocities: " << weeklyReportedVelocity;

	qDebug() << "\n\n";
	qDebug() << "Average Estimated Velocity: " << averageCalendarEstimatedVelocity << "or" << averageAvailabilityEstimatedVelocity;
	qDebug() << "Average Complexity Mapped Velocity: " << averageCalendarComplexityMappedVelocity << "or" << averageAvailabilityComplexityMappedVelocity;
	qDebug() << "Average Reported Velocity: " << averageCalendarReportedVelocity << "or" << averageAvailabilityReportedVelocity;
	*/


	/* NO PLOTTING
		// Create the plot window.
		MPlotWidget *plotView = new MPlotWidget;
		plotView->enableAntiAliasing(true);

		// Create the plot and setup all the axes.
		MPlot *plot = new MPlot;
		plot->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
		plot->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
		plot->axisBottom()->setAxisName("Weeks, index");
		plot->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
		plot->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
		plot->axisLeft()->setAxisName("Hours");

		// Set the margins for the plot.
		plot->setMarginLeft(10);
		plot->setMarginBottom(15);
		plot->setMarginRight(2);
		plot->setMarginTop(2);

		// Enable autoscaling of both axes.
		plot->axisScaleLeft()->setAutoScaleEnabled();
		plot->axisScaleBottom()->setAutoScaleEnabled();

		// Enable some convenient zoom tools.
		plot->addTool(new MPlotDragZoomerTool());
		plot->addTool(new MPlotWheelZoomerTool());
		plotView->setPlot(plot);
		plotView->setMinimumHeight(450);
		plotView->setMinimumWidth(600);

		// Set the number of ticks.  A balance between readability and being practical.
		plot->axisBottom()->setTicks(3);
		plot->axisTop()->setTicks(0);
		plot->axisLeft()->setTicks(4);
		plot->axisRight()->setTicks(0);

		// Set the autoscale constraints.
		plot->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));

		MPlotRectangle *oneOutstandingEstimateRectangle;
		MPlotRectangle *oneCompletedEstimateRectangle;
		MPlotRectangle *oneWithdrawnEstimateRectangle;

		MPlotRectangle *oneEstimatedCompletedWorkRectangle;
		MPlotRectangle *oneComplexityMappedCompletedWorkRectangle;
		MPlotRectangle *oneReportedCompletedWorkRectangle;

		qDebug() << "\n\n";
		for(int x = 0, size = dateList.count(); x < size; x++){
			qreal xpos = (dateList.count()-x)*4;
//			qDebug() << "Place index " << x << "at" << xpos << "with outstanding" << allOutstandingEstimatesTotals.at(x) << "and completed" << allCompletedEstimatesTotals.at(x) << "and withdrawn" << allWithdrawnEstimatesTotals.at(x)
			qDebug() << "Date " << dateList.at(x) << "with outstanding" << allOutstandingEstimatesTotals.at(x) << "and completed" << allCompletedEstimatesTotals.at(x) << "and withdrawn" << allWithdrawnEstimatesTotals.at(x)
				 << "and complexity mapped completed" << allComplexityMappedCompetedWorkTotals.at(x) << "and reported completed" << allReportedCompetedWorkTotals.at(x);

			oneOutstandingEstimateRectangle = new MPlotRectangle(QRectF(xpos, 0, 1, allOutstandingEstimatesTotals.at(x)), Qt::NoPen, QBrush(Qt::red));
			oneOutstandingEstimateRectangle->setLegendVisibility(false);
			oneCompletedEstimateRectangle = new MPlotRectangle(QRectF(xpos, allOutstandingEstimatesTotals.at(x), 1, allCompletedEstimatesTotals.at(x)), Qt::NoPen, QBrush(Qt::blue));
			oneCompletedEstimateRectangle->setLegendVisibility(false);
			oneWithdrawnEstimateRectangle = new MPlotRectangle(QRectF(xpos, -allWithdrawnEstimatesTotals.at(x), 1, allWithdrawnEstimatesTotals.at(x)), Qt::NoPen, QBrush(Qt::black));
			oneWithdrawnEstimateRectangle->setLegendVisibility(false);

			plot->addItem(oneOutstandingEstimateRectangle);
			plot->addItem(oneCompletedEstimateRectangle);
			plot->addItem(oneWithdrawnEstimateRectangle);

			oneEstimatedCompletedWorkRectangle = new MPlotRectangle(QRectF(xpos+1.33, 0, 0.33, allCompletedEstimatesTotals.at(x)), Qt::NoPen, QBrush(Qt::blue));
			oneEstimatedCompletedWorkRectangle->setLegendVisibility(false);
			oneComplexityMappedCompletedWorkRectangle = new MPlotRectangle(QRectF(xpos+1.66, 0, 0.33, allComplexityMappedCompetedWorkTotals.at(x)), Qt::NoPen, QBrush(Qt::green));
			oneComplexityMappedCompletedWorkRectangle->setLegendVisibility(false);
			oneReportedCompletedWorkRectangle = new MPlotRectangle(QRectF(xpos+1.99, 0, 0.33, allReportedCompetedWorkTotals.at(x)), Qt::NoPen, QBrush(Qt::darkGreen));
			oneReportedCompletedWorkRectangle->setLegendVisibility(false);

			plot->addItem(oneEstimatedCompletedWorkRectangle);
			plot->addItem(oneComplexityMappedCompletedWorkRectangle);
			plot->addItem(oneReportedCompletedWorkRectangle);
		}

		plotView->show();
		*/
}

