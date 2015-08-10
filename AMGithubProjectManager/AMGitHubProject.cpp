#include "AMGitHubProject.h"

AMGitHubIssueValueMap::AMGitHubIssueValueMap(QObject *parent) :
	QObject(parent)
{
	valueMap_ = new QMap<const AMGitHubIssue*, double>();
}

bool AMGitHubIssueValueMap::containsIssue(const AMGitHubIssue *issue) const
{
	return valueMap_->contains(issue);
}

double AMGitHubIssueValueMap::valueForIssue(const AMGitHubIssue *issue) const
{
	if(valueMap_->contains(issue))
		return valueMap_->value(issue);
	return 0;
}

QList<double> AMGitHubIssueValueMap::valueList() const
{
	QList<double> retVal;
	QMap<const AMGitHubIssue*, double>::const_iterator i = valueMap_->constBegin();
	while(i != valueMap_->constEnd()){
		retVal.append(i.value());
		i++;
	}
	return retVal;
}

double AMGitHubIssueValueMap::total() const
{
	double runningTotal = 0;
	QMap<const AMGitHubIssue*, double>::const_iterator i = valueMap_->constBegin();
	while(i != valueMap_->constEnd()){
		runningTotal += i.value();
		i++;
	}
	return runningTotal;
}

double AMGitHubIssueValueMap::average(double divisor) const
{
	if(divisor < 0)
		return total()/(double)(valueMap_->count());
	if(divisor == 0)
		return 0;
	return total()/divisor;
}

void AMGitHubIssueValueMap::insertMapping(const AMGitHubIssue *issue, double value)
{
	if(valueMap_->contains(issue))
		return;
	valueMap_->insert(issue, value);
}

AMGitHubIssueValueDateProgression::AMGitHubIssueValueDateProgression(QObject *parent) :
	QObject(parent)
{
	dateProgression_ = new QMap<QDateTime, AMGitHubIssueValueMap*>();
}

bool AMGitHubIssueValueDateProgression::containsDateTime(const QDateTime &dateTime) const
{
	return dateProgression_->contains(dateTime);
}

const AMGitHubIssueValueMap* AMGitHubIssueValueDateProgression::valueMapForDateTime(const QDateTime &dateTime) const
{
	if(dateProgression_->contains(dateTime))
		return dateProgression_->value(dateTime);
	return 0;
}

QList<QDateTime> AMGitHubIssueValueDateProgression::dateTimeList() const
{
	QList<QDateTime> retVal;
	QMap<QDateTime, AMGitHubIssueValueMap*>::const_iterator i = dateProgression_->constBegin();
	while(i != dateProgression_->constEnd()){
		retVal.append(i.key());
		i++;
	}
	return retVal;
}

QList<AMGitHubIssueValueMap*> AMGitHubIssueValueDateProgression::valueMapList() const
{
	QList<AMGitHubIssueValueMap*> retVal;
	QMap<QDateTime, AMGitHubIssueValueMap*>::const_iterator i = dateProgression_->constBegin();
	while(i != dateProgression_->constEnd()){
		retVal.append(i.value());
		i++;
	}
	return retVal;
}

QList<double> AMGitHubIssueValueDateProgression::totalsList() const
{
	QList<double> retVal;
	QMap<QDateTime, AMGitHubIssueValueMap*>::const_iterator i = dateProgression_->constBegin();
	while(i != dateProgression_->constEnd()){
		retVal.append(totalForDate(i.key()));
		i++;
	}
	return retVal;
}

QList<double> AMGitHubIssueValueDateProgression::averageList(double divisor) const
{
	QList<double> retVal;
	QMap<QDateTime, AMGitHubIssueValueMap*>::const_iterator i = dateProgression_->constBegin();
	while(i != dateProgression_->constEnd()){
		retVal.append(averageForDate(i.key(), divisor));
		i++;
	}
	return retVal;
}

double AMGitHubIssueValueDateProgression::totalForDate(const QDateTime &dateTime) const
{
	if(dateProgression_->contains(dateTime))
		return dateProgression_->value(dateTime)->total();
	return 0;
}

double AMGitHubIssueValueDateProgression::averageForDate(const QDateTime &dateTime, double divisor) const
{
	if(dateProgression_->contains(dateTime))
		return dateProgression_->value(dateTime)->average(divisor);
	return 0;
}

void AMGitHubIssueValueDateProgression::insertMapping(const QDateTime &dateTime, AMGitHubIssueValueMap *valueMap)
{
	if(dateProgression_->contains(dateTime))
		return;
	dateProgression_->insert(dateTime, valueMap);
}

/*
AMGitHubProject::AMGitHubProject(QList<AMGitHubIssueFamily *> projectIssues, const QDateTime &endDate, int weeksToCompute, QObject *parent) :
	QObject(parent)
{
	projectIssues_ = projectIssues;
	endDate_ = endDate;
	weeksToCompute_ = weeksToCompute;

	normalizedEstimated_ = 0;
	normalizedCurrent_ = 0;
	normalizedCompleted_ = 0;
	normalizedValue_ = 0;
	normalizedOpen_ = 0;
	normalizedClosed_ = 0;
}

AMGitHubProject::AMGitHubProject(AMGitHubMilestone *projectMilestone, const QDateTime &endDate, int weeksToCompute, QObject *parent) :
	QObject(parent)
{
	projectIssues_ = projectMilestone->associatedFamilies();
	endDate_ = endDate;
	weeksToCompute_ = weeksToCompute;
}

void AMGitHubProject::evaluate(){


//	QList<AMGitHubIssueFamily*> workingIssueFamilies;
//	QMap<int, AMGitHubIssueFamily*>::const_iterator rr = allIssueFamilies_.constBegin();
//	while(rr != allIssueFamilies_.constEnd()){
//		qDebug() << "Adding " << rr.value()->originatingIssueNumber() << "as" << workingIssueFamilies.count();
//		workingIssueFamilies.append(rr.value());
//		rr++;
//	}

//	//		for(int x = 0, size = bioxasSideMilestone->associatedFamilies().count(); x < size; x++){
//	for(int x = 0, size = workingIssueFamilies.count(); x < size; x++){
//		//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(bioxasSideMilestone->associatedFamilies().at(x));
//		//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(workingIssueFamilies.at(x));
//		//			bioXASSideVL->addWidget(oneFamilyView);

//		//			qDebug() << "Issue " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber() << " created at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->createdDate() << " and closed at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->closedDate();

//		//			qDebug() << "Family " << x << " orginates from " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber();


//		qDebug() << "About to look into " << x << workingIssueFamilies.at(x)->originatingIssueNumber();
//		if(workingIssueFamilies.at(x)->estimatedComplexity() != AMGitHubIssue::InvalidComplexity){
//			normalizedEstimatedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
//			if(workingIssueFamilies.at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
//				normalizedOpenList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
//		}
//		if(workingIssueFamilies.at(x)->actualComplexity() != AMGitHubIssue::InvalidComplexity){
//			normalizedCompletedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
//			if(workingIssueFamilies.at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
//				normalizedClosedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
//		}
//		if(workingIssueFamilies.at(x)->normalizedTimeEstiamte() > 0){
//			normalizedValueList.append(workingIssueFamilies.at(x)->normalizedTimeEstiamte());
//		}

//		if(workingIssueFamilies.at(x)->originatingIssue()->isClosed())
//			normalizedCurrentList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->actualComplexity()));
//		else if(workingIssueFamilies.at(x)->originatingIssue()->isOpen())
//			normalizedCurrentList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
//	}

//	for(int x = 0, size = normalizedEstimatedList.count(); x < size; x++)
//		normalizedEstimated += normalizedEstimatedList.at(x);
//	for(int x = 0, size = normalizedCurrentList.count(); x < size; x++)
//		normalizedCurrent += normalizedCurrentList.at(x);
//	for(int x = 0, size = normalizedCompletedList.count(); x < size; x++)
//		normalizedCompleted += normalizedCompletedList.at(x);
//	for(int x = 0, size = normalizedValueList.count(); x < size; x++)
//		normalizedValue += normalizedValueList.at(x);
//	for(int x = 0, size = normalizedOpenList.count(); x < size; x++)
//		normalizedOpen += normalizedOpenList.at(x);
//	for(int x = 0, size = normalizedClosedList.count(); x < size; x++)
//		normalizedClosed += normalizedClosedList.at(x);

//	qDebug() << "\n\n\n\n";

//	//		qDebug() << "Normalized Estimated List: " << normalizedEstimatedList;
//	//		qDebug() << "Normalized Current List: " << normalizedCurrentList;
//	//		qDebug() << "Normalized Completed List: " << normalizedCompletedList;
//	//		qDebug() << "Normalized Value List: " << normalizedValueList;

//	qDebug() << "Normalized Estimated Total: " << normalizedEstimated;
//	qDebug() << "Normalized Current Total: " << normalizedCurrent;
//	qDebug() << "Normalized Completed Total: " << normalizedCompleted;
//	qDebug() << "Normalized Value Total: " << normalizedValue;

//	qDebug() << "\n\n";
//	//		qDebug() << "Normalized Open List: " << normalizedOpenList;
//	//		qDebug() << "Normalized Closed List: " << normalizedClosedList;

//	qDebug() << "Normalized Open Total: " << normalizedOpen;
//	qDebug() << "Normalized Closed Total: " << normalizedClosed;


//	QList<QList<double> > allOutstandingEstimatesLists;
//	QList<double> allOutstandingEstimatesTotals;
//	QList<QList<double> > allCompletedEstimatesLists;
//	QList<double> allCompletedEstimatesTotals;
//	QList<QList<double> > allWithdrawnEstimatesLists;
//	QList<double> allWithdrawnEstimatesTotals;

//	QList<QList<double> > allComplexityMappedCompetedWorkLists;
//	QList<double> allComplexityMappedCompetedWorkTotals;
//	QList<QList<double> > allReportedCompetedWorkLists;
//	QList<double> allReportedCompetedWorkTotals;

//	QList<QDateTime> dateList;
//	QDateTime oneDateTime = QDateTime::currentDateTime();
//	dateList.append(oneDateTime);
//	allOutstandingEstimatesLists.append(QList<double>());
//	allCompletedEstimatesLists.append(QList<double>());
//	allWithdrawnEstimatesLists.append(QList<double>());
//	allOutstandingEstimatesTotals.append(0);
//	allCompletedEstimatesTotals.append(0);
//	allWithdrawnEstimatesTotals.append(0);

//	allComplexityMappedCompetedWorkLists.append(QList<double>());
//	allReportedCompetedWorkLists.append(QList<double>());
//	allComplexityMappedCompetedWorkTotals.append(0);
//	allReportedCompetedWorkTotals.append(0);

//	for(int x = 0, size = 12; x < size; x++){
//		oneDateTime = oneDateTime.addDays(-7);
//		dateList.append(oneDateTime);
//		allOutstandingEstimatesLists.append(QList<double>());
//		allCompletedEstimatesLists.append(QList<double>());
//		allWithdrawnEstimatesLists.append(QList<double>());
//		allOutstandingEstimatesTotals.append(0);
//		allCompletedEstimatesTotals.append(0);
//		allWithdrawnEstimatesTotals.append(0);

//		allComplexityMappedCompetedWorkLists.append(QList<double>());
//		allReportedCompetedWorkLists.append(QList<double>());
//		allComplexityMappedCompetedWorkTotals.append(0);
//		allReportedCompetedWorkTotals.append(0);
//	}


//	for(int x = 0, xSize = workingIssueFamilies.count(); x < xSize; x++){
//		for(int y = 0, ySize = dateList.count(); y < ySize; y++){
//			allOutstandingEstimatesLists[y].append(complexityManager->outstandingEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));
//			allCompletedEstimatesLists[y].append(complexityManager->completedEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));
//			allWithdrawnEstimatesLists[y].append(complexityManager->withdrawnEstimateAtDate(workingIssueFamilies.at(x), dateList.at(y)));

//			allComplexityMappedCompetedWorkLists[y].append(complexityManager->complexityMappedCompletedWorkAtDate(workingIssueFamilies.at(x), dateList.at(y)));
//			allReportedCompetedWorkLists[y].append(complexityManager->reportedCompletedWorkAtDate(workingIssueFamilies.at(x), dateList.at(y)));
//		}
//	}

//	for(int x = 0, xSize = dateList.count(); x < xSize; x++){
//		for(int y = 0, ySize = allOutstandingEstimatesLists.at(x).count(); y < ySize; y++)
//			allOutstandingEstimatesTotals[x] = allOutstandingEstimatesTotals.at(x)+allOutstandingEstimatesLists.at(x).at(y);
//		for(int y = 0, ySize = allCompletedEstimatesLists.at(x).count(); y < ySize; y++)
//			allCompletedEstimatesTotals[x] = allCompletedEstimatesTotals.at(x)+allCompletedEstimatesLists.at(x).at(y);
//		for(int y = 0, ySize = allWithdrawnEstimatesLists.at(x).count(); y < ySize; y++)
//			allWithdrawnEstimatesTotals[x] = allWithdrawnEstimatesTotals.at(x)+allWithdrawnEstimatesLists.at(x).at(y);

//		for(int y = 0, ySize = allComplexityMappedCompetedWorkLists.at(x).count(); y < ySize; y++)
//			allComplexityMappedCompetedWorkTotals[x] = allComplexityMappedCompetedWorkTotals.at(x)+allComplexityMappedCompetedWorkLists.at(x).at(y);
//		for(int y = 0, ySize = allReportedCompetedWorkLists.at(x).count(); y < ySize; y++)
//			allReportedCompetedWorkTotals[x] = allReportedCompetedWorkTotals.at(x)+allReportedCompetedWorkLists.at(x).at(y);
//	}

//	QList<double> weeklyEstimatedVelocity;
//	QList<double> weeklyComplexityMappedVelocity;
//	QList<double> weeklyReportedVelocity;

//	weeklyEstimatedVelocity.append(0);
//	weeklyComplexityMappedVelocity.append(0);
//	weeklyReportedVelocity.append(0);
//	for(int x = 1, size = dateList.count(); x < size; x++){
//		weeklyEstimatedVelocity.append(allCompletedEstimatesTotals.at(x-1)-allCompletedEstimatesTotals.at(x));
//		weeklyComplexityMappedVelocity.append(allComplexityMappedCompetedWorkTotals.at(x-1)-allComplexityMappedCompetedWorkTotals.at(x));
//		weeklyReportedVelocity.append(allReportedCompetedWorkTotals.at(x-1)-allReportedCompetedWorkTotals.at(x));
//	}

//	double averageEstimatedVelocityTotal = 0;
//	double averageComplexityMappedVelocityTotal = 0;
//	double averageReportedVelocityTotal = 0;

//	for(int x = 0, size = dateList.count(); x < size; x++){
//		averageEstimatedVelocityTotal += weeklyEstimatedVelocity.at(x);
//		averageComplexityMappedVelocityTotal += weeklyComplexityMappedVelocity.at(x);
//		averageReportedVelocityTotal += weeklyReportedVelocity.at(x);
//	}

//	double averageCalendarEstimatedVelocity  = averageEstimatedVelocityTotal/(dateList.count()-1);
//	double averageCalendarComplexityMappedVelocity = averageComplexityMappedVelocityTotal/(dateList.count()-1);
//	double averageCalendarReportedVelocity = averageReportedVelocityTotal/(dateList.count()-1);
//	double availablePersonWeeks = 48-(3+2+2+4); //10 weeks, 4 full time people, Sheldon away for 3 weeks vacation, Sheldon half time for one month, Darren gone for 2 weeks, Iain didn't start until May
//	double averageAvailabilityEstimatedVelocity = averageEstimatedVelocityTotal/availablePersonWeeks;
//	double averageAvailabilityComplexityMappedVelocity = averageComplexityMappedVelocityTotal/availablePersonWeeks;
//	double averageAvailabilityReportedVelocity = averageReportedVelocityTotal/availablePersonWeeks;

//	qDebug() << "\n\n";
//	qDebug() << "Average Estimated Velocity: " << averageCalendarEstimatedVelocity << "or" << averageAvailabilityEstimatedVelocity;
//	qDebug() << "Average Complexity Mapped Velocity: " << averageCalendarComplexityMappedVelocity << "or" << averageAvailabilityComplexityMappedVelocity;
//	qDebug() << "Average Reported Velocity: " << averageCalendarReportedVelocity << "or" << averageAvailabilityReportedVelocity;
}
*/
