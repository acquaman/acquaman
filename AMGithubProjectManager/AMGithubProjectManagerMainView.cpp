#include "AMGithubProjectManagerMainView.h"

#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QDebug>
#include <QScrollArea>

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMRestAction.h"
#include "actions3/actions/AMGitHubGetIssuesAction.h"
#include "actions3/actions/AMGitHubGetCommentsAction.h"

#include "AMGitHubMilestone.h"
#include "AMGitHubIssueFamilyView.h"
#include "AMGitHubComplexityManager.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotRectangle.h"

AMGithubProjectManagerMainView::AMGithubProjectManagerMainView(QWidget *parent)
	: QWidget(parent)
{
	userNameLineEdit_ = new QLineEdit();
	passwordLineEdit_ = new QLineEdit();
	passwordLineEdit_->setEchoMode(QLineEdit::Password);

	initiateButton_ = new QPushButton("Initiate");
	connect(initiateButton_, SIGNAL(clicked()), this, SLOT(onInitiateButtonClicked()));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(userNameLineEdit_);
	vl->addWidget(passwordLineEdit_);
	vl->addWidget(initiateButton_);

	setLayout(vl);
}

AMGithubProjectManagerMainView::~AMGithubProjectManagerMainView()
{

}

void AMGithubProjectManagerMainView::onInitiateButtonClicked(){
	headerData_ = "token 2f8e7e362e5c0a5ea065255ccfdc369e70f4327b";

	manager_ = new QNetworkAccessManager(this);

	AMGitHubGetIssuesActionInfo *getAllIssuesActionInfo = new AMGitHubGetIssuesActionInfo("acquaman", "acquaman", AMGitHubGetIssuesActionInfo::AllIssues);
	AMGitHubGetIssuesAction *getAllIssuesAction = new AMGitHubGetIssuesAction(getAllIssuesActionInfo, manager_, headerData_, &allIssues_, &allMilestones_);
	connect(getAllIssuesAction, SIGNAL(succeeded()), this, SLOT(onGetAllIssuesActionSucceeded()));
	getAllIssuesAction->start();
}

void AMGithubProjectManagerMainView::onGetAllIssuesActionSucceeded()
{
	QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_.constBegin();
	while (j != allIssues_.constEnd()) {
		if(j.value()->isPullRequest() && allIssues_.contains(j.value()->originatingIssueNumber()))
			j.value()->setOriginatingIssue(allIssues_.value(j.value()->originatingIssueNumber()));

		if( (!j.value()->isPullRequest()) && (j.value()->commentCount() > 0) && (!j.value()->commentsURL().isEmpty()) && (j.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) && (j.value()->issueState() == AMGitHubIssue::ClosedState) ){
			commentURLs_.append(j.value()->commentsURL());
		}
		j++;
	}

	qDebug() << "Try this fake out";
//	QStringList fakeCommentURLs;
//	fakeCommentURLs << "https://api.github.com/repos/acquaman/acquaman/issues/1484/comments";
//	fakeCommentURLs << "https://api.github.com/repos/acquaman/acquaman/issues/1214/comments";
	AMGitHubGetCommentsActionInfo *getAllCommentsActionInfo = new AMGitHubGetCommentsActionInfo("acquaman", "acquaman", commentURLs_);
	AMGitHubGetCommentsAction *getAllCommentsAction = new AMGitHubGetCommentsAction(getAllCommentsActionInfo, manager_, headerData_, &allIssues_);
	connect(getAllCommentsAction, SIGNAL(succeeded()), this, SLOT(onGetAllCommentsActionSucceeded()));
	getAllCommentsAction->start();

//	if(commentURLs_.count() > 0){
//		QString oneCommentURL = commentURLs_.takeFirst();

//		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
//		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneIssueCommentsAction->start();
//	}
}

void AMGithubProjectManagerMainView::onGetOneIssueCommentsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
}
//	int lastPageNumber = -1;
//	int nextPageNumber = -1;
//	QString nextPageURL;
//	for(int x = 0; x < headerPairs.count(); x++){
//		if(headerPairs.at(x).first == "Link"){
//			QString linkHeader = headerPairs.at(x).second;
//			QStringList linkHeaderItems = linkHeader.split(',');
//			for(int y = 0; y < linkHeaderItems.count(); y++){
//				if(linkHeaderItems.at(y).contains("; rel=\"last\"")){
//					lastPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
//				}
//				if(linkHeaderItems.at(y).contains("; rel=\"next\"")){
//					nextPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
//					nextPageURL = linkHeaderItems.at(y).section(';', 0, 0).remove('<').remove('>');
//					qDebug() << nextPageURL;
//				}
//			}
//		}
//	}

//	QVariantList githubListReply = fullResponse.toList();
//	int issueNumber = githubListReply.at(0).toMap().value("issue_url").toString().section("/", -1, -1).toInt();
//	qDebug() << "Checking Issue " << issueNumber << "(" << commentURLs_.count() << " comments remaining to fetch)";
//	for(int x = 0, size = githubListReply.count(); x < size; x++){
//		if(githubListReply.at(x).toMap().value("body").toString().contains("Time Estimate:")){
//			QStringList splitCommentBody = githubListReply.at(x).toMap().value("body").toString().split("\n");
//			for(int y = 0, ySize = splitCommentBody.count(); y < ySize; y++){
//				if(splitCommentBody.at(y).contains("Time Estimate:")){
//					QString timeEstimateString = splitCommentBody.at(y);
//					timeEstimateString.remove("Time Estimate:");
//					timeEstimateString = timeEstimateString.simplified();
//					if(timeEstimateString.endsWith("."))
//						timeEstimateString.remove(timeEstimateString.count()-1, 1);

//					if(allIssues_.contains(issueNumber))
//						allIssues_.value(issueNumber)->setTimeEstimateString(timeEstimateString);
//				}
//			}
//		}
//	}

//	if(nextPageNumber != -1){
//		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(nextPageURL, AMRestActionInfo::GetRequest);
//		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneIssueCommentsAction->start();
//	}
//	else if(commentURLs_.count() > 0){
//		QString oneCommentURL = commentURLs_.takeFirst();

//		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
//		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneIssueCommentsAction->start();
//	}
//	else{

void AMGithubProjectManagerMainView::onGetAllCommentsActionSucceeded(){

	QMap<int, AMGitHubIssue*>::const_iterator h = allIssues_.constBegin();
	while(h != allIssues_.constEnd()){
		if(h.value()->timeEstimateString().isNull())
			qDebug() << h.value()->issueNumber() << " has no time estimate";
		else
			qDebug() << h.value()->issueNumber() << " time estimate: " << h.value()->timeEstimateString();
		h++;
	}
	return;

		QMap<int, AMGitHubIssue*>::const_iterator i = allIssues_.constBegin();
		while (i != allIssues_.constEnd()) {
			if(!i.value()->isPullRequest() && i.value()->inlineIssue()){
				AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(i.value(), i.value());
				allIssueFamilies_.insert(i.value()->issueNumber(), oneIssueFamily);
			}
			else if(!i.value()->isPullRequest() && !i.value()->projectTrackingDisabled()){
				AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(i.value(), 0);
				allIssueFamilies_.insert(i.value()->issueNumber(), oneIssueFamily);
			}
			else if(!i.value()->isPullRequest() && (i.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) && (i.value()->timeEstimateString() != "Invalid Time Estimate") && (!i.value()->timeEstimateString().isEmpty()))
				qDebug() << "Issue " << i.value()->issueNumber() << " may be an inline issue";
			i++;
		}

		QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_.constBegin();
		while (j != allIssues_.constEnd()) {
			if(j.value()->isPullRequest() && allIssueFamilies_.contains(j.value()->originatingIssueNumber()))
				allIssueFamilies_.value(j.value()->originatingIssueNumber())->setPullRequestIssue(j.value());

			j++;
		}

		QList<int> totallyUnspecifiedIssues;
		QList<int> fullySpecifiedIssues;
		QMap<int, AMGitHubIssueFamily*>::const_iterator k = allIssueFamilies_.constBegin();
		while(k != allIssueFamilies_.constEnd()){
			if(k.value()->totallyIncomplete())
				totallyUnspecifiedIssues.append(k.value()->originatingIssueNumber());
			if(k.value()->completeInfo())
				fullySpecifiedIssues.append(k.value()->originatingIssueNumber());
			k++;
		}
		for(int x = 0, size = totallyUnspecifiedIssues.size(); x < size; x++)
			allIssueFamilies_.remove(totallyUnspecifiedIssues.at(x));
		for(int x = 0, size = fullySpecifiedIssues.size(); x < size; x++)
			fullySpecifiedIssueFamilies_.insert(fullySpecifiedIssues.at(x), allIssueFamilies_.value(fullySpecifiedIssues.at(x)));


		QWidget *familiesMasterWidget = new QWidget();
		QVBoxLayout *familiesMasterVL = new QVBoxLayout();
		QScrollArea *familiesScrollArea = new QScrollArea();
		QWidget *familiesWidget = new QWidget();
		QVBoxLayout *familiesVL = new QVBoxLayout();
		qDebug() << "\n\n\n\n";
		QMap<int, AMGitHubIssueFamily*>::const_iterator m = allIssueFamilies_.constBegin();
		while(m != allIssueFamilies_.constEnd()){
			bool printIt = true;
			if(m.value()->completeInfo())
				printIt = false;
			if(m.value()->totallyIncomplete())
				printIt = false;
			if(m.value()->onlyMissingActualComplexity())
				printIt = false;
			if(m.value()->onlyMissingTimeEstimate())
				printIt = false;
			if(printIt)
				qDebug() << m.value()->multiLineDebugInfo();
			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(m.value());
			familiesVL->addWidget(oneFamilyView);
			m++;
		}

		familiesWidget->setLayout(familiesVL);
		familiesScrollArea->setWidget(familiesWidget);

		familiesMasterVL->addWidget(familiesScrollArea);
		familiesMasterWidget->setLayout(familiesMasterVL);
		familiesMasterWidget->show();

		qDebug() << "\n\n\n\n";
		QMap<int, AMGitHubIssueFamily*>::const_iterator o = allIssueFamilies_.constBegin();
		while(o != allIssueFamilies_.constEnd()){
			if(o.value()->onlyMissingActualComplexity())
				qDebug() << "Need to do actual complexity for:\n" << o.value()->multiLineDebugInfo() << "\n";
			o++;
		}

		qDebug() << "\n\n\n\n";
		QMap<int, AMGitHubIssueFamily*>::const_iterator p = allIssueFamilies_.constBegin();
		while(p != allIssueFamilies_.constEnd()){
			if(p.value()->onlyMissingTimeEstimate())
				qDebug() << p.value()->originatingIssue()->assignee() << " needs to do time estimate for: " << p.value()->originatingIssueNumber() << "\n";
			p++;
		}

		int totallyCompleteIssues = 0;
		int totallyIncompleteIssues = 0;
		int onlyMissingActualComplexityIssues = 0;
		int onlyMissingTimeEstimateIssues = 0;

		qDebug() << "\n\n\n\n";
		QMap<int, AMGitHubIssueFamily*>::const_iterator q = allIssueFamilies_.constBegin();
		while(q != allIssueFamilies_.constEnd()){
			if(q.value()->completeInfo())
				totallyCompleteIssues++;
			if(q.value()->totallyIncomplete())
				totallyIncompleteIssues++;
			if(q.value()->onlyMissingActualComplexity())
				onlyMissingActualComplexityIssues++;
			if(q.value()->onlyMissingTimeEstimate())
				onlyMissingTimeEstimateIssues++;
			q++;
		}
		qDebug() << totallyCompleteIssues << " of " << allIssueFamilies_.count() << " are fully specfied";
		qDebug() << totallyIncompleteIssues << " of " << allIssueFamilies_.count() << " are totally unspecified";
		qDebug() << onlyMissingActualComplexityIssues << " of " << allIssueFamilies_.count() << " are only missing an actual complexity";
		qDebug() << onlyMissingTimeEstimateIssues << " of " << allIssueFamilies_.count() << " are only missing a time estimate";

		qDebug() << "\n\n\n\n";

		AMGitHubComplexityManager *complexityManager = new AMGitHubComplexityManager();

		QMap<int, AMGitHubIssueFamily*>::const_iterator r = fullySpecifiedIssueFamilies_.constBegin();
		while(r != fullySpecifiedIssueFamilies_.constEnd()){
			complexityManager->incrementComplexityMapping(r.value()->complexityMapping());
			if(r.value()->normalizedTimeEstiamte() > 0){
				complexityManager->addTimeEstimateForEstimatedComplexity(r.value()->normalizedTimeEstiamte(), r.value()->estimatedComplexity());
				complexityManager->addTimeEstimateForActualComplexity(r.value()->normalizedTimeEstiamte(), r.value()->actualComplexity());
			}
			else
				qDebug() << "Issue " << r.value()->originatingIssueNumber() << " cannot normalize " << r.value()->timeEstimate();

			r++;
		}

//		QString debugString;
//		QString debugStringQuantized;
//		for(int x = 1, xSize = complexityMappingMatrix.count(); x < xSize; x++){
//			double percentValue = ((double)complexityMappingMatrix.at(x))/((double)allIssueFamilies_.count())*100;
//			debugString.append(QString("%1\t").arg(percentValue, 0, 'f', 2));
//			debugStringQuantized.append(QString("%1\t").arg(complexityMappingMatrix.at(x)));
//			if(x%5 == 0){
//				debugString.append("\n");
//				debugStringQuantized.append("\n");
//			}
//		}
//		qDebug() << debugString;
//		qDebug() << debugStringQuantized;

		qDebug() << "Average Estimated Complexity 1 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "Average Estimated Complexity 2 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "Average Estimated Complexity 3 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "Average Estimated Complexity 5 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "Average Estimated Complexity 8 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity8);
		qDebug() << "Average Estimated Complexity K time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityK);

		qDebug() << "Average Actual Complexity 1 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "Average Actual Complexity 2 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "Average Actual Complexity 3 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "Average Actual Complexity 5 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "Average Actual Complexity 8 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity8);

		qDebug() << "Probable Estimated Complexity 1 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "Probable Estimated Complexity 2 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "Probable Estimated Complexity 3 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "Probable Estimated Complexity 5 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "Probable Estimated Complexity 8 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity8);

		QMap<int, AMGitHubMilestone*>::const_iterator c = allMilestones_.constBegin();
		while(c != allMilestones_.constEnd()){
//			qDebug() << "Milestone " << c.key() << " named " << c.value()->title() << " is " << c.value()->stateAsString();
			QString associatedIssuesString;
			for(int x = 0, size = c.value()->associatedIssues().count(); x < size; x++){
				associatedIssuesString.append(QString("%1 [%2] \n").arg(c.value()->associatedIssues().at(x)->issueNumber()).arg(AMGitHubIssue::stringFromState(c.value()->associatedIssues().at(x)->issueState())));
				if(!c.value()->associatedIssues().at(x)->isPullRequest() && allIssueFamilies_.contains(c.value()->associatedIssues().at(x)->issueNumber())){
//					qDebug() << c.value()->associatedIssues().at(x)->issueNumber() << " is not a pull request, log it";
					c.value()->appendAssociatedFamily(allIssueFamilies_.value(c.value()->associatedIssues().at(x)->issueNumber()));
				}
//				else if(c.value()->associatedIssues().at(x)->isPullRequest())
//					qDebug() << c.value()->associatedIssues().at(x)->issueNumber() << " is a pull request, ignore it";
//				qDebug() << "Current associated families size: " << c.value()->associatedFamilies().count();
			}
//			if(!associatedIssuesString.isEmpty())
//				qDebug() << associatedIssuesString << "\n";
			c++;
		}

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

//		for(int x = 0, size = bioxasSideMilestone->associatedFamilies().count(); x < size; x++){
		for(int x = 0, size = workingIssueFamilies.count(); x < size; x++){
//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(bioxasSideMilestone->associatedFamilies().at(x));
//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(workingIssueFamilies.at(x));
//			bioXASSideVL->addWidget(oneFamilyView);

//			qDebug() << "Issue " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber() << " created at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->createdDate() << " and closed at " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->closedDate();

//			qDebug() << "Family " << x << " orginates from " << bioxasSideMilestone->associatedFamilies().at(x)->originatingIssueNumber();


			qDebug() << "About to look into " << x << workingIssueFamilies.at(x)->originatingIssueNumber();
			if(workingIssueFamilies.at(x)->estimatedComplexity() != AMGitHubIssue::InvalidComplexity){
				normalizedEstimatedList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
				if(workingIssueFamilies.at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
					normalizedOpenList.append(complexityManager->probableTimeForEstimatedComplexity(workingIssueFamilies.at(x)->estimatedComplexity()));
			}
			if(workingIssueFamilies.at(x)->actualComplexity() != AMGitHubIssue::InvalidComplexity){
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


//		QString eventsString = QString("https://api.github.com/repos/acquaman/acquaman/issues/1315/events");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1176/pipelines");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1177/estimates?repo=acquaman&issue_number=1177&organization=acquaman");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1276/pipelines");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1462/estimates?repo=acquaman&issue_number=1462&organization=acquaman");
//		QString eventsString = QString("https://api.github.com/repos/acquaman/acquaman/issues/1462/labels");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1462/estimates?repo=acquaman&issue_number=1462&estimate_value=3&organization=acquaman");
		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1458/estimates");
//		QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/board");

//		AMRestActionInfo *getOneIssueEventsActionInfo = new AMRestActionInfo(eventsString, AMRestActionInfo::GetRequest);
		AMRestActionInfo *getOneIssueEventsActionInfo = new AMRestActionInfo(eventsString, AMRestActionInfo::PutRequest);
		getOneIssueEventsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
		QString xauthtokenData = "ced5ec812ee0ee5576da1345550d0110ba65946262541a1db6030ebda5dd4c0d95b31d24d954c563";
		getOneIssueEventsActionInfo->setRawHeader("x-authentication-token", xauthtokenData.toLocal8Bit());
		getOneIssueEventsActionInfo->setContentType(AMRestActionInfo::FormURLEncoded);
		getOneIssueEventsActionInfo->setRequestData(QString("repo=acquaman&issue_number=1458&estimate_value=3&organization=acquaman").toAscii());

//		getOneIssueEventsActionInfo->setRawHeader("Content-Type", QString("application/x-www-form-urlencoded").toLocal8Bit());
//		getOneIssueEventsActionInfo->setRawHeader("Content-Length", QString("70").toLocal8Bit());
		AMRestAction *getOneIssueEventsAction = new AMRestAction(getOneIssueEventsActionInfo, manager_);

		connect(getOneIssueEventsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueEventsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneIssueEventsAction->start();

	}
//}

void AMGithubProjectManagerMainView::onGetOneIssueEventsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs){
	qDebug() << "Got the pipeline for 1462 info back";

	qDebug() << "\n\n" << fullResponse;

//	QVariantList responseList = fullResponse.toList();
//	for(int x = 0, size = responseList.count(); x < size; x++){
////		qDebug() << "At" << x;
////		qDebug() << responseList.at(x);
////		qDebug() << "\n";
//		QVariantMap jsonMap = responseList.at(x).toMap();
//		qDebug() << "At " << x << "event type" << jsonMap.value("event").toString();
//	}

	/* QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1276/pipelines");
	QVariantList responseList = fullResponse.toList();
	for(int x = 0, size = responseList.count(); x < size; x++){
		qDebug() << "At" << x << responseList.at(x);
	}
	*/

	/* QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/1276/pipelines"); */
	QVariantList responseList = fullResponse.toList();
	for(int x = 0, size = responseList.count(); x < size; x++){
		qDebug() << "At" << x << responseList.at(x);
	}
	/**/

	/* QString eventsString = QString("https://api.zenhub.io/v2/acquaman/acquaman/board");
	QVariantMap jsonMap = fullResponse.toMap();
	QMap<QString, QVariant>::const_iterator j = jsonMap.constBegin();
	while (j != jsonMap.constEnd()) {
		if(j.key() == "pipelines"){
			qDebug() << "PIPELINES:";
			QVariantList pipelinesList = j.value().toList();
			for(int x = 0, xSize = pipelinesList.count(); x < xSize; x++){
				QVariantMap innerMap = pipelinesList.at(x).toMap();
				QMap<QString, QVariant>::const_iterator k = innerMap.constBegin();
				while (k != innerMap.constEnd()) {
					if(k.key() == "issues"){
						qDebug() << "\tISSUES for: " << innerMap.value("name").toString();
						QVariantList issuesList = k.value().toList();
						for(int y = 0, ySize = issuesList.count(); y < ySize; y++){
							QVariantMap issuesMap = issuesList.at(y).toMap();
							QMap<QString, QVariant>::const_iterator l = issuesMap.constBegin();
							while (l != issuesMap.constEnd()) {
								qDebug() << "\t\t" << l.key() << l.value();
								l++;
							}
						}
					}
					else
						qDebug() << "\t" << k.key() << k.value();
					k++;
				}
			}
		}
		else
			qDebug() << j.key() << j.value();
		j++;
	}
	*/
}
