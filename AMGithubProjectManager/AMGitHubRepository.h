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
	AMGitHubRepository(const QString &owner, const QString &repo, QNetworkAccessManager *manager, const QString &accessToken, QObject *parent = 0);

	QString owner() const { return owner_; }
	QString repo() const { return repo_; }

	QNetworkAccessManager* networkAccessManager() const { return manager_; }
	QString accessToken() const { return accessToken_; }

	const QMap<int, AMGitHubIssue*>* allIssues() const { return allIssues_; }
	const QMap<int, AMGitHubIssueFamily*>* allIssueFamilies() const { return allIssueFamilies_; }
	const QMap<int, AMGitHubMilestone*>* allMilestones() const { return allMilestones_; }


public slots:
	void initiateRepositoryLoading();

	void setOwner(const QString &owner) { owner_ = owner; }
	void setRepo(const QString &repo) { repo_ = repo; }

	void setAccessToken(const QString &accessToken) { accessToken_ = accessToken; }

signals:
	void repositoryLoaded();

protected slots:
	void onGetAllIssuesActionSucceeded();
	void onGetAllCommentsActionSucceeded();
	void onGetAllZenhubEstimatesSucceeded();

protected:
	QString owner_;
	QString repo_;

	QNetworkAccessManager *manager_;
	QString accessToken_;

	QMap<int, AMGitHubIssue*> *allIssues_;
	QMap<int, AMGitHubIssueFamily*> *allIssueFamilies_;
	QMap<int, AMGitHubMilestone*> *allMilestones_;

	QStringList commentURLs_;
};

#endif // AMGITHUBREPOSITORY_H
