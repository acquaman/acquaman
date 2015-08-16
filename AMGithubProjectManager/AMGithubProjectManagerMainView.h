#ifndef AMGITHUBPROJECTMANAGERMAINVIEW_H
#define AMGITHUBPROJECTMANAGERMAINVIEW_H

#include <QWidget>
#include <QNetworkReply>

class QPushButton;
class QLabel;
class QNetworkAccessManager;
class QProgressBar;

class AMGitHubRepository;
class AMControlButton;
class AMRestAction;
class AMGitHubIssue;
class AMGitHubIssueFamily;
class AMGitHubMilestone;

class AMGithubProjectManagerMainView : public QWidget
{
	Q_OBJECT

public:
	AMGithubProjectManagerMainView(QWidget *parent = 0);
	~AMGithubProjectManagerMainView();

protected slots:
	void onInitiateButtonClicked();
	void onReloadButtonClicked();

	void onRepositorySubItemProgressUpdated(int percentComplete);
	void onRepositoryOverallProgressUpdated(int percentComplete);

	void onRepositoryLoaded();
	void onRepositoryReloaded();

protected:
	void repositoryReadyToProceed();

	void onOneZenhubEstimateUpdateSucceeded();

protected:
	QPushButton *initiateButton_;
	QPushButton *reloadButton_;
	QProgressBar *subItemProgressBar_;
	QProgressBar *overallProgressBar_;
	QList<int> overallProgressValues_;
	QLabel *statusMessageLabel_;

	QNetworkAccessManager *manager_;

	AMGitHubRepository *repository_;

	const QMap<int, AMGitHubIssue*> *allIssues_;
	const QMap<int, AMGitHubMilestone*> *allMilestones_;
	const QMap<int, AMGitHubIssueFamily*> *allIssueFamilies_;
};

#endif // AMGITHUBPROJECTMANAGERMAINVIEW_H
