#ifndef AMGITHUBPROJECTMANAGERMAINVIEW_H
#define AMGITHUBPROJECTMANAGERMAINVIEW_H

#include <QWidget>
#include <QNetworkReply>

class QPushButton;
class QLineEdit;
class QNetworkAccessManager;

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
	void onGetAllZenhubEstimatesSucceeded();

	void onOneZenhubEstimateUpdateSucceeded();

	void onGetOneIssueEventsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);

protected:
	QPushButton *initiateButton_;
	QLineEdit *userNameLineEdit_;
	QLineEdit *passwordLineEdit_;

	QNetworkAccessManager *manager_;

	AMGitHubRepository *repository_;

	const QMap<int, AMGitHubIssue*> *allIssues_;
	const QMap<int, AMGitHubMilestone*> *allMilestones_;
	QMap<int, AMGitHubIssueFamily*> allIssueFamilies_;
};

#endif // AMGITHUBPROJECTMANAGERMAINVIEW_H
