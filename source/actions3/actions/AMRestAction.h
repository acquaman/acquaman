#ifndef AMRESTACTION_H
#define AMRESTACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMRestActionInfo.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>

#define AMRESTACTION_CANNOT_START_WITH_EMPTY_URL 355001
#define AMRESTACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER 355002
#define AMRESTACTION_NETWORK_ERROR_FAILURE 355003

class AMRestAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMRestAction(AMRestActionInfo *info, QNetworkAccessManager *networkAccessManager,  QObject *parent = 0);
	/// Copy constructor.
	AMRestAction(const AMRestAction &other);
	/// Destructor.
	virtual ~AMRestAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

	/// Specify that we can pause.
	virtual bool canPause() const { return false; }
	/// Specify that we can skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has no children.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMRestActionInfo *restActionInfo() const { return qobject_cast<const AMRestActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMRestActionInfo* restActionInfo() { return qobject_cast<AMRestActionInfo *>(info()); }

	QNetworkAccessManager* networkAccessManager() const { return networkAccessManager_; }

	/// Returns a string in a readable format based on the returned values when requesting the issues from the respository.
	QString jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel = 0) const;

	static int pageNumberFromURLString(const QString &urlString);

	/// Returns the generic JSON response
	QVariant fullResponse() const { fullResponse_;}

public slots:
	/// Sets the pointer to a long-lived network access manager. This action will not clean up the memory or take ownership of this.
	void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

signals:
	void fullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair>);

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// Pauses the action by stopping the timer.
	virtual void pauseImplementation();
	/// Resumes the action by resuming the timer.
	virtual void resumeImplementation();
	/// Handles cancelling the scan.
	virtual void cancelImplementation();
	/// Handles skipping the action.
	virtual void skipImplementation(const QString &command);


protected slots:
	void onNetworkRequestReturned();
	void onNetworkRequestError(QNetworkReply::NetworkError networkError);

protected:
	/// Network reply for our request
	QNetworkReply *restReply_;
	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// The generic JSON response
	QVariant fullResponse_;
};

class AMGitHubIssue : public QObject
{
Q_OBJECT
public:
	enum IssueState { ClosedState, OpenState, InvalidState };
	enum ComplexityValue { Complexity1, Complexity2, Complexity3, Complexity5, Complexity8, ComplexityK, InvalidComplexity };

	AMGitHubIssue(int issueNumber = -1, AMGitHubIssue::ComplexityValue complexityValue = AMGitHubIssue::InvalidComplexity, const QString &title = "Invalid Title", bool isPullRequest = false, AMGitHubIssue *originatingIssue = 0, int originatingIssueNumber = -1, int commentCount = 0, const QString &commentsURL = "InvalidURL", const QString &timeEstimateString = "Invalid Time Estimate", const QString &assignee = "Invalid Assignee", bool projectTrackingDisabled = false, bool inlineIssue = false, AMGitHubIssue::IssueState issueState = AMGitHubIssue::InvalidState, QObject *parent = 0);
	AMGitHubIssue(QVariantMap jsonMap, QObject *parent = 0);
	virtual ~AMGitHubIssue();

	int issueNumber() const { return issueNumber_; }
	AMGitHubIssue::ComplexityValue complexityValue() const { return complexityValue_; }
	QString title() const { return title_; }
	bool isPullRequest() const { return isPullRequest_; }
	AMGitHubIssue* originatingIssue() const { return originatingIssue_; }
	int originatingIssueNumber() const { return originatingIssueNumber_; }
	int commentCount() const { return commentCount_; }
	QString commentsURL() const { return commentsURL_; }
	QString timeEstimateString() const { return timeEstimateString_; }
	QString assignee() const { return assignee_; }
	bool projectTrackingDisabled() const { return projectTrackingDisabled_; }
	bool inlineIssue() const { return inlineIssue_; }
	AMGitHubIssue::IssueState issueState() const { return issueState_; }

	static AMGitHubIssue::ComplexityValue complexityFromString(const QString &complexityString);
	static QString stringFromComplexity(AMGitHubIssue::ComplexityValue complexityValue);

	static QString stringFromState(AMGitHubIssue::IssueState issueState);

	QString oneLineDebugInfo() const;
	QString multiLineDebugInfo() const;

public slots:
	void setIssueNumber(int issueNumber) { issueNumber_ = issueNumber; }
	void setComplexityValue(AMGitHubIssue::ComplexityValue complexityValue) { complexityValue_ = complexityValue; }
	void setTitle(const QString &title) { title_ = title; }
	void setIsPullRequest(bool isPullRequest) { isPullRequest_ = isPullRequest; }
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setOriginatingIssueNumber(int originatingIssueNumber) { originatingIssueNumber_ = originatingIssueNumber; }
	void setCommentCount(int commentCount) { commentCount_ = commentCount; }
	void setCommentsURL(const QString &commentsURL) { commentsURL_ = commentsURL; }
	void setTimeEstimateString(const QString &timeEstimateString) { timeEstimateString_ = timeEstimateString; }
	void setAssignee(const QString &assignee) { assignee_ = assignee; }
	void setProjectTrackingDisabled(bool projectTrackingDisabled) { projectTrackingDisabled_ = projectTrackingDisabled; }
	void setInlineIssue(bool inlineIssue) { inlineIssue_ = inlineIssue; }

protected:
	int issueNumber_;
	AMGitHubIssue::ComplexityValue complexityValue_;
	QString title_;
	bool isPullRequest_;
	int originatingIssueNumber_;
	AMGitHubIssue *originatingIssue_;
	int commentCount_;
	QString commentsURL_;
	QString timeEstimateString_;
	QString assignee_;
	bool projectTrackingDisabled_;
	bool inlineIssue_;
	AMGitHubIssue::IssueState issueState_;
};

class AMGitHubIssueFamily : public QObject
{
Q_OBJECT
public:
	enum ComplexityMapping { InvalidComplexityMapping = 0,
				 Complexity1ToComplexity1 = 1, Complexity1ToComplexity2 = 2, Complexity1ToComplexity3 = 3, Complexity1ToComplexity5 = 4, Complexity1ToComplexity8 = 5,
				 Complexity2ToComplexity1 = 6, Complexity2ToComplexity2 = 7, Complexity2ToComplexity3= 7, Complexity2ToComplexity5 = 9, Complexity2ToComplexity8 = 10,
				 Complexity3ToComplexity1 = 11, Complexity3ToComplexity2 = 12, Complexity3ToComplexity3 = 13, Complexity3ToComplexity5 = 14, Complexity3ToComplexity8 = 15,
				 Complexity5ToComplexity1 = 16, Complexity5ToComplexity2 = 17, Complexity5ToComplexity3 = 18, Complexity5ToComplexity5 = 19, Complexity5ToComplexity8 = 20,
				 Complexity8ToComplexity1 = 21, Complexity8ToComplexity2 = 22, Complexity8ToComplexity3 = 23, Complexity8ToComplexity5 = 24, Complexity8ToComplexity8 = 25,
				 ComplexityKToComplexity1 = 26, ComplexityKToComplexity2 = 27, ComplexityKToComplexity3 = 28, ComplexityKToComplexity5 = 29, ComplexityKToComplexity8 = 30 };

	AMGitHubIssueFamily(AMGitHubIssue *originatingIssue = 0, AMGitHubIssue *pullRequestIssue = 0, QObject *parent = 0);
	virtual ~AMGitHubIssueFamily();

	int originatingIssueNumber() const;
	int pullRequestIssueNumber() const;

	AMGitHubIssue* originatingIssue() const { return originatingIssue_; }
	AMGitHubIssue* pullRequestIssue() const { return pullRequestIssue_; }

	AMGitHubIssue::ComplexityValue estimatedComplexity() const;
	AMGitHubIssue::ComplexityValue actualComplexity() const;

	QString timeEstimate() const;

	QString multiLineDebugInfo() const;

	bool completeInfo() const;
	bool totallyIncomplete() const;
	bool onlyMissingActualComplexity() const;
	bool onlyMissingTimeEstimate() const;

	AMGitHubIssueFamily::ComplexityMapping complexityMapping() const;
	bool symmetricComplexityMapping() const;

	double normalizedTimeEstiamte() const;

public slots:
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setPullRequestIssue(AMGitHubIssue *pullRequestIssue) { pullRequestIssue_ = pullRequestIssue; }

protected:
	AMGitHubIssue *originatingIssue_;
	AMGitHubIssue *pullRequestIssue_;
};

#include <QWidget>

class AMGitHubIssueFamilyView : public QWidget
{
Q_OBJECT
public:
	AMGitHubIssueFamilyView(AMGitHubIssueFamily *issueFamily, QWidget *parent = 0);

protected:
	AMGitHubIssueFamily *issueFamily_;
};

class AMGitHubMilestone : public QObject
{
Q_OBJECT
	enum MilestoneState { ClosedState, OpenState, InvalidState };
public:
	AMGitHubMilestone(int number = -1, const QString &title = QString(), AMGitHubMilestone::MilestoneState state = AMGitHubMilestone::InvalidState, QObject *parent = 0);
	AMGitHubMilestone(QVariantMap jsonMap, QObject *parent = 0);

	int number() const { return number_; }
	QString title() const { return title_; }
	AMGitHubMilestone::MilestoneState state() const { return state_; }
	QString stateAsString() const;

	QList<AMGitHubIssue*> associatedIssues() const { return associatedIssues_; }
	QList<AMGitHubIssueFamily*> associatedFamilies() const { return associatedFamilies_; }

public slots:
	void appendAssociatedIssue(AMGitHubIssue *associatedIssue);
	void appendAssociatedFamily(AMGitHubIssueFamily *associatedFamily);

protected:
	int number_;
	QString title_;
	AMGitHubMilestone::MilestoneState state_;

	QList<AMGitHubIssue*> associatedIssues_;
	QList<AMGitHubIssueFamily*> associatedFamilies_;
};

class AMGitHubComplexityManager : public QObject
{
Q_OBJECT
public:
	AMGitHubComplexityManager(QObject *parent = 0);

	double averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue);
	double averageTimeForActualComplexity(AMGitHubIssue::ComplexityValue actualComplexityValue);
	double probableTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue);

public slots:
	void incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping);
	void clearComplexityMappings();

	void addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue);
	void addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue);

protected:
	QVector<int> complexityMappingMatrix_;

	QList<double> estimatedComplexity1Times_;
	QList<double> estimatedComplexity2Times_;
	QList<double> estimatedComplexity3Times_;
	QList<double> estimatedComplexity5Times_;
	QList<double> estimatedComplexity8Times_;
	QList<double> estimatedComplexityKTimes_;
	QList<double> actualComplexity1Times_;
	QList<double> actualComplexity2Times_;
	QList<double> actualComplexity3Times_;
	QList<double> actualComplexity5Times_;
	QList<double> actualComplexity8Times_;
};

#endif // AMRESTACTION_H
