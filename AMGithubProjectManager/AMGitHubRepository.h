#ifndef AMGITHUBREPOSITORY_H
#define AMGITHUBREPOSITORY_H

#include <QObject>
#include <QMap>
#include <QStringList>

class QNetworkAccessManager;

class AMGitHubIssue;
class AMGitHubIssueFamily;
class AMGitHubMilestone;

class AMGitHubRepository : public QObject
{
Q_OBJECT
public:
	enum IssueMapType {AllIssuesMap, OpenIssuesMap, ClosedIssuesMap, FullySpecifiedOpenMap, MissingEstimateOpenMap,
		      UntrackedClosedMap, PartiallyTrackedClosedMap, FullyTrackedClosedMap, CompleteClosedMap,
		      MissingEstimateClosedMap, MissingActualClosedMap, MissingTimeReportClosedMap, FullySpecifiedClosedMap,
		      InvalidIssuesMap};

	AMGitHubRepository(const QString &owner, const QString &repo, QNetworkAccessManager *manager, const QString &accessToken, QObject *parent = 0);

	QString owner() const { return owner_; }
	QString repo() const { return repo_; }

	QNetworkAccessManager* networkAccessManager() const { return manager_; }
	QString accessToken() const { return accessToken_; }

	const QMap<int, AMGitHubIssue*>* issuesMap(AMGitHubRepository::IssueMapType issuesMapType) const;

	const QMap<int, AMGitHubIssue*>* allIssues() const { return allIssues_; }
	const QMap<int, AMGitHubIssue*>* openIssues() const { return allOpenIssues_; }
	const QMap<int, AMGitHubIssue*>* closedIssues() const { return allClosedIssues_; }

	const QMap<int, AMGitHubIssue*>* fullySpecifiedOpenIssues() const { return fullySpecifiedOpenIssues_; }
	const QMap<int, AMGitHubIssue*>* missingEstimateOpenIssues() const { return missingEstimateOpenIssues_; }

	const QMap<int, AMGitHubIssue*>* untrackedClosedIssues() const { return completelyUntrackedIssues_; }
	const QMap<int, AMGitHubIssue*>* partiallyTrackedClosedIssues() const { return trackedWithoutEstimateIssues_; }
	const QMap<int, AMGitHubIssue*>* fullyTrackedClosedIssues() const { return fullyTrackedIssues_; }
	const QMap<int, AMGitHubIssue*>* completeClosedIssues() const { return completeIssues_; }

	const QMap<int, AMGitHubIssue*> *missingEstimateClosedIssues() const { return missingEstimateClosedIssues_; }
	const QMap<int, AMGitHubIssue*> *missingActualClosedIssues() const { return missingActualClosedIssues_; }
	const QMap<int, AMGitHubIssue*> *missingTimeClosedIssues() const { return missingTimeClosedIssues_; }

	static QMap<int, AMGitHubIssue*> filterForLabels(const QMap<int, AMGitHubIssue*> *mapToFilter, QStringList labelsToFilter);


	const QMap<int, AMGitHubIssue*> *fullySpecifiedClosedIssues() const { return fullySpecifiedClosedIssues_; }

	const QMap<int, AMGitHubIssueFamily*>* allIssueFamilies() const { return allIssueFamilies_; }

	const QMap<int, AMGitHubMilestone*>* allMilestones() const { return allMilestones_; }

	int issuesCount(AMGitHubRepository::IssueMapType issuesMapType) const;

	/// A count of ALL issues both open and closed. Should match the number in the repo
	int issueCount() const { return allIssues_->count(); }

	/// A count of ALL open issues
	int openIssueCount() const { return allOpenIssues_->count(); }
	/// A count of open issues that are fully specified (have an estimate)
	int fullySpecifiedOpenIssueCount() const { return fullySpecifiedOpenIssues_->count(); }
	/// A count of open issues missing an estimate
	int missingEstimateOpenIssueCount() const { return missingEstimateOpenIssues_->count(); }

	/// A count of ALL closed issues
	int closedIssueCount() const { return allClosedIssues_->count(); }
	/// A count of (closed) issues that have no tracking whatsoever (pull requests with project tracking disabled are partially tracked)
	int completeUntrackedIssueCount() const { return completelyUntrackedIssues_->count(); }
	/// A count of (closed) issues that are tracked without time estimates (pull requests prior to Zenhub)
	int trackedWithoutEstimateIssueCount() const { return trackedWithoutEstimateIssues_->count(); }
	/// A count of (closed) issues that should have complete information for the Estimate/Actual/Time data
	int fullyTrackedIssueCount() const { return fullyTrackedIssues_->count(); }
	/// A count of (closed) issues that are complete (have valid information for the Estimate/Actual/Time data)
	int completeIssueCount() const { return completeIssues_->count(); }
	/// A count of (closed) issues that are missing an estimate (they may be missing more than that) (we ignore untracked and partially tracked here, they shouldn't have estimates)
	int missingEstimateClosedIssueCount() const { return missingEstimateClosedIssues_->count(); }
	/// A count of (closed) issues that are missing an actual complexity (they may be missing more than that) (we ignore untracked and partially tracked here, they shouldn't have actuals)
	int missingActualClosedIssueCount() const { return missingActualClosedIssues_->count(); }
	/// A count of (closed) issues that are missing a time reporting string (they may be missing more than that) (we ignore untracked and partially tracked here, they shouldn't have time reports)
	int missingTimeClosedIssueCount() const { return missingTimeClosedIssues_->count(); }
	/// A count of (closed) issues that have either complete or a pull request in the old style (not really missing any info)
	int fullySpecifiedClosedIssueCount() const { return fullySpecifiedClosedIssues_->count(); }

	/// A count of ALL families
	int familyCount() const { return allIssueFamilies_->count(); }
	/// A count of families that are fully specified for their current state (with respect to open, closed, pull request, etc)
	int fullySpecifiedFamilyCount() const { return fullySpecifiedFamilies_->count(); }
	/// A count of families that are fully closed and fully specified
	int completedFamilyCount() const { return completedFamilies_->count(); }


	/// A count of ALL milestones
	int milestoneCount() const { return allMilestones_->count(); }

public slots:
	void initiateRepositoryLoading();
	void reloadRepositoryFromFile(const QString &filePath);

	void setOwner(const QString &owner) { owner_ = owner; }
	void setRepo(const QString &repo) { repo_ = repo; }

	void setAccessToken(const QString &accessToken) { accessToken_ = accessToken; }

signals:
	void repositoryLoaded();
	void repositoryReloaded();

	void repositorySubItemProgressUpdated(int percentComplete);
	void repositoryOverallProgressUpdated(int percentComplete);
	void repositoryStatusMessageChanged(const QString &statusMessage);

protected slots:
	void onGetAllIssuesActionSucceeded();
	void onGetAllCommentsActionSucceeded();
	void onGetAllZenhubEstimatesSucceeded();

	void onActionProgressChanged(double numerator, double denominator);

protected:
	void sortMaps();

protected:
	QString owner_;
	QString repo_;

	QNetworkAccessManager *manager_;
	QString accessToken_;

	/// Map of all issues
	QMap<int, AMGitHubIssue*> *allIssues_;

	/// Category maps for issues
	QMap<int, AMGitHubIssue*> *allOpenIssues_;
	QMap<int, AMGitHubIssue*> *fullySpecifiedOpenIssues_;
	QMap<int, AMGitHubIssue*> *missingEstimateOpenIssues_;

	QMap<int, AMGitHubIssue*> *allClosedIssues_;
	QMap<int, AMGitHubIssue*> *completelyUntrackedIssues_;
	QMap<int, AMGitHubIssue*> *trackedWithoutEstimateIssues_;
	QMap<int, AMGitHubIssue*> *fullyTrackedIssues_;
	QMap<int, AMGitHubIssue*> *completeIssues_;
	QMap<int, AMGitHubIssue*> *missingEstimateClosedIssues_;
	QMap<int, AMGitHubIssue*> *missingActualClosedIssues_;
	QMap<int, AMGitHubIssue*> *missingTimeClosedIssues_;
	QMap<int, AMGitHubIssue*> *fullySpecifiedClosedIssues_;


	/// Map of all families
	QMap<int, AMGitHubIssueFamily*> *allIssueFamilies_;

	/// Category maps for families
	QMap<int, AMGitHubIssueFamily*> *fullySpecifiedFamilies_;
	QMap<int, AMGitHubIssueFamily*> *completedFamilies_;

	/// Map of all milestones
	QMap<int, AMGitHubMilestone*> *allMilestones_;

	QStringList commentURLs_;
};

#endif // AMGITHUBREPOSITORY_H
