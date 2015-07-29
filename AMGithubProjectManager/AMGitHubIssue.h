#ifndef AMGITHUBISSUE_H
#define AMGITHUBISSUE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QDateTime>

class AMGitHubIssue : public QObject
{
Q_OBJECT
public:
	enum IssueState { ClosedState, OpenState, InvalidState };
	enum ComplexityValue { Complexity1, Complexity2, Complexity3, Complexity5, Complexity8, Complexity13, Complexity21, ComplexityK, InvalidComplexity };

	enum ZenhubComplexityValue { ZenhubComplexity1, ZenhubComplexity2, ZenhubComplexity3, ZenhubComplexity5, ZenhubComplexity8, ZenhubComplexity13, ZenhubComplexity21, ZenhubComplexity40, ZenhubComplexityInvalid };

//	AMGitHubIssue(int issueNumber = -1, AMGitHubIssue::ComplexityValue complexityValue = AMGitHubIssue::InvalidComplexity, const QString &title = "Invalid Title", bool isPullRequest = false, AMGitHubIssue *originatingIssue = 0, int originatingIssueNumber = -1, int commentCount = 0, const QString &commentsURL = "InvalidURL", const QString &timeEstimateString = "Invalid Time Estimate", const QString &assignee = "Invalid Assignee", bool projectTrackingDisabled = false, bool inlineIssue = false, AMGitHubIssue::IssueState issueState = AMGitHubIssue::InvalidState, QObject *parent = 0);
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
	bool isOpen() const { return issueState_ == AMGitHubIssue::OpenState; }
	bool isClosed() const { return issueState_ == AMGitHubIssue::ClosedState; }

	QDateTime createdDate() const { return createdDate_; }
	QDateTime closedDate() const { return closedDate_; }

	static AMGitHubIssue::ComplexityValue complexityFromString(const QString &complexityString);
	static QString stringFromComplexity(AMGitHubIssue::ComplexityValue complexityValue);
	static int integerFromComplexity(AMGitHubIssue::ComplexityValue complexityValue);

	static QString stringFromState(AMGitHubIssue::IssueState issueState);

	QString oneLineDebugInfo() const;
	QString multiLineDebugInfo() const;

	AMGitHubIssue::ZenhubComplexityValue zenhubComplexityValue() const { return zenhubComplexityValue_; }
	static AMGitHubIssue::ZenhubComplexityValue zenhubComplexityFromInteger(int zenhubComplexityAsInteger);
	static int integerFromZenhubComplexityValue(AMGitHubIssue::ZenhubComplexityValue zenhubComplexityValue);

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

	void setZenubComplexityValue(AMGitHubIssue::ZenhubComplexityValue zenhubComplexityValue) { zenhubComplexityValue_ = zenhubComplexityValue; }

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
	QDateTime createdDate_;
	QDateTime closedDate_;

	AMGitHubIssue::ZenhubComplexityValue zenhubComplexityValue_;
};

#endif // AMGITHUBISSUE_H
