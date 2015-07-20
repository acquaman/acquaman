#include "AMGitHubMilestone.h"

AMGitHubMilestone::AMGitHubMilestone(int number, const QString &title, AMGitHubMilestone::MilestoneState state, QObject *parent) :
	QObject(parent)
{
	number_ = number;
	title_ = title;
	state_ = state;
}

AMGitHubMilestone::AMGitHubMilestone(QVariantMap jsonMap, QObject *parent) :
	QObject(parent)
{
	if(jsonMap.value("number").canConvert<int>())
		number_ = jsonMap.value("number").toInt();
	else
		number_ = -1;

	if(jsonMap.value("title").canConvert<QString>())
		title_ = jsonMap.value("title").toString();
	else
		title_ = QString();

	if(jsonMap.value("state").canConvert<QString>()){
		QString stateAsString = jsonMap.value("state").toString();
		if(stateAsString == "open")
			state_ = AMGitHubMilestone::OpenState;
		else if(stateAsString == "closed")
			state_ = AMGitHubMilestone::ClosedState;
		else
			state_ = AMGitHubMilestone::InvalidState;
	}else
		state_ = AMGitHubMilestone::InvalidState;
}

QString AMGitHubMilestone::stateAsString() const{
	switch(state_){
	case AMGitHubMilestone::OpenState:
		return "Open";
	case AMGitHubMilestone::ClosedState:
		return "Closed";
	case AMGitHubMilestone::InvalidState:
		return "Invalid State";
	default:
		return "Invalid State";
	}
}

void AMGitHubMilestone::appendAssociatedIssue(AMGitHubIssue *associatedIssue){
	if(!associatedIssues_.contains(associatedIssue))
		associatedIssues_.append(associatedIssue);
}

void AMGitHubMilestone::appendAssociatedFamily(AMGitHubIssueFamily *associatedFamily){
	if(!associatedFamilies_.contains(associatedFamily))
		associatedFamilies_.append(associatedFamily);
}
