#include "AMGitHubIssue.h"

#include <QDebug>

AMGitHubIssue::AMGitHubIssue(QVariantMap jsonMap, QObject *parent) :
	QObject(parent)
{
	issueNumber_ = -1;
	complexityValue_ = AMGitHubIssue::InvalidComplexity;
	title_ = "Invalid Title";
	isPullRequest_ = false;
	originatingIssue_ = 0;
	originatingIssueNumber_ = -1;
	commentCount_ = 0;
	commentsURL_ = "InvalidURL";
//	timeEstimateString_ = "Invalid Time Estimate";
	timeEstimateString_ = QString();
	assignee_ = "Invalid Assignee";
	projectTrackingDisabled_ = false;
	inlineIssue_ = false;
	issueState_ = AMGitHubIssue::InvalidState;

	if(jsonMap.value("number").canConvert<int>())
		issueNumber_ = jsonMap.value("number").toInt();
	if(jsonMap.value("title").canConvert<QString>()){
		title_ = jsonMap.value("title").toString();
		int startPoint = title_.indexOf("Issue") + 5;
		int endPoint = title_.indexOf(":");
		QString originatingIssue = title_.mid(startPoint, endPoint-startPoint);
		if(!originatingIssue.isEmpty())
			originatingIssueNumber_ = originatingIssue.toInt();
	}
	if(jsonMap.value("comments").canConvert<int>())
		commentCount_ = jsonMap.value("comments").toInt();
	if(jsonMap.value("comments_url").canConvert<QString>())
		commentsURL_ = jsonMap.value("comments_url").toString();
	if(jsonMap.contains("assignee") && jsonMap.value("assignee").toMap().value("login").canConvert<QString>())
		assignee_ = jsonMap.value("assignee").toMap().value("login").toString();
	if(jsonMap.contains("labels") && jsonMap.value("labels").canConvert<QVariantList>()){
		QVariantList labelsList = jsonMap.value("labels").toList();
		for(int x = 0, size = labelsList.count(); x < size; x++){
			QVariantMap labelMap = labelsList.at(x).toMap();
//			qDebug() << "One label: " << labelMap;
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Complexity"))
				complexityValue_ = complexityFromString(labelMap.value("name").toString());
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Disabled"))
				projectTrackingDisabled_ = true;
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Inline Issue"))
				inlineIssue_ = true;
		}
	}

	if(jsonMap.contains("pull_request") && !jsonMap.value("pull_request").isNull())
		isPullRequest_ = true;

	if(jsonMap.contains("state") && jsonMap.value("state").canConvert<QString>()){
		QString stateAsString = jsonMap.value("state").toString();
		if(stateAsString == "open")
			issueState_ = AMGitHubIssue::OpenState;
		else if(stateAsString == "closed")
			issueState_ = AMGitHubIssue::ClosedState;
	}

	if(jsonMap.contains("created_at") && jsonMap.value("created_at").canConvert<QString>()){
		QString createdDateString = jsonMap.value("created_at").toString();
		createdDate_ = QDateTime::fromString(createdDateString, "yyyy-MM-ddThh:mm:ssZ");
		if(!createdDate_.isValid()){
			qDebug() << "Not a valid created at date";
			createdDate_ = QDateTime();
		}
	}
	if(jsonMap.contains("closed_at") && jsonMap.value("closed_at").canConvert<QString>()){
		QString closedDateString = jsonMap.value("closed_at").toString();
		closedDate_ = QDateTime::fromString(closedDateString, "yyyy-MM-ddThh:mm:ssZ");
		if(!closedDate_.isValid()){
			qDebug() << "Not a valid closed at date";
			closedDate_ = QDateTime();
		}
	}
}

AMGitHubIssue::~AMGitHubIssue()
{
}

AMGitHubIssue::ComplexityValue AMGitHubIssue::complexityFromString(const QString &complexityString)
{
	if(complexityString == "Complexity1")
		return AMGitHubIssue::Complexity1;
	else if(complexityString == "Complexity2")
		return AMGitHubIssue::Complexity2;
	else if(complexityString == "Complexity3")
		return AMGitHubIssue::Complexity3;
	else if(complexityString == "Complexity5")
		return AMGitHubIssue::Complexity5;
	else if(complexityString == "Complexity8")
		return AMGitHubIssue::Complexity8;
	else if(complexityString == "ComplexityK")
		return AMGitHubIssue::ComplexityK;
	else
		return AMGitHubIssue::InvalidComplexity;
}

QString AMGitHubIssue::stringFromComplexity(AMGitHubIssue::ComplexityValue complexityValue){
	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		return "Complexity1";
	case AMGitHubIssue::Complexity2:
		return "Complexity2";
	case AMGitHubIssue::Complexity3:
		return "Complexity3";
	case AMGitHubIssue::Complexity5:
		return "Complexity5";
	case AMGitHubIssue::Complexity8:
		return "Complexity8";
	case AMGitHubIssue::ComplexityK:
		return "ComplexityK";
	case AMGitHubIssue::InvalidComplexity:
		return "Invalid Complexity";
	default:
		return "Invalid Complexity";
	}
}

QString AMGitHubIssue::stringFromState(AMGitHubIssue::IssueState issueState){
	switch(issueState){
	case AMGitHubIssue::OpenState:
		return "Open";
	case AMGitHubIssue::ClosedState:
		return "Closed";
	case AMGitHubIssue::InvalidState:
		return "Invalid State";
	}
}

QString AMGitHubIssue::oneLineDebugInfo() const
{
	QString retVal;
	if(!isPullRequest_)
		retVal = QString("[%1] %2: %3").arg(issueNumber_).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));
	else if(originatingIssue_)
		retVal = QString("[%1 --> %2] %3: %4").arg(issueNumber_).arg(originatingIssue_->issueNumber()).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));
	else
		retVal = QString("[%1 ==> %2] %3: %4").arg(issueNumber_).arg(originatingIssueNumber_).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));

	if(!timeEstimateString_.isEmpty() || timeEstimateString_ == "Invalid Time Estimate")
		retVal.append(QString("{%1}").arg(timeEstimateString_));

//	if(!commentsURL_.isEmpty())
//		retVal.append(QString(" [@%1]").arg(commentsURL_));

	return retVal;
}

QString AMGitHubIssue::multiLineDebugInfo() const
{
	QString retVal;
	if(!isPullRequest_)
		retVal = QString("[%1]\n").arg(issueNumber_);
	else if(originatingIssue_)
		retVal = QString("[%1 --> %2]\n").arg(issueNumber_).arg(originatingIssue_->issueNumber());
	else
		retVal = QString("[%1 ==> %2]\n").arg(issueNumber_).arg(originatingIssueNumber_);

	retVal.append(QString("Title:\t\t\t%1\n").arg(title_));
	retVal.append(QString("Complexity:\t\t\t%1\n").arg(AMGitHubIssue::stringFromComplexity(complexityValue_)));
	if(!timeEstimateString_.isEmpty() || timeEstimateString_ == "Invalid Time Estimate")
		retVal.append(QString("Time Estimate:\t\t%1\n").arg(timeEstimateString_));

	return retVal;
}
