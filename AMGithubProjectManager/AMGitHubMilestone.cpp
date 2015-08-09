#include "AMGitHubMilestone.h"

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
	}
	else
		state_ = AMGitHubMilestone::InvalidState;

	associatedIssues_ = new QMap<int, AMGitHubIssue*>();
	associatedFamilies_ = new QMap<int, AMGitHubIssueFamily*>();
}

QString AMGitHubMilestone::stateAsString() const
{
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

void AMGitHubMilestone::associateIssue(AMGitHubIssue *associatedIssue)
{
	if(!associatedIssues_->contains(associatedIssue->issueNumber()))
		associatedIssues_->insert(associatedIssue->issueNumber(), associatedIssue);
}

void AMGitHubMilestone::associateFamily(AMGitHubIssueFamily *associatedFamily)
{
	if(!associatedFamilies_->contains(associatedFamily->originatingIssue()->issueNumber()))
		associatedFamilies_->insert(associatedFamily->originatingIssue()->issueNumber(), associatedFamily);
}
