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
#include "actions3/actions/AMZenHubGetEstimatesAction.h"

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

//	QString removeLabelString = QString("https://api.github.com/repos/%1/%2/issues/%3/labels/%4").arg("acquaman").arg("acquaman").arg(39).arg("Complexity13");
//	AMRestActionInfo *removeOneLabelActionInfo = new AMRestActionInfo(removeLabelString, AMRestActionInfo::DeleteRequest);
//	removeOneLabelActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//	AMRestAction *removeOneLabelAction = new AMRestAction(removeOneLabelActionInfo, manager_);
//	removeOneLabelAction->start();
//	return;

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

	AMGitHubGetCommentsActionInfo *getAllCommentsActionInfo = new AMGitHubGetCommentsActionInfo("acquaman", "acquaman", commentURLs_);
	AMGitHubGetCommentsAction *getAllCommentsAction = new AMGitHubGetCommentsAction(getAllCommentsActionInfo, manager_, headerData_, &allIssues_);
	connect(getAllCommentsAction, SIGNAL(succeeded()), this, SLOT(onGetAllCommentsActionSucceeded()));
	getAllCommentsAction->start();
}

void AMGithubProjectManagerMainView::onGetAllCommentsActionSucceeded(){

	QStringList allEstimateURLs;
	QMap<int, AMGitHubIssue*>::const_iterator h = allIssues_.constBegin();
	while(h != allIssues_.constEnd()){
		if(h.value()->timeEstimateString().isNull())
			qDebug() << h.value()->issueNumber() << " has no time estimate";
		else
			qDebug() << h.value()->issueNumber() << " time estimate: " << h.value()->timeEstimateString();

		if(!h.value()->projectTrackingDisabled() && !h.value()->isPullRequest())
			allEstimateURLs << QString("https://api.zenhub.io/v2/acquaman/acquaman/issues/%1/estimates").arg(h.value()->issueNumber());

		h++;
	}

	AMZenHubGetEstimatesActionInfo *getAllEstimatesActionInfo = new AMZenHubGetEstimatesActionInfo("acquaman", "acquaman", allEstimateURLs);
	AMZenHubGetEstimatesAction *getAllEstimatesAction = new AMZenHubGetEstimatesAction(getAllEstimatesActionInfo, manager_, headerData_, &allIssues_);
	connect(getAllEstimatesAction, SIGNAL(succeeded()), this, SLOT(onGetAllZenhubEstimatesSucceeded()));
	getAllEstimatesAction->start();
}

void AMGithubProjectManagerMainView::onGetAllZenhubEstimatesSucceeded()
{
	qDebug() << "All estimates returned. Start parsing.\n\n\n\n\n";

	QList<int> allTrackableIssuesWithinRange;
	QList<int> closedMissingComplexityLabel;
	QList<int> openMissingComplexityLabelHasEstimate;
	QList<int> openMissingComplexityLabelAndEstimate;
	QList<int> missingZenhubEstimate;
	QList<int> complexityMismatches;
	QList<int> goodIssues;

	QMap<int, AMGitHubIssue*>::const_iterator h = allIssues_.constBegin();
	while(h != allIssues_.constEnd()){
//		if(!h.value()->isPullRequest() && !h.value()->projectTrackingDisabled() && (h.value()->createdDate() < QDateTime::fromString("2015-07-31", "yyyy-MM-dd")) ){
		if(!h.value()->isPullRequest() && !h.value()->projectTrackingDisabled() && (h.value()->createdDate() < QDateTime::fromString("2015-07-05", "yyyy-MM-dd")) ){
			allTrackableIssuesWithinRange.append(h.value()->issueNumber());

			if((h.value()->complexityValue() == AMGitHubIssue::InvalidComplexity) && h.value()->isClosed())
				closedMissingComplexityLabel.append(h.value()->issueNumber());
			else if((h.value()->complexityValue() == AMGitHubIssue::InvalidComplexity) && (h.value()->zenhubComplexityValue() == AMGitHubIssue::ZenhubComplexityInvalid) && h.value()->isOpen())
				openMissingComplexityLabelAndEstimate.append(h.value()->issueNumber());
			else if((h.value()->complexityValue() == AMGitHubIssue::InvalidComplexity) && h.value()->isOpen())
				openMissingComplexityLabelHasEstimate.append(h.value()->issueNumber());
			else if(h.value()->zenhubComplexityValue() == AMGitHubIssue::ZenhubComplexityInvalid)
				missingZenhubEstimate.append(h.value()->issueNumber());
			else if(AMGitHubIssue::integerFromComplexity(h.value()->complexityValue()) != AMGitHubIssue::integerFromZenhubComplexityValue(h.value()->zenhubComplexityValue()))
				complexityMismatches.append(h.value()->issueNumber());
			else if(AMGitHubIssue::integerFromComplexity(h.value()->complexityValue()) == AMGitHubIssue::integerFromZenhubComplexityValue(h.value()->zenhubComplexityValue()))
				goodIssues.append(h.value()->issueNumber());
		}

		h++;
	}

	qDebug() << "Trackable Issues: " << allTrackableIssuesWithinRange.count();
	qDebug() << "Good Issues: " << goodIssues.count();
	qDebug() << "Closed Missing label: " << closedMissingComplexityLabel.count();
	qDebug() << "Open Missing label AND Estimate: " << openMissingComplexityLabelAndEstimate.count();
	qDebug() << "GOOD ONES: " << openMissingComplexityLabelHasEstimate.count();
	qDebug() << "Mismatches: " << complexityMismatches.count();
	qDebug() << "Missing Zenhub estimate: " << missingZenhubEstimate.count();

	qDebug() << "\n\n\n\n\n";
	qDebug() << "\n\nTrackable Issues: " << allTrackableIssuesWithinRange;
	qDebug() << "\n\nGood Issues: " << goodIssues;
	qDebug() << "\n\nClosed Missing label: " << closedMissingComplexityLabel;
	qDebug() << "\n\nOpen Missing label AND Estimate: " << openMissingComplexityLabelAndEstimate;
	qDebug() << "\n\nGOOD ONES: " << openMissingComplexityLabelHasEstimate;
	qDebug() << "\n\nMismatches: " << complexityMismatches;
	qDebug() << "\n\nMissing Zenhub estimate: " << missingZenhubEstimate;



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

	QMap<int, AMGitHubIssueFamily*> recentIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> openGoodIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> openUnestimatedIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> openMatchingIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> openMismatchedIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> finalizedIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> completeIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> missingActualComplexityIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> missingPullRequestIssueFamilies;
	QMap<int, AMGitHubIssueFamily*> allOtherIssueFamilies;

	QMap<int, AMGitHubIssueFamily*>::const_iterator k = allIssueFamilies_.constBegin();
	while(k != allIssueFamilies_.constEnd()){
		AMGitHubIssue *originatingIssue = k.value()->originatingIssue();
		AMGitHubIssue *pullRequestIssue = k.value()->pullRequestIssue();

//		if(originatingIssue->createdDate() > QDateTime::fromString("2015-07-31", "yyyy-MM-dd"))
		if(originatingIssue->createdDate() > QDateTime::fromString("2015-07-05", "yyyy-MM-dd"))
			recentIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue->isOpen() && (originatingIssue->complexityValue() == AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() != AMGitHubIssue::ZenhubComplexityInvalid) )
			openGoodIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue->isOpen() && (originatingIssue->complexityValue() == AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() == AMGitHubIssue::ZenhubComplexityInvalid) )
			openUnestimatedIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue->isOpen() && (AMGitHubIssue::integerFromComplexity(originatingIssue->complexityValue()) == AMGitHubIssue::integerFromZenhubComplexityValue(originatingIssue->zenhubComplexityValue())) )
			openMatchingIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue->isOpen())
			openMismatchedIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue && originatingIssue->inlineIssue() && (originatingIssue->complexityValue() != AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() != AMGitHubIssue::ZenhubComplexityInvalid))
			finalizedIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue && pullRequestIssue && (originatingIssue->complexityValue() != AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() != AMGitHubIssue::ZenhubComplexityInvalid) && (pullRequestIssue->projectTrackingDisabled()))
			finalizedIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue && pullRequestIssue && (originatingIssue->complexityValue() != AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() != AMGitHubIssue::ZenhubComplexityInvalid) && (pullRequestIssue->complexityValue() != AMGitHubIssue::InvalidComplexity))
			completeIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(originatingIssue && pullRequestIssue && (originatingIssue->complexityValue() != AMGitHubIssue::InvalidComplexity) && (originatingIssue->zenhubComplexityValue() != AMGitHubIssue::ZenhubComplexityInvalid) && (pullRequestIssue->complexityValue() == AMGitHubIssue::InvalidComplexity))
			missingActualComplexityIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else if(!pullRequestIssue)
			missingPullRequestIssueFamilies.insert(originatingIssue->issueNumber(), k.value());
		else
			allOtherIssueFamilies.insert(originatingIssue->issueNumber(), k.value());

		k++;
	}

	qDebug() << "Total Families: " << allIssueFamilies_.count();
	qDebug() << "Recent Families: " << recentIssueFamilies.count();
	qDebug() << "Open GOOD Families: " << openGoodIssueFamilies.count();
	qDebug() << "Open Unestimated Families: " << openUnestimatedIssueFamilies.count();
	qDebug() << "Open Matching Families: " << openMatchingIssueFamilies.count();
	qDebug() << "Open Mismatched Families: " << openMismatchedIssueFamilies.count();
	qDebug() << "Finalized Families: " << finalizedIssueFamilies.count();
	qDebug() << "Complete Families: " << completeIssueFamilies.count();
	qDebug() << "Missing Actual Complexity Families: " << missingActualComplexityIssueFamilies.count();
	qDebug() << "Missing Pull Request Families: " << missingPullRequestIssueFamilies.count();
	qDebug() << "All Other Families: " << allOtherIssueFamilies.count();

//	qDebug() << "\n\nRECENT";
//	QMap<int, AMGitHubIssueFamily*>::const_iterator ka = recentIssueFamilies.constBegin();
//	while(ka != recentIssueFamilies.constEnd()){
//		qDebug() << ka.key() << ka.value()->originatingIssue()->createdDate().toString("yyyy-MM-dd");
//		ka++;
//	}

	qDebug() << "\n\nOPEN GOOD";
	QMap<int, AMGitHubIssueFamily*>::const_iterator ka = openGoodIssueFamilies.constBegin();
	while(ka != openGoodIssueFamilies.constEnd()){
		qDebug() << ka.key() << AMGitHubIssue::integerFromZenhubComplexityValue(ka.value()->originatingIssue()->zenhubComplexityValue());
		ka++;
	}

	qDebug() << "\n\nOPEN UNESTIMATED";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kw = openUnestimatedIssueFamilies.constBegin();
	while(kw != openUnestimatedIssueFamilies.constEnd()){
		qDebug() << kw.key();
		kw++;
	}

	qDebug() << "\n\nOPEN MATCHING";
	QStringList openMatchingFamiliesList;
	QStringList openMatchingFamiliesComplexityValueList;
	QMap<int, AMGitHubIssueFamily*>::const_iterator kb = openMatchingIssueFamilies.constBegin();
	while(kb != openMatchingIssueFamilies.constEnd()){
//		qDebug() << kb.key() << AMGitHubIssue::integerFromComplexity(kb.value()->originatingIssue()->complexityValue()) << AMGitHubIssue::integerFromZenhubComplexityValue(kb.value()->originatingIssue()->zenhubComplexityValue());
		openMatchingFamiliesList << QString("%1").arg(kb.key());
		openMatchingFamiliesComplexityValueList << QString("%1").arg(AMGitHubIssue::integerFromComplexity(kb.value()->originatingIssue()->complexityValue()));
		kb++;
	}
	qDebug() << openMatchingFamiliesList;
	qDebug() << openMatchingFamiliesComplexityValueList;

	qDebug() << "\n\nOPEN MISMATCHED";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kq = openMismatchedIssueFamilies.constBegin();
	while(kq != openMismatchedIssueFamilies.constEnd()){
		qDebug() << kq.key() << AMGitHubIssue::integerFromComplexity(kq.value()->originatingIssue()->complexityValue()) << AMGitHubIssue::integerFromZenhubComplexityValue(kq.value()->originatingIssue()->zenhubComplexityValue());
		kq++;
	}

	qDebug() << "\n\nFINALIZED";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kz = finalizedIssueFamilies.constBegin();
	while(kz != finalizedIssueFamilies.constEnd()){
		QString debugString = QString("%1 (%2) %3 - %4 (%5)")
				.arg(kz.key())
				.arg(kz.value()->pullRequestIssueNumber())
				.arg(AMGitHubIssue::integerFromZenhubComplexityValue(kz.value()->originatingIssue()->zenhubComplexityValue()))
				.arg(AMGitHubIssue::integerFromZenhubComplexityValue(kz.value()->originatingIssue()->zenhubComplexityValue()))
				.arg(AMGitHubIssue::integerFromComplexity(kz.value()->originatingIssue()->complexityValue()));
		qDebug() << debugString;
		kz++;
	}

	qDebug() << "\n\nCOMPLETE";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kc = completeIssueFamilies.constBegin();
	while(kc != completeIssueFamilies.constEnd()){
		QString debugString = QString("%1 (%2) %3 - %4 (%5)")
				.arg(kc.key())
				.arg(kc.value()->pullRequestIssueNumber())
				.arg(AMGitHubIssue::integerFromComplexity(kc.value()->originatingIssue()->complexityValue()))
				.arg(AMGitHubIssue::integerFromZenhubComplexityValue(kc.value()->originatingIssue()->zenhubComplexityValue()))
				.arg(AMGitHubIssue::integerFromComplexity(kc.value()->pullRequestIssue()->complexityValue()));
		qDebug() << debugString;
		kc++;
	}

	qDebug() << "\n\nMISSING ACTUAL";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kd = missingActualComplexityIssueFamilies.constBegin();
	while(kd != missingActualComplexityIssueFamilies.constEnd()){
		QString debugString = QString("%1 (%2) %3 - %4")
				.arg(kd.key())
				.arg(kd.value()->pullRequestIssueNumber())
				.arg(AMGitHubIssue::integerFromComplexity(kd.value()->originatingIssue()->complexityValue()))
				.arg(AMGitHubIssue::integerFromZenhubComplexityValue(kd.value()->originatingIssue()->zenhubComplexityValue()));
		qDebug() << debugString;
		kd++;
	}

	qDebug() << "\n\nMISSING PULL REQUEST";
	QMap<int, AMGitHubIssueFamily*>::const_iterator ke = missingPullRequestIssueFamilies.constBegin();
	while(ke != missingPullRequestIssueFamilies.constEnd()){
		QString debugString = QString("%1 %2 - %3")
				.arg(ke.key())
				.arg(AMGitHubIssue::integerFromComplexity(ke.value()->originatingIssue()->complexityValue()))
				.arg(AMGitHubIssue::integerFromZenhubComplexityValue(ke.value()->originatingIssue()->zenhubComplexityValue()));
		qDebug() << debugString;
		ke++;
	}

	qDebug() << "\n\nGENKED";
	QMap<int, AMGitHubIssueFamily*>::const_iterator kf = allOtherIssueFamilies.constBegin();
	while(kf != allOtherIssueFamilies.constEnd()){
		QString debugString = QString("%1").arg(kf.key());
		qDebug() << debugString;
		kf++;
	}


	qDebug() << "\n\nFinalizing...";
//	int finalizingCounter = 0;
	QMap<int, AMGitHubIssueFamily*>::const_iterator kt = completeIssueFamilies.constBegin();
	while(kt != completeIssueFamilies.constEnd()){
		AMGitHubIssueFamily *oneIssueFamily = kt.value();
		if(!oneIssueFamily->originatingIssue()->inlineIssue()){
//			AMGitHubUpdateCompletedIssueFamily *oneUpdate = new AMGitHubUpdateCompletedIssueFamily(oneIssueFamily, manager_, headerData_);
//			oneUpdate->start();
			familiesToFinalize_.append(oneIssueFamily);
		}

		kt++;
//		finalizingCounter++;
//		if(finalizingCounter > 5){
//			qDebug() << "Done 5, let's check on that shiatzu";
//			return;
//		}
	}

	finalizingCounter_ = 0;
	onOneFamilyFinalized();

//	if(completeIssueFamilies.contains(348)){
//		AMGitHubIssueFamily *oneIssueFamily = completeIssueFamilies.value(348);
//		qDebug() << "Found complete issue " << oneIssueFamily->originatingIssue()->issueNumber() << oneIssueFamily->pullRequestIssue()->issueNumber();

//		AMGitHubUpdateCompletedIssueFamily *oneUpdate = new AMGitHubUpdateCompletedIssueFamily(oneIssueFamily, manager_, headerData_);
//		oneUpdate->start();
//	}
//	else{
//		qDebug() << "348 is NOT in the completed families";
//	}

	return;
}

void AMGithubProjectManagerMainView::onOneFamilyFinalized()
{
//	if(finalizingCounter_ > 15){
//		qDebug() << "Done 15, let's check them over";
//		return;
//	}
	if(familiesToFinalize_.count() > 0){
		AMGitHubUpdateCompletedIssueFamily *oneUpdate = new AMGitHubUpdateCompletedIssueFamily(familiesToFinalize_.takeFirst(), manager_, headerData_);
		connect(oneUpdate, SIGNAL(updated()), this, SLOT(onOneFamilyFinalized()));
		oneUpdate->start();
		finalizingCounter_++;
	}
}

void AMGithubProjectManagerMainView::onOneZenhubEstimateUpdateSucceeded()
{

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


	/*
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

	*/
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


AMGitHubUpdateCompletedIssueFamily::AMGitHubUpdateCompletedIssueFamily(AMGitHubIssueFamily *issueFamily, QNetworkAccessManager *manager, const QString &headerData, QObject *parent) :
	QObject(parent)
{
	issueFamily_ = issueFamily;
	initialIssueComplexityLabel_ = issueFamily_->originatingIssue()->complexityValue();
	initialIssueZenhubComplexity_ = issueFamily->originatingIssue()->zenhubComplexityValue();
	initialPullRequestComplexityLabel_ = issueFamily_->pullRequestIssue()->complexityValue();

	manager_ = manager;
	headerData_ = headerData;
}

void AMGitHubUpdateCompletedIssueFamily::start()
{
	QString removeLabelString = QString("https://api.github.com/repos/%1/%2/issues/%3/labels/%4").arg("acquaman").arg("acquaman").arg(issueFamily_->originatingIssue()->issueNumber()).arg(AMGitHubIssue::stringFromComplexity(issueFamily_->originatingIssue()->complexityValue()));
	AMRestActionInfo *removeOneLabelActionInfo = new AMRestActionInfo(removeLabelString, AMRestActionInfo::DeleteRequest);
	removeOneLabelActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
	AMRestAction *removeOneLabelAction = new AMRestAction(removeOneLabelActionInfo, manager_);
	connect(removeOneLabelAction, SIGNAL(succeeded()), this, SLOT(onRemoveInitialComplexityLabelSucceeded()));
	removeOneLabelAction->start();
}

#include "qjson/serializer.h"
#include "qjson/parser.h"

void AMGitHubUpdateCompletedIssueFamily::onRemoveInitialComplexityLabelSucceeded()
{
	qDebug() << "Succeeded removing originating issue complexity label for " << issueFamily_->originatingIssue()->issueNumber();

	QVariantList jdata;
	jdata.append(QString(AMGitHubIssue::stringFromComplexity(issueFamily_->pullRequestIssue()->complexityValue())));
	QJson::Serializer jserializer;
	QByteArray jsonData = jserializer.serialize(jdata);

	QString replaceLabelString = QString("https://api.github.com/repos/%1/%2/issues/%3/labels").arg("acquaman").arg("acquaman").arg(issueFamily_->originatingIssue()->issueNumber());
	AMRestActionInfo *replaceLabelActionInfo = new AMRestActionInfo(replaceLabelString, AMRestActionInfo::PostRequest, AMRestActionInfo::JSONContent, jsonData);
	replaceLabelActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());

	AMRestAction *replaceLabelAction = new AMRestAction(replaceLabelActionInfo, manager_);
	connect(replaceLabelAction, SIGNAL(succeeded()), this, SLOT(onUpdateComplexityLabelSucceeded()));
	replaceLabelAction->start();
}

void AMGitHubUpdateCompletedIssueFamily::onUpdateComplexityLabelSucceeded()
{
	qDebug() << "Succeeded updating originating issue complexity label for " << issueFamily_->originatingIssue()->issueNumber();

	QString removeLabelString = QString("https://api.github.com/repos/%1/%2/issues/%3/labels/%4").arg("acquaman").arg("acquaman").arg(issueFamily_->pullRequestIssue()->issueNumber()).arg(AMGitHubIssue::stringFromComplexity(issueFamily_->pullRequestIssue()->complexityValue()));
	AMRestActionInfo *removeOneLabelActionInfo = new AMRestActionInfo(removeLabelString, AMRestActionInfo::DeleteRequest);
	removeOneLabelActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
	AMRestAction *removeOneLabelAction = new AMRestAction(removeOneLabelActionInfo, manager_);
	connect(removeOneLabelAction, SIGNAL(succeeded()), this, SLOT(onRemovePullRequestComplexityLabelSucceeded()));
	removeOneLabelAction->start();
}

void AMGitHubUpdateCompletedIssueFamily::onRemovePullRequestComplexityLabelSucceeded()
{
	qDebug() << "Succeeded removing pull request issue complexity label for " << issueFamily_->originatingIssue()->issueNumber();

	QVariantList jdata;
	jdata.append(QString("Project Tracking Disabled"));
	QJson::Serializer jserializer;
	QByteArray jsonData = jserializer.serialize(jdata);

	QString replaceLabelString = QString("https://api.github.com/repos/%1/%2/issues/%3/labels").arg("acquaman").arg("acquaman").arg(issueFamily_->pullRequestIssue()->issueNumber());
	AMRestActionInfo *replaceLabelActionInfo = new AMRestActionInfo(replaceLabelString, AMRestActionInfo::PostRequest, AMRestActionInfo::JSONContent, jsonData);
	replaceLabelActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());

	AMRestAction *replaceLabelAction = new AMRestAction(replaceLabelActionInfo, manager_);
	connect(replaceLabelAction, SIGNAL(succeeded()), this, SLOT(onUpdatePullRequestLabelSucceeded()));
	replaceLabelAction->start();
}

void AMGitHubUpdateCompletedIssueFamily::onUpdatePullRequestLabelSucceeded()
{
	qDebug() << "Succeeded in updating issue family for " << issueFamily_->originatingIssue()->issueNumber() << issueFamily_->pullRequestIssue()->issueNumber();
	emit updated();
}
