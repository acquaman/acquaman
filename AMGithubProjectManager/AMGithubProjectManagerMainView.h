#ifndef AMGITHUBPROJECTMANAGERMAINVIEW_H
#define AMGITHUBPROJECTMANAGERMAINVIEW_H

#include <QWidget>
#include <QNetworkReply>

class QPushButton;
class QLineEdit;
class QNetworkAccessManager;
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

	void onGetAllIssuesActionSucceeded();
	void onGetAllCommentsActionSucceeded();
	void onGetAllZenhubEstimatesSucceeded();

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
};

#endif // AMGITHUBPROJECTMANAGERMAINVIEW_H
