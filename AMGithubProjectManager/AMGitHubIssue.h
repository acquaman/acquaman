#ifndef AMGITHUBISSUE_H
#define AMGITHUBISSUE_H

#include <QObject>
#include <QString>
#include <QStringList>
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

	QVariantMap toMap() const;
	QByteArray toJSON() const;

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

	/// No tracking whatsoever (really old issues)
	bool issueCompletelyUntracked() const;
	/// Tracked but no Estimate/Actual/Time info (old pull requests)
	bool issueTrackedWithoutEstimates() const;
	/// Issue SHOULD have full Estimate/Actual/Time info
	bool fullyTrackedIssue() const;
	/// Issue has full Estimate/Actual/Time info
	bool completeIssue() const;

	/// Open issues missing valid estimate and closed issues that should have an estimate but are missing one
	bool issueMissingEstimateComplexity() const;
	/// Closed issues that should have an actual but are missing one
	bool issueMissingActualComplexity() const;
	/// Closed issues that should have a time report but are missing one
	bool issueMissingTimeReporting() const;

	/// Open issues with estimate, closed issues with full Estimate/Actual/Time info, and closed issues that are partially tracked
	bool issueFullySpecifiedForState() const;

	AMGitHubIssue::IssueState issueState() const { return issueState_; }
	bool isOpen() const { return issueState_ == AMGitHubIssue::OpenState; }
	bool isClosed() const { return issueState_ == AMGitHubIssue::ClosedState; }

	const AMGitHubComplexityMapping* complexityMapping() const { return complexityMapping_; }

	QDateTime createdDate() const { return createdDate_; }
	QDateTime closedDate() const { return closedDate_; }

	double normalizedTimeEstimate() const;

	QStringList rawLabels() const { return rawLabels_; }

	QString oneLineDebugInfo() const;
	QString multiLineDebugInfo() const;

	static AMGitHubIssue::ActualComplexityValue actualComplexityFromString(const QString &actualComplexityString);
	static QString stringFromActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue);
	static int integerFromActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue);

	static AMGitHubIssue::EstimatedComplexityValue estimatedComplexityFromInteger(int estimatedComplexityAsInteger);
	static int integerFromEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);

	static QString stringFromState(AMGitHubIssue::IssueState issueState);

	static AMGitHubIssue::EstimatedComplexityValue correspondingEstimatedComplexity(AMGitHubIssue::ActualComplexityValue actualComplexity);
	static AMGitHubIssue::ActualComplexityValue correspondingActualComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexity);

public slots:
	void setIssueNumber(int issueNumber) { issueNumber_ = issueNumber; }
	void setTitle(const QString &title) { title_ = title; }
	void setAssignee(const QString &assignee) { assignee_ = assignee; }

	void setActualComplexityValue(AMGitHubIssue::ActualComplexityValue actualComplexityValue);
	void setEstimatedComplexityValue(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);
	void setTimeEstimateString(const QString &timeEstimateString);

	void setIsPullRequest(bool isPullRequest) { isPullRequest_ = isPullRequest; }
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setOriginatingIssueNumber(int originatingIssueNumber) { originatingIssueNumber_ = originatingIssueNumber; }

	void setCommentCount(int commentCount) { commentCount_ = commentCount; }
	void setCommentsURL(const QString &commentsURL) { commentsURL_ = commentsURL; }

	void setProjectTrackingDisabled(bool projectTrackingDisabled) { projectTrackingDisabled_ = projectTrackingDisabled; }
	void setInlineIssue(bool inlineIssue) { inlineIssue_ = inlineIssue; }

	void resetFromMap(const QVariantMap &map);
	void resetFromJSON(const QByteArray &json);

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

	mutable double normalizedTimeEstimate_;

	QStringList rawLabels_;
};

Q_DECLARE_METATYPE(AMGitHubIssue::ActualComplexityValue)
Q_DECLARE_METATYPE(AMGitHubIssue::EstimatedComplexityValue)
Q_DECLARE_METATYPE(AMGitHubIssue::IssueState)

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

	bool symmetricComplexityMapping() const;

	static AMGitHubIssue::ActualComplexityValue actualComplexityValueFromMappingIndex(int index);
	static AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValueFromMappingIndex(int index);

	static bool validMappingIndex(int mappingIndex);
	static bool validComplexityValues(AMGitHubIssue::ActualComplexityValue actualComplexityValue, AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue);
	static int maximumIndex();

protected:
	AMGitHubIssue::ActualComplexityValue actualComplexityValue_;
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue_;

	bool validMapping_;
};

#endif // AMGITHUBISSUE_H
