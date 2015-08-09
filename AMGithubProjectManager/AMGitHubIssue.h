#ifndef AMGITHUBISSUE_H
#define AMGITHUBISSUE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QDateTime>

class AMGitHubComplexityMapping;

class AMGitHubIssue : public QObject
{
Q_OBJECT
public:
	enum IssueState { ClosedState, OpenState, InvalidState };
	enum ActualComplexityValue { ActualComplexity1, ActualComplexity2, ActualComplexity3, ActualComplexity5, ActualComplexity8, ActualComplexity13, ActualComplexity21, ActualComplexity40, ActualComplexityInvalid };
	enum EstimatedComplexityValue { EstimatedComplexity1, EstimatedComplexity2, EstimatedComplexity3, EstimatedComplexity5, EstimatedComplexity8, EstimatedComplexity13, EstimatedComplexity21, EstimatedComplexity40, EstimatedComplexityInvalid };

	AMGitHubIssue(QVariantMap jsonMap, QObject *parent = 0);
	virtual ~AMGitHubIssue();

	int issueNumber() const { return issueNumber_; }
	QString title() const { return title_; }
	QString assignee() const { return assignee_; }

	AMGitHubIssue::ActualComplexityValue actualComplexityValue() const { return actualComplexityValue_; }
	bool validActualComplexity() const;
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue() const { return estimatedComplexityValue_; }
	bool validEstimatedComplexity() const;
	QString timeEstimateString() const { return timeEstimateString_; }

	bool isPullRequest() const { return isPullRequest_; }
	AMGitHubIssue* originatingIssue() const { return originatingIssue_; }
	int originatingIssueNumber() const { return originatingIssueNumber_; }

	int commentCount() const { return commentCount_; }
	QString commentsURL() const { return commentsURL_; }

	bool projectTrackingDisabled() const { return projectTrackingDisabled_; }
	bool inlineIssue() const { return inlineIssue_; }

	bool issueCompletelyUntracked() const;
	bool issueTrackedWithoutEstimates() const;
	bool completeIssue() const;

	AMGitHubIssue::IssueState issueState() const { return issueState_; }
	bool isOpen() const { return issueState_ == AMGitHubIssue::OpenState; }
	bool isClosed() const { return issueState_ == AMGitHubIssue::ClosedState; }

	const AMGitHubComplexityMapping* complexityMapping() const { return complexityMapping_; }

	QDateTime createdDate() const { return createdDate_; }
	QDateTime closedDate() const { return closedDate_; }

	double normalizedTimeEstimate() const;

	QString oneLineDebugInfo() const;
	QString multiLineDebugInfo() const;

	static AMGitHubIssue::ActualComplexityValue actualComplexityFromString(const QString &actualComplexityString);
	static QString stringFromActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue);
	static int integerFromActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue);

	static AMGitHubIssue::EstimatedComplexityValue estimatedComplexityFromInteger(int estimatedComplexityAsInteger);
	static int integerFromEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);

	static QString stringFromState(AMGitHubIssue::IssueState issueState);

public slots:
	void setIssueNumber(int issueNumber) { issueNumber_ = issueNumber; }
	void setTitle(const QString &title) { title_ = title; }
	void setAssignee(const QString &assignee) { assignee_ = assignee; }

	void setActualComplexityValue(AMGitHubIssue::ActualComplexityValue actualComplexityValue);
	void setEstimatedComplexityValue(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);
	void setTimeEstimateString(const QString &timeEstimateString) { timeEstimateString_ = timeEstimateString; }

	void setIsPullRequest(bool isPullRequest) { isPullRequest_ = isPullRequest; }
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setOriginatingIssueNumber(int originatingIssueNumber) { originatingIssueNumber_ = originatingIssueNumber; }

	void setCommentCount(int commentCount) { commentCount_ = commentCount; }
	void setCommentsURL(const QString &commentsURL) { commentsURL_ = commentsURL; }

	void setProjectTrackingDisabled(bool projectTrackingDisabled) { projectTrackingDisabled_ = projectTrackingDisabled; }
	void setInlineIssue(bool inlineIssue) { inlineIssue_ = inlineIssue; }

protected:
	int issueNumber_;
	QString title_;
	QString assignee_;

	AMGitHubIssue::ActualComplexityValue actualComplexityValue_;
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue_;
	QString timeEstimateString_;

	bool isPullRequest_;
	AMGitHubIssue *originatingIssue_;
	int originatingIssueNumber_;

	int commentCount_;
	QString commentsURL_;

	bool projectTrackingDisabled_;
	bool inlineIssue_;

	AMGitHubIssue::IssueState issueState_;

	AMGitHubComplexityMapping *complexityMapping_;

	QDateTime createdDate_;
	QDateTime closedDate_;
};

class AMGitHubComplexityMapping : public QObject
{
Q_OBJECT
public:
	AMGitHubComplexityMapping(int mappingIndex, QObject *parent = 0);
	AMGitHubComplexityMapping(AMGitHubIssue *issue, QObject *parent = 0);
	AMGitHubComplexityMapping(AMGitHubIssue::ActualComplexityValue actualComplexityValue, AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, QObject *parent = 0);

	AMGitHubIssue::ActualComplexityValue actualComplexityValue() const { return actualComplexityValue_; }
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue() const { return estimatedComplexityValue_; }

	int mappingIndex() const;
	bool validMapping() const { return validMapping_; }

	static AMGitHubIssue::ActualComplexityValue actualComplexityValueFromMappingIndex(int index);
	static AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValueFromMappingIndex(int index);

	static bool validMappingIndex(int mappingIndex);
	static bool validComplexityValues(AMGitHubIssue::ActualComplexityValue actualComplexityValue, AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);

protected:
	AMGitHubIssue::ActualComplexityValue actualComplexityValue_;
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue_;

	bool validMapping_;
};

#endif // AMGITHUBISSUE_H
