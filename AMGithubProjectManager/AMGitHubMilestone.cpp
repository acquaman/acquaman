#include "AMGitHubMilestone.h"

#include <QDebug>

#include "qjson/serializer.h"
#include "qjson/parser.h"

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

QVariantMap AMGitHubMilestone::toMap() const
{
	QVariantMap retVal;

	retVal.insert("number", number_);
	retVal.insert("title", title_);
	retVal.insert("state", state_);

	QVariantList issueNumbers;
	for(int x = 0, size = associatedIssues_->keys().count(); x < size; x++)
		issueNumbers.append(associatedIssues_->keys().at(x));
	retVal.insert("issueNumbers", issueNumbers);

	return retVal;
}

QByteArray AMGitHubMilestone::toJSON() const
{
	QVariantMap asMap = toMap();

	QJson::Serializer jserializer;
	QByteArray retVal = jserializer.serialize(asMap);
	return retVal;
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

void AMGitHubMilestone::resetFromMap(const QVariantMap &map, QMap<int, AMGitHubIssue *> *allIssues)
{
	if(map.contains("number"))
		number_ = map.value("number").toInt();
	if(map.contains("title"))
		title_ = map.value("title").toString();
	if(map.contains("state"))
		state_ = AMGitHubMilestone::MilestoneState(map.value("state").toLongLong());

	if(map.contains("issueNumbers")){
		QVariantList issueNumbers = map.value("issueNumbers").toList();
		for(int x = 0, size = issueNumbers.count(); x < size; x++)
			if(allIssues->contains(issueNumbers.at(x).toInt()))
				associateIssue(allIssues->value(issueNumbers.at(x).toInt()));
	}
}

void AMGitHubMilestone::resetFromJSON(const QByteArray &json, QMap<int, AMGitHubIssue*> *allIssues)
{
	QJson::Parser jparser;
	bool successfulParsing = false;
	QVariant parseVariant = jparser.parse(json, &successfulParsing);
	if(successfulParsing && parseVariant.canConvert<QVariantMap>()){
		QVariantMap parseVariantMap = parseVariant.toMap();
		resetFromMap(parseVariantMap, allIssues);
	}
	else
		qDebug() << "[AMGitHubMilestone] Failed to parse JSON in resetFromJSON";
}
