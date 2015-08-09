#include "AMGitHubIssueFamilyView.h"

#include <QFormLayout>
#include <QLabel>

AMGitHubIssueFamilyView::AMGitHubIssueFamilyView(AMGitHubIssueFamily *issueFamily, QWidget *parent) :
	QWidget(parent)
{
	issueFamily_ = issueFamily;
	if(issueFamily){
		QFormLayout *fl = new QFormLayout();

		if(issueFamily_->originatingIssue())
			fl->addRow("Original:", new QLabel(QString("%1").arg(issueFamily_->originatingIssueNumber())));
		else
			fl->addRow("Original:", new QLabel("MISSING"));

		if(issueFamily_->pullRequestIssue())
			fl->addRow("Pull Request:", new QLabel(QString("%1").arg(issueFamily_->pullRequestIssueNumber())));
		else
			fl->addRow("Pull Request:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->estimatedComplexity() != AMGitHubIssue::ActualComplexityInvalid)
			fl->addRow("Estimated Complexity:", new QLabel(AMGitHubIssue::stringFromActualComplexity(issueFamily_->estimatedComplexity())));
		else
			fl->addRow("Estimated Complexity:", new QLabel("MISSING"));

		if(issueFamily_->pullRequestIssue() && issueFamily_->actualComplexity() != AMGitHubIssue::ActualComplexityInvalid)
			fl->addRow("Actual Complexity:", new QLabel(AMGitHubIssue::stringFromActualComplexity(issueFamily_->actualComplexity())));
		else
			fl->addRow("Actual Complexity:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->timeEstimate() != "Invalid Time Estimated" && !issueFamily_->timeEstimate().isEmpty())
			fl->addRow("Time Estimate:", new QLabel(issueFamily_->timeEstimate()));
		else
			fl->addRow("Time Estimate:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
			fl->addRow("State:", new QLabel("Closed"));
		else
			fl->addRow("State:", new QLabel("Open"));

		setLayout(fl);
	}
}
