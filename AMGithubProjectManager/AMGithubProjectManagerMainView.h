#ifndef AMGITHUBPROJECTMANAGERMAINVIEW_H
#define AMGITHUBPROJECTMANAGERMAINVIEW_H

#include <QWidget>
#include <QNetworkReply>

class QPushButton;
class QLineEdit;
class QNetworkAccessManager;
class AMControlButton;
class AMRestAction;
//class AMGitHubIssue;
class AMGitHubIssueFamily;
class AMGitHubMilestone;

#include "AMGitHubIssue.h"

class AMGithubProjectManagerMainView : public QWidget
{
	Q_OBJECT

public:
	AMGithubProjectManagerMainView(QWidget *parent = 0);
	~AMGithubProjectManagerMainView();

protected slots:
	void onInitiateButtonClicked();

	void onGetAllIssuesActionSucceeded();
	void onGetAllCommentsActionSucceeded();
	void onGetAllZenhubEstimatesSucceeded();

	void onOneFamilyFinalized();

	void onOneZenhubEstimateUpdateSucceeded();

	void onGetOneIssueEventsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);

protected:
	QPushButton *initiateButton_;
	QLineEdit *userNameLineEdit_;
	QLineEdit *passwordLineEdit_;

	QNetworkAccessManager *manager_;
	QString headerData_;
	AMRestAction *authenicationAction_;
	AMRestAction *getAllClosedIssuesAction_;
	int currentClosedIssuesPage_;
	bool lastPage_;
	QMap<int, AMGitHubIssue*> allIssues_;
	QMap<int, AMGitHubIssueFamily*> allIssueFamilies_;
	QMap<int, AMGitHubIssueFamily*> fullySpecifiedIssueFamilies_;
	QStringList commentURLs_;
	QMap<int, AMGitHubMilestone*> allMilestones_;

	QStringList openIssuesToFix_;
	QStringList openIssueComplexitiesToFix_;

	QList<AMGitHubIssueFamily*> familiesToFinalize_;
	int finalizingCounter_;
};

class AMGitHubUpdateCompletedIssueFamily : public QObject
{
Q_OBJECT
public:
	AMGitHubUpdateCompletedIssueFamily(AMGitHubIssueFamily *issueFamily, QNetworkAccessManager *manager, const QString &headerData, QObject *parent = 0);

public slots:
	void start();

signals:
	void updated();

protected slots:
	void onRemoveInitialComplexityLabelSucceeded();
	void onUpdateComplexityLabelSucceeded();
	void onRemovePullRequestComplexityLabelSucceeded();
	void onUpdatePullRequestLabelSucceeded();

protected:
	AMGitHubIssueFamily *issueFamily_;
	AMGitHubIssue::ComplexityValue initialIssueComplexityLabel_;
	AMGitHubIssue::ZenhubComplexityValue initialIssueZenhubComplexity_;
	AMGitHubIssue::ComplexityValue initialPullRequestComplexityLabel_;

	QNetworkAccessManager *manager_;
	QString headerData_;
};

#endif // AMGITHUBPROJECTMANAGERMAINVIEW_H
