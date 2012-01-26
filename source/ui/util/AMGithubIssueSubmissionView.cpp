#include "AMGithubIssueSubmissionView.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>

AMGithubIssueSubmissionView::AMGithubIssueSubmissionView(QWidget *parent) :
	QWidget(parent)
{
	exitCountDownTimer_ = 0;
	issuesTypesAndAssignees_.setAllowsDuplicateKeys(true);
	issuesTypesAndAssignees_.append("I think it's a general issue", "AcquamanIssues");

	issueTitleEdit_ = new QLineEdit();
	issueTitleEdit_->setEnabled(false);
	issueBodyEdit_ = new QTextEdit();
	issueBodyEdit_->setEnabled(false);
	issueTypeComboBox_ = new QComboBox();
	QStringList issuesTypes;
	issuesTypes << issuesTypesAndAssignees_.at(0);
	issueTypeComboBox_->addItems(issuesTypes);
	issueTypeComboBox_->setEnabled(false);
	submitIssuesButton_ = new QPushButton("Submit");
	submitIssuesButton_->setEnabled(false);
	cancelButton_ = new QPushButton("Cancel");
	waitingBar_ = new QProgressBar();
	waitingBar_->setMinimum(0);
	waitingBar_->setMaximum(0);
	messageLabel_ = new QLabel();

	QVBoxLayout *messageVL = new QVBoxLayout();
	messageVL->addWidget(waitingBar_);
	messageVL->addWidget(messageLabel_);

	QHBoxLayout *topHL = new QHBoxLayout();
	topHL->addWidget(issueTypeComboBox_);
	topHL->addLayout(messageVL);

	QFormLayout *fl = new QFormLayout();
	fl->addRow("Issue About:", topHL);
	fl->addRow("Title", issueTitleEdit_);
	fl->addRow("Description", issueBodyEdit_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(cancelButton_);
	hl->addWidget(submitIssuesButton_);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addLayout(fl);
	vl->addLayout(hl);

	setLayout(vl);

	messageLabel_->setText("Authenticating ...");
	issueManager_ = new AMGithubManager("AcquamanIssues", "sucking2report", "acquaman", this);
	connect(issueManager_, SIGNAL(authenticated(bool)), this, SLOT(onGitAuthenticated(bool)));

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(submitIssuesButton_, SIGNAL(clicked()), this, SLOT(onSubmitIssueButtonClicked()));

	connect(issueTitleEdit_, SIGNAL(textEdited(QString)), this, SLOT(onEditsChanged()));
	connect(issueBodyEdit_, SIGNAL(textChanged()), this, SLOT(onEditsChanged()));
}

void AMGithubIssueSubmissionView::addIssueType(const QString &issueType, const QString &assigneeForType){
	issuesTypesAndAssignees_.append(issueType, assigneeForType);
	QStringList issuesTypes;
	issuesTypes << issuesTypesAndAssignees_.at(issuesTypesAndAssignees_.count()-1);
	issueTypeComboBox_->addItems(issuesTypes);
}

void AMGithubIssueSubmissionView::onCancelButtonClicked(){
	hideAndFinish();
}

void AMGithubIssueSubmissionView::onSubmitIssueButtonClicked(){
	QString assignee = QString();
	if(issueTypeComboBox_->currentIndex() != 0)
		assignee = issuesTypesAndAssignees_.keyAt(issueTypeComboBox_->currentIndex());
	waitingBar_->show();
	messageLabel_->show();
	messageLabel_->setText("Submitting Issue...");
	issueManager_->createNewIssue(issueTitleEdit_->text(), issueBodyEdit_->document()->toPlainText(), assignee);
	connect(issueManager_, SIGNAL(issueCreated(bool)), this, SLOT(onGitIssueCreated(bool)));
}

void AMGithubIssueSubmissionView::onGitAuthenticated(bool authenticated){
	if(authenticated){
		issueTitleEdit_->setEnabled(true);
		issueBodyEdit_->setEnabled(true);
		issueTypeComboBox_->setEnabled(true);
		waitingBar_->hide();
		messageLabel_->hide();
	}
	else{
		issueTitleEdit_->setEnabled(false);
		issueBodyEdit_->setEnabled(false);
		issueTypeComboBox_->setEnabled(false);
		submitIssuesButton_->setEnabled(false);
		waitingBar_->show();
		messageLabel_->show();
		messageLabel_->setText("Authentication Failed");
	}

}

void AMGithubIssueSubmissionView::onGitIssueCreated(bool issueCreated){
	if(issueCreated){
		waitingBar_->setMaximum(1);
		waitingBar_->setValue(1);
		messageLabel_->setText("Issue Submitted");

		issueTitleEdit_->setEnabled(false);
		issueBodyEdit_->setEnabled(false);
		issueTypeComboBox_->setEnabled(false);
		submitIssuesButton_->setEnabled(false);
		cancelButton_->setEnabled(false);

		exitCountDownCounter_ = 0;
		exitCountDownTimer_ = new QTimer(this);
		connect(exitCountDownTimer_, SIGNAL(timeout()), this, SLOT(onExitCountDownTimeout()));
		exitCountDownTimer_->start(1000);
		onExitCountDownTimeout();
	}
	else{
		waitingBar_->show();
		messageLabel_->show();
		messageLabel_->setText("Could not create issue");
	}
}

void AMGithubIssueSubmissionView::onEditsChanged(){
	if(!issueTitleEdit_->text().isEmpty() && !issueBodyEdit_->document()->toPlainText().isEmpty())
		submitIssuesButton_->setEnabled(true);
	else
		submitIssuesButton_->setEnabled(false);
}

void AMGithubIssueSubmissionView::onExitCountDownTimeout(){
	if(exitCountDownCounter_ == 5){
		hideAndFinish();
		return;
	}
	QString goodbyeMessage = QString("Thanks for submitting your issue\nWe appreciate your help\n(Closing this window in %1 seconds)").arg(5-exitCountDownCounter_);
	issueBodyEdit_->setText(goodbyeMessage);
	exitCountDownCounter_++;
}

void AMGithubIssueSubmissionView::hideAndFinish(){
	hide();
	emit finished();
}

#include <QCloseEvent>
void AMGithubIssueSubmissionView::closeEvent(QCloseEvent *e){
	e->accept();
	hideAndFinish();
	return;
}
