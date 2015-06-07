#include "AMGithubProjectManagerMainView.h"

#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QDebug>
#include <QScrollArea>

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMRestAction.h"

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
	QString userInfo = userNameLineEdit_->text()+":"+passwordLineEdit_->text();
	QByteArray userData = userInfo.toLocal8Bit().toBase64();
	headerData_ = "Basic " + userData;

	manager_ = new QNetworkAccessManager(this);

	currentClosedIssuesPage_ = 1;
	lastPage_ = false;
	QString issuesString = QString("https://api.github.com/repos/acquaman/acquaman/issues?filter=all&state=all&page=%1&per_page=30").arg(currentClosedIssuesPage_);
	AMRestActionInfo *getAllClosedIssuesActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
	getAllClosedIssuesActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
	getAllClosedIssuesAction_ = new AMRestAction(getAllClosedIssuesActionInfo, manager_);

	connect(getAllClosedIssuesAction_, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllClosedActionsFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
	getAllClosedIssuesAction_->start();
}

void AMGithubProjectManagerMainView::onGetAllClosedActionsFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs){
	QVariant allClosedIssuesResponse = fullResponse;

	int lastPageNumber = -1;
	int nextPageNumber = -1;
	for(int x = 0; x < headerPairs.count(); x++){
		if(headerPairs.at(x).first == "Link"){
			QString linkHeader = headerPairs.at(x).second;
			QStringList linkHeaderItems = linkHeader.split(',');
			for(int y = 0; y < linkHeaderItems.count(); y++){
				if(linkHeaderItems.at(y).contains("; rel=\"last\""))
					lastPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
				if(linkHeaderItems.at(y).contains("; rel=\"next\""))
					nextPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
			}
		}
	}

	if(allClosedIssuesResponse.canConvert(QVariant::List)){
		QVariantMap jsonMap;
		QVariantList githubListReply = allClosedIssuesResponse.toList();
		if(githubListReply.at(0).canConvert(QVariant::Map)){
			for(int x = 0; x < githubListReply.count(); x++){
				jsonMap = githubListReply.at(x).toMap();

				AMGitHubMilestone *associatedMilestone = 0;
				if(jsonMap.contains("milestone") && !jsonMap.value("milestone").toMap().value("title").toString().isEmpty()){
					int milestoneNumber = -1;
					if(jsonMap.value("milestone").toMap().value("number").canConvert<int>())
						milestoneNumber = jsonMap.value("milestone").toMap().value("number").toInt();
					if(milestoneNumber > 0 && !allMilestones_.contains(milestoneNumber)){
						AMGitHubMilestone *oneMilestone = new AMGitHubMilestone(jsonMap.value("milestone").toMap());
						allMilestones_.insert(oneMilestone->number(), oneMilestone);

						associatedMilestone = oneMilestone;
					}
					else if(milestoneNumber > 0 && allMilestones_.contains(milestoneNumber))
						associatedMilestone = allMilestones_.value(milestoneNumber);
				}

				AMGitHubIssue *oneIssue = new AMGitHubIssue(jsonMap);

				allIssues_.insert(oneIssue->issueNumber(), oneIssue);

				if(associatedMilestone)
					associatedMilestone->appendAssociatedIssue(oneIssue);
			}
		}
	}

	if(!lastPage_){
		currentClosedIssuesPage_ = nextPageNumber;
		if(nextPageNumber == lastPageNumber)
			lastPage_ = true;

//		if(nextPageNumber == 5)
//			lastPage_ = true;

		qDebug() << "More more issues to fetch, going to page " << currentClosedIssuesPage_;

		QString issuesString = QString("https://api.github.com/repos/acquaman/acquaman/issues?filter=all&state=all&page=%1&per_page=30").arg(currentClosedIssuesPage_);
		AMRestActionInfo *getOneClosedIssuesPageActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
		getOneClosedIssuesPageActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
		AMRestAction *getOneClosedIssuesPageAction = new AMRestAction(getOneClosedIssuesPageActionInfo, manager_);

		connect(getOneClosedIssuesPageAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllClosedActionsFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
		getOneClosedIssuesPageAction->start();
	}
	else{

		QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_.constBegin();
		while (j != allIssues_.constEnd()) {
			if(j.value()->isPullRequest() && allIssues_.contains(j.value()->originatingIssueNumber()))
				j.value()->setOriginatingIssue(allIssues_.value(j.value()->originatingIssueNumber()));

			if( (!j.value()->isPullRequest()) && (j.value()->commentCount() > 0) && (!j.value()->commentsURL().isEmpty()) && (j.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) ){
				commentURLs_.append(j.value()->commentsURL());
			}
			j++;
		}

		if(commentURLs_.count() > 0){
			QString oneCommentURL = commentURLs_.takeFirst();

			AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
			getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
			AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

			connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
			getOneIssueCommentsAction->start();
		}
	}
}

void AMGithubProjectManagerMainView::onGetOneIssueCommentsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	int lastPageNumber = -1;
	int nextPageNumber = -1;
	QString nextPageURL;
	for(int x = 0; x < headerPairs.count(); x++){
		if(headerPairs.at(x).first == "Link"){
			QString linkHeader = headerPairs.at(x).second;
			QStringList linkHeaderItems = linkHeader.split(',');
			for(int y = 0; y < linkHeaderItems.count(); y++){
				if(linkHeaderItems.at(y).contains("; rel=\"last\"")){
					lastPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
				}
				if(linkHeaderItems.at(y).contains("; rel=\"next\"")){
					nextPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
					nextPageURL = linkHeaderItems.at(y).section(';', 0, 0).remove('<').remove('>');
					qDebug() << nextPageURL;
				}
			}
		}
	}

	QVariantList githubListReply = fullResponse.toList();
	int issueNumber = githubListReply.at(0).toMap().value("issue_url").toString().section("/", -1, -1).toInt();
	qDebug() << "Checking Issue " << issueNumber << "(" << commentURLs_.count() << " comments remaining to fetch)";
	for(int x = 0, size = githubListReply.count(); x < size; x++){
		if(githubListReply.at(x).toMap().value("body").toString().contains("Time Estimate:")){
			QStringList splitCommentBody = githubListReply.at(x).toMap().value("body").toString().split("\n");
			for(int y = 0, ySize = splitCommentBody.count(); y < ySize; y++){
				if(splitCommentBody.at(y).contains("Time Estimate:")){
					QString timeEstimateString = splitCommentBody.at(y);
					timeEstimateString.remove("Time Estimate:");
					timeEstimateString = timeEstimateString.simplified();
					if(timeEstimateString.endsWith("."))
						timeEstimateString.remove(timeEstimateString.count()-1, 1);

					if(allIssues_.contains(issueNumber))
						allIssues_.value(issueNumber)->setTimeEstimateString(timeEstimateString);
				}
			}
		}
	}

	if(nextPageNumber != -1){
		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(nextPageURL, AMRestActionInfo::GetRequest);
		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
		getOneIssueCommentsAction->start();
	}
	else if(commentURLs_.count() > 0){
		QString oneCommentURL = commentURLs_.takeFirst();

		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
		getOneIssueCommentsAction->start();
	}
	else{

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
		QVector<int> complexityMappingMatrix = QVector<int>(31, 0);
		QList<double> estimatedComplexity1Times;
		QList<double> estimatedComplexity2Times;
		QList<double> estimatedComplexity3Times;
		QList<double> estimatedComplexity5Times;
		QList<double> estimatedComplexity8Times;
		QList<double> estimatedComplexityKTimes;
		QList<double> actualComplexity1Times;
		QList<double> actualComplexity2Times;
		QList<double> actualComplexity3Times;
		QList<double> actualComplexity5Times;
		QList<double> actualComplexity8Times;

		AMGitHubComplexityManager *complexityManager = new AMGitHubComplexityManager();

//		QMap<int, AMGitHubIssueFamily*>::const_iterator r = allIssueFamilies_.constBegin();
		QMap<int, AMGitHubIssueFamily*>::const_iterator r = fullySpecifiedIssueFamilies_.constBegin();
//		while(r != allIssueFamilies_.constEnd()){
		while(r != fullySpecifiedIssueFamilies_.constEnd()){
			complexityMappingMatrix[r.value()->complexityMapping()] = complexityMappingMatrix.at(r.value()->complexityMapping())+1;

			if(r.value()->normalizedTimeEstiamte() > 0){
				switch(r.value()->estimatedComplexity()){
				case AMGitHubIssue::Complexity1:
					estimatedComplexity1Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity2:
					estimatedComplexity2Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity3:
					estimatedComplexity3Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity5:
					estimatedComplexity5Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity8:
					estimatedComplexity8Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::ComplexityK:
					estimatedComplexityKTimes.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::InvalidComplexity:
					break;
				}

				switch(r.value()->actualComplexity()){
				case AMGitHubIssue::Complexity1:
					actualComplexity1Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity2:
					actualComplexity2Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity3:
					actualComplexity3Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity5:
					actualComplexity5Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::Complexity8:
					actualComplexity8Times.append(r.value()->normalizedTimeEstiamte());
					break;
				case AMGitHubIssue::ComplexityK:
					break;
				case AMGitHubIssue::InvalidComplexity:
					break;
				}

				complexityManager->incrementComplexityMapping(r.value()->complexityMapping());
				complexityManager->addTimeEstimateForEstimatedComplexity(r.value()->normalizedTimeEstiamte(), r.value()->estimatedComplexity());
				complexityManager->addTimeEstimateForActualComplexity(r.value()->normalizedTimeEstiamte(), r.value()->actualComplexity());
			}
			else
				qDebug() << "Issue " << r.value()->originatingIssueNumber() << " cannot normalize " << r.value()->timeEstimate();

			r++;
		}

		QString debugString;
		QString debugStringQuantized;
		for(int x = 1, xSize = complexityMappingMatrix.count(); x < xSize; x++){
			double percentValue = ((double)complexityMappingMatrix.at(x))/((double)allIssueFamilies_.count())*100;
			debugString.append(QString("%1\t").arg(percentValue, 0, 'f', 2));
			debugStringQuantized.append(QString("%1\t").arg(complexityMappingMatrix.at(x)));
			if(x%5 == 0){
				debugString.append("\n");
				debugStringQuantized.append("\n");
			}
		}
		qDebug() << debugString;
		qDebug() << debugStringQuantized;

		double averageEstimatedComplexity1Time = 0;
		double averageEstimatedComplexity2Time = 0;
		double averageEstimatedComplexity3Time = 0;
		double averageEstimatedComplexity5Time = 0;
		double averageEstimatedComplexity8Time = 0;
		double averageEstimatedComplexityKTime = 0;
		double averageActualComplexity1Time = 0;
		double averageActualComplexity2Time = 0;
		double averageActualComplexity3Time = 0;
		double averageActualComplexity5Time = 0;
		double averageActualComplexity8Time = 0;

		for(int x = 0, size = estimatedComplexity1Times.count(); x < size; x++)
			averageEstimatedComplexity1Time += estimatedComplexity1Times.at(x);
		averageEstimatedComplexity1Time = averageEstimatedComplexity1Time/((double)estimatedComplexity1Times.count());

		for(int x = 0, size = estimatedComplexity2Times.count(); x < size; x++)
			averageEstimatedComplexity2Time += estimatedComplexity2Times.at(x);
		averageEstimatedComplexity2Time = averageEstimatedComplexity2Time/((double)estimatedComplexity2Times.count());

		for(int x = 0, size = estimatedComplexity3Times.count(); x < size; x++)
			averageEstimatedComplexity3Time += estimatedComplexity3Times.at(x);
		averageEstimatedComplexity3Time = averageEstimatedComplexity3Time/((double)estimatedComplexity3Times.count());

		for(int x = 0, size = estimatedComplexity5Times.count(); x < size; x++)
			averageEstimatedComplexity5Time += estimatedComplexity5Times.at(x);
		averageEstimatedComplexity5Time = averageEstimatedComplexity5Time/((double)estimatedComplexity5Times.count());

		for(int x = 0, size = estimatedComplexity8Times.count(); x < size; x++)
			averageEstimatedComplexity8Time += estimatedComplexity8Times.at(x);
		averageEstimatedComplexity8Time = averageEstimatedComplexity8Time/((double)estimatedComplexity8Times.count());

		for(int x = 0, size = estimatedComplexityKTimes.count(); x < size; x++)
			averageEstimatedComplexityKTime += estimatedComplexityKTimes.at(x);
		averageEstimatedComplexityKTime = averageEstimatedComplexityKTime/((double)estimatedComplexityKTimes.count());


		for(int x = 0, size = actualComplexity1Times.count(); x < size; x++)
			averageActualComplexity1Time += actualComplexity1Times.at(x);
		averageActualComplexity1Time = averageActualComplexity1Time/((double)actualComplexity1Times.count());

		for(int x = 0, size = actualComplexity2Times.count(); x < size; x++)
			averageActualComplexity2Time += actualComplexity2Times.at(x);
		averageActualComplexity2Time = averageActualComplexity2Time/((double)actualComplexity2Times.count());

		for(int x = 0, size = actualComplexity3Times.count(); x < size; x++)
			averageActualComplexity3Time += actualComplexity3Times.at(x);
		averageActualComplexity3Time = averageActualComplexity3Time/((double)actualComplexity3Times.count());

		for(int x = 0, size = actualComplexity5Times.count(); x < size; x++)
			averageActualComplexity5Time += actualComplexity5Times.at(x);
		averageActualComplexity5Time = averageActualComplexity5Time/((double)actualComplexity5Times.count());

		for(int x = 0, size = actualComplexity8Times.count(); x < size; x++)
			averageActualComplexity8Time += actualComplexity8Times.at(x);
		if(actualComplexity8Times.count() != 0)
			averageActualComplexity8Time = averageActualComplexity8Time/((double)actualComplexity8Times.count());

		qDebug() << "Average Estimated Complexity 1 time estimate: " << averageEstimatedComplexity1Time;
		qDebug() << "Average Estimated Complexity 2 time estimate: " << averageEstimatedComplexity2Time;
		qDebug() << "Average Estimated Complexity 3 time estimate: " << averageEstimatedComplexity3Time;
		qDebug() << "Average Estimated Complexity 5 time estimate: " << averageEstimatedComplexity5Time;
		qDebug() << "Average Estimated Complexity 8 time estimate: " << averageEstimatedComplexity8Time;
		qDebug() << "Average Estimated Complexity K time estimate: " << averageEstimatedComplexityKTime;

		qDebug() << "ALTERNATE Average Estimated Complexity 1 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "ALTERNATE Average Estimated Complexity 2 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "ALTERNATE Average Estimated Complexity 3 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "ALTERNATE Average Estimated Complexity 5 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "ALTERNATE Average Estimated Complexity 8 time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::Complexity8);
		qDebug() << "ALTERNATE Average Estimated Complexity K time estimate: " << complexityManager->averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityK);

		qDebug() << "Average Actual Complexity 1 time estimate: " << averageActualComplexity1Time;
		qDebug() << "Average Actual Complexity 2 time estimate: " << averageActualComplexity2Time;
		qDebug() << "Average Actual Complexity 3 time estimate: " << averageActualComplexity3Time;
		qDebug() << "Average Actual Complexity 5 time estimate: " << averageActualComplexity5Time;
		qDebug() << "Average Actual Complexity 8 time estimate: " << averageActualComplexity8Time;

		qDebug() << "ALTERNATE Average Actual Complexity 1 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "ALTERNATE Average Actual Complexity 2 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "ALTERNATE Average Actual Complexity 3 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "ALTERNATE Average Actual Complexity 5 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "ALTERNATE Average Actual Complexity 8 time estimate: " << complexityManager->averageTimeForActualComplexity(AMGitHubIssue::Complexity8);


		int totalEstimatedComplexity1Issues = complexityMappingMatrix.at(1) + complexityMappingMatrix.at(2) + complexityMappingMatrix.at(3) + complexityMappingMatrix.at(4) + complexityMappingMatrix.at(5);
		int totalEstimatedComplexity2Issues = complexityMappingMatrix.at(6) + complexityMappingMatrix.at(7) + complexityMappingMatrix.at(8) + complexityMappingMatrix.at(9) + complexityMappingMatrix.at(10);
		int totalEstimatedComplexity3Issues = complexityMappingMatrix.at(11) + complexityMappingMatrix.at(12) + complexityMappingMatrix.at(13) + complexityMappingMatrix.at(14) + complexityMappingMatrix.at(15);
		int totalEstimatedComplexity5Issues = complexityMappingMatrix.at(16) + complexityMappingMatrix.at(17) + complexityMappingMatrix.at(18) + complexityMappingMatrix.at(19) + complexityMappingMatrix.at(20);
		int totalEstimatedComplexity8Issues = complexityMappingMatrix.at(21) + complexityMappingMatrix.at(22) + complexityMappingMatrix.at(23) + complexityMappingMatrix.at(24) + complexityMappingMatrix.at(25);
//		int totalEstimatedComplexityKIssues = complexityMappingMatrix.at(26) + complexityMappingMatrix.at(27) + complexityMappingMatrix.at(28) + complexityMappingMatrix.at(29) + complexityMappingMatrix.at(30);

		if(totalEstimatedComplexity1Issues == 0)
			totalEstimatedComplexity1Issues = 1;
		if(totalEstimatedComplexity2Issues == 0)
			totalEstimatedComplexity2Issues = 1;
		if(totalEstimatedComplexity3Issues == 0)
			totalEstimatedComplexity3Issues = 1;
		if(totalEstimatedComplexity5Issues == 0)
			totalEstimatedComplexity5Issues = 1;
		if(totalEstimatedComplexity8Issues == 0)
			totalEstimatedComplexity8Issues = 1;
//		if(totalEstimatedComplexityKIssues == 0)
//			totalEstimatedComplexityKIssues = 1;

		double probableEstimatedComplexity1Time = averageActualComplexity1Time*complexityMappingMatrix.at(1)/totalEstimatedComplexity1Issues + averageActualComplexity2Time*complexityMappingMatrix.at(2)/totalEstimatedComplexity1Issues + averageActualComplexity3Time*complexityMappingMatrix.at(3)/totalEstimatedComplexity1Issues + averageActualComplexity5Time*complexityMappingMatrix.at(4)/totalEstimatedComplexity1Issues + averageActualComplexity8Time*complexityMappingMatrix.at(5)/totalEstimatedComplexity1Issues;
		double probableEstimatedComplexity2Time = averageActualComplexity1Time*complexityMappingMatrix.at(6)/totalEstimatedComplexity2Issues + averageActualComplexity2Time*complexityMappingMatrix.at(7)/totalEstimatedComplexity2Issues + averageActualComplexity3Time*complexityMappingMatrix.at(8)/totalEstimatedComplexity2Issues + averageActualComplexity5Time*complexityMappingMatrix.at(9)/totalEstimatedComplexity2Issues + averageActualComplexity8Time*complexityMappingMatrix.at(10)/totalEstimatedComplexity2Issues;
		double probableEstimatedComplexity3Time = averageActualComplexity1Time*complexityMappingMatrix.at(11)/totalEstimatedComplexity3Issues + averageActualComplexity2Time*complexityMappingMatrix.at(12)/totalEstimatedComplexity3Issues + averageActualComplexity3Time*complexityMappingMatrix.at(13)/totalEstimatedComplexity3Issues + averageActualComplexity5Time*complexityMappingMatrix.at(14)/totalEstimatedComplexity3Issues + averageActualComplexity8Time*complexityMappingMatrix.at(15)/totalEstimatedComplexity3Issues;
		double probableEstimatedComplexity5Time = averageActualComplexity1Time*complexityMappingMatrix.at(16)/totalEstimatedComplexity5Issues + averageActualComplexity2Time*complexityMappingMatrix.at(17)/totalEstimatedComplexity5Issues + averageActualComplexity3Time*complexityMappingMatrix.at(18)/totalEstimatedComplexity5Issues + averageActualComplexity5Time*complexityMappingMatrix.at(19)/totalEstimatedComplexity5Issues + averageActualComplexity8Time*complexityMappingMatrix.at(20)/totalEstimatedComplexity5Issues;
		double probableEstimatedComplexity8Time = averageActualComplexity1Time*complexityMappingMatrix.at(21)/totalEstimatedComplexity8Issues + averageActualComplexity2Time*complexityMappingMatrix.at(22)/totalEstimatedComplexity8Issues + averageActualComplexity3Time*complexityMappingMatrix.at(23)/totalEstimatedComplexity8Issues + averageActualComplexity5Time*complexityMappingMatrix.at(24)/totalEstimatedComplexity8Issues + averageActualComplexity8Time*complexityMappingMatrix.at(25)/totalEstimatedComplexity8Issues;
//		double probableEstimatedComplexityKTime = averageActualComplexityKTime*complexityMappingMatrix.at(26)/totalEstimatedComplexityKIssues + averageActualComplexityKTime*complexityMappingMatrix.at(27)/totalEstimatedComplexityKIssues + averageActualComplexityKTime*complexityMappingMatrix.at(28)/totalEstimatedComplexityKIssues + averageActualComplexityKTime*complexityMappingMatrix.at(29)/totalEstimatedComplexityKIssues + averageActualComplexityKTime*complexityMappingMatrix.at(30)/totalEstimatedComplexityKIssues;


		qDebug() << "Probable Estimated Complexity 1 time estimate: " << probableEstimatedComplexity1Time;
		qDebug() << "Probable Estimated Complexity 2 time estimate: " << probableEstimatedComplexity2Time;
		qDebug() << "Probable Estimated Complexity 3 time estimate: " << probableEstimatedComplexity3Time;
		qDebug() << "Probable Estimated Complexity 5 time estimate: " << probableEstimatedComplexity5Time;
		qDebug() << "Probable Estimated Complexity 8 time estimate: " << probableEstimatedComplexity8Time;
//		qDebug() << "Probable Estimated Complexity K time estimate: " << probableEstimatedComplexityKTime;

		qDebug() << "ALTERNATE Probable Estimated Complexity 1 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity1);
		qDebug() << "ALTERNATE Probable Estimated Complexity 2 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity2);
		qDebug() << "ALTERNATE Probable Estimated Complexity 3 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity3);
		qDebug() << "ALTERNATE Probable Estimated Complexity 5 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity5);
		qDebug() << "ALTERNATE Probable Estimated Complexity 8 time estimate: " << complexityManager->probableTimeForEstimatedComplexity(AMGitHubIssue::Complexity8);

		QMap<int, AMGitHubMilestone*>::const_iterator c = allMilestones_.constBegin();
		while(c != allMilestones_.constEnd()){
			qDebug() << "Milestone " << c.key() << " named " << c.value()->title() << " is " << c.value()->stateAsString();
			QString associatedIssuesString;
			for(int x = 0, size = c.value()->associatedIssues().count(); x < size; x++){
				associatedIssuesString.append(QString("%1 [%2] ").arg(c.value()->associatedIssues().at(x)->issueNumber()).arg(AMGitHubIssue::stringFromState(c.value()->associatedIssues().at(x)->issueState())));
				if(!c.value()->associatedIssues().at(x)->isPullRequest() && allIssueFamilies_.contains(c.value()->associatedIssues().at(x)->issueNumber()))
					c.value()->appendAssociatedFamily(allIssueFamilies_.value(c.value()->associatedIssues().at(x)->issueNumber()));
			}
			if(!associatedIssuesString.isEmpty())
				qDebug() << associatedIssuesString;
			c++;
		}

		AMGitHubMilestone *bioxasSideMilestone = allMilestones_.value(10);
		QList<double> normalizedEstimatedList;
		double normalizedEstimated = 0;
		QList<double> normalizedCompletedList;
		double normalizedCompleted = 0;
		QList<double> normalizedValueList;
		double normalizedValue = 0;

		QList<double> normalizedOpenList;
		double normalizedOpen;
		QList<double> normalizedClosedList;
		double normalizedClosed;

		for(int x = 0, size = bioxasSideMilestone->associatedFamilies().count(); x < size; x++){
			if(bioxasSideMilestone->associatedFamilies().at(x)->estimatedComplexity() != AMGitHubIssue::InvalidComplexity){
				AMGitHubIssue::ComplexityValue oneEstimatedComplexity = bioxasSideMilestone->associatedFamilies().at(x)->estimatedComplexity();
				switch (oneEstimatedComplexity) {
				case AMGitHubIssue::Complexity1:
					normalizedEstimatedList.append(probableEstimatedComplexity1Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
						normalizedOpenList.append(probableEstimatedComplexity1Time);
					break;
				case AMGitHubIssue::Complexity2:
					normalizedEstimatedList.append(probableEstimatedComplexity2Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
						normalizedOpenList.append(probableEstimatedComplexity2Time);
					break;
				case AMGitHubIssue::Complexity3:
					normalizedEstimatedList.append(probableEstimatedComplexity3Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
						normalizedOpenList.append(probableEstimatedComplexity3Time);
					break;
				case AMGitHubIssue::Complexity5:
					normalizedEstimatedList.append(probableEstimatedComplexity5Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
						normalizedOpenList.append(probableEstimatedComplexity5Time);
					break;
				case AMGitHubIssue::Complexity8:
					normalizedEstimatedList.append(probableEstimatedComplexity8Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::OpenState)
						normalizedOpenList.append(probableEstimatedComplexity8Time);
					break;
				case AMGitHubIssue::ComplexityK:
					break;
				default:
					break;
				}
			}
			if(bioxasSideMilestone->associatedFamilies().at(x)->estimatedComplexity() != AMGitHubIssue::InvalidComplexity){
				AMGitHubIssue::ComplexityValue oneActualComplexity = bioxasSideMilestone->associatedFamilies().at(x)->actualComplexity();
				switch (oneActualComplexity) {
				case AMGitHubIssue::Complexity1:
					normalizedCompletedList.append(probableEstimatedComplexity1Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
						normalizedClosedList.append(probableEstimatedComplexity1Time);
					break;
				case AMGitHubIssue::Complexity2:
					normalizedCompletedList.append(probableEstimatedComplexity2Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
						normalizedClosedList.append(probableEstimatedComplexity2Time);
					break;
				case AMGitHubIssue::Complexity3:
					normalizedCompletedList.append(probableEstimatedComplexity3Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
						normalizedClosedList.append(probableEstimatedComplexity3Time);
					break;
				case AMGitHubIssue::Complexity5:
					normalizedCompletedList.append(probableEstimatedComplexity5Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
						normalizedClosedList.append(probableEstimatedComplexity5Time);
					break;
				case AMGitHubIssue::Complexity8:
					normalizedCompletedList.append(probableEstimatedComplexity8Time);
					if(bioxasSideMilestone->associatedFamilies().at(x)->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
						normalizedClosedList.append(probableEstimatedComplexity8Time);
					break;
				case AMGitHubIssue::ComplexityK:
					break;
				default:
					break;
				}
			}
			if(bioxasSideMilestone->associatedFamilies().at(x)->normalizedTimeEstiamte() > 0){
				normalizedValueList.append(bioxasSideMilestone->associatedFamilies().at(x)->normalizedTimeEstiamte());
			}
		}

		for(int x = 0, size = normalizedEstimatedList.count(); x < size; x++)
			normalizedEstimated += normalizedEstimatedList.at(x);
		for(int x = 0, size = normalizedCompletedList.count(); x < size; x++)
			normalizedCompleted += normalizedCompletedList.at(x);
		for(int x = 0, size = normalizedValueList.count(); x < size; x++)
			normalizedValue += normalizedValueList.at(x);
		for(int x = 0, size = normalizedOpenList.count(); x < size; x++)
			normalizedOpen += normalizedOpenList.at(x);
		for(int x = 0, size = normalizedClosedList.count(); x < size; x++)
			normalizedClosed += normalizedClosedList.at(x);

		qDebug() << "\n\n\n\n";

		qDebug() << "Normalized Estimated List: " << normalizedEstimatedList;
		qDebug() << "Normalized Completed List: " << normalizedCompletedList;
		qDebug() << "Normalized Value List: " << normalizedValueList;

		qDebug() << "Normalized Estimated Total: " << normalizedEstimated;
		qDebug() << "Normalized Completed Total: " << normalizedCompleted;
		qDebug() << "Normalized Value Total: " << normalizedValue;

		qDebug() << "\n\n";
		qDebug() << "Normalized Open List: " << normalizedOpenList;
		qDebug() << "Normalized Closed List: " << normalizedClosedList;

		qDebug() << "Normalized Open Total: " << normalizedOpen;
		qDebug() << "Normalized Closed Total: " << normalizedClosed;
	}
}
