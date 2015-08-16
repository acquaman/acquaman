#ifndef AMGITHUBMILESTONE_H
#define AMGITHUBMILESTONE_H

#include "AMGitHubIssueFamily.h"

class AMGitHubMilestone : public QObject
{
Q_OBJECT
public:
	enum MilestoneState { ClosedState, OpenState, InvalidState };
	AMGitHubMilestone(QVariantMap jsonMap, QObject *parent = 0);

	int number() const { return number_; }
	QString title() const { return title_; }
	AMGitHubMilestone::MilestoneState state() const { return state_; }
	QString stateAsString() const;

	const QMap<int, AMGitHubIssue*>* associatedIssues() const { return associatedIssues_; }
	const QMap<int, AMGitHubIssueFamily*>* associatedFamilies() const { return associatedFamilies_; }

	QVariantMap toMap() const;
	QByteArray toJSON() const;

public slots:
	void associateIssue(AMGitHubIssue *associateIssue);
	void associateFamily(AMGitHubIssueFamily *associatedFamily);

protected:
	int number_;
	QString title_;
	AMGitHubMilestone::MilestoneState state_;

	QMap<int, AMGitHubIssue*> *associatedIssues_;
	QMap<int, AMGitHubIssueFamily*> *associatedFamilies_;
};

Q_DECLARE_METATYPE(AMGitHubMilestone::MilestoneState)

#endif // AMGITHUBMILESTONE_H
