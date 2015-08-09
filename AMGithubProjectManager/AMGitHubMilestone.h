#ifndef AMGITHUBMILESTONE_H
#define AMGITHUBMILESTONE_H

#include "AMGitHubIssueFamily.h"

class AMGitHubMilestone : public QObject
{
Q_OBJECT
	enum MilestoneState { ClosedState, OpenState, InvalidState };
public:
//	AMGitHubMilestone(int number = -1, const QString &title = QString(), AMGitHubMilestone::MilestoneState state = AMGitHubMilestone::InvalidState, QObject *parent = 0);
	AMGitHubMilestone(QVariantMap jsonMap, QObject *parent = 0);

	int number() const { return number_; }
	QString title() const { return title_; }
	AMGitHubMilestone::MilestoneState state() const { return state_; }
	QString stateAsString() const;

	const QMap<int, AMGitHubIssue*>* associatedIssues() const { return associatedIssues_; }
//	QList<AMGitHubIssue*> associatedIssues() const { return associatedIssues_; }
//	QList<AMGitHubIssueFamily*> associatedFamilies() const { return associatedFamilies_; }

public slots:
	void associatedIssue(AMGitHubIssue *associatedIssue);
//	void appendAssociatedFamily(AMGitHubIssueFamily *associatedFamily);

protected:
	int number_;
	QString title_;
	AMGitHubMilestone::MilestoneState state_;

	QMap<int, AMGitHubIssue*> *associatedIssues_;
//	QList<AMGitHubIssue*> associatedIssues_;
//	QList<AMGitHubIssueFamily*> associatedFamilies_;
};

#endif // AMGITHUBMILESTONE_H
