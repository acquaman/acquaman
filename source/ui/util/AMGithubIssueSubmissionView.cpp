/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMGithubIssueSubmissionView.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>

 AMGithubIssueSubmissionView::~AMGithubIssueSubmissionView(){}
AMGithubIssueSubmissionView::AMGithubIssueSubmissionView(QWidget *parent)
	: QDialog(parent)
{
	issueCreatedSuccessfully_ = false;
	exitCountDownTimer_ = 0;
	issuesTypesAndAssignees_.setAllowsDuplicateKeys(true);
	issuesTypesAndAssignees_.append("I think it's a general issue", "AcquamanIssues");

	issueTitleEdit_ = new QLineEdit();
	issueTitleEdit_->setEnabled(false);

	issueBodyEdit_ = new QTextEdit();
	issueBodyEdit_->setEnabled(false);

	issueTypeComboBox_ = new QComboBox();
	issueTypeComboBox_->addItems(QStringList() << issuesTypesAndAssignees_.at(0));
	issueTypeComboBox_->setEnabled(false);

	submitIssuesButton_ = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Submit");
	submitIssuesButton_->setEnabled(false);

	cancelButton_ = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");

	waitingBar_ = new QProgressBar();
	waitingBar_->setMinimum(0);
	waitingBar_->setMaximum(0);
	waitingBar_->setMinimumWidth(200);

	messageLabel_ = new QLabel();

	QHBoxLayout *messageVL = new QHBoxLayout();
	messageVL->addWidget(messageLabel_, 0, Qt::AlignCenter);
	messageVL->addWidget(waitingBar_, 0, Qt::AlignCenter);

	QHBoxLayout *topHL = new QHBoxLayout();
	topHL->addWidget(new QLabel("Issue About:"));
	topHL->addWidget(issueTypeComboBox_);

	QVBoxLayout *fl = new QVBoxLayout();
	fl->addLayout(topHL);
	fl->addWidget(new QLabel("Title"), 0, Qt::AlignLeft);
	fl->addWidget(issueTitleEdit_);
	fl->addWidget(new QLabel("Description"), 0, Qt::AlignLeft);
	fl->addWidget(issueBodyEdit_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(submitIssuesButton_);
	hl->addWidget(cancelButton_);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addLayout(fl);
	vl->addLayout(messageVL);
	vl->addLayout(hl);

	setLayout(vl);

	messageLabel_->setText("Authenticating ...");
	issueManager_ = new AMGithubManager("AcquamanIssues", "sucking2report", "acquaman/acquaman", this);
	connect(issueManager_, SIGNAL(authenticated(bool)), this, SLOT(onGitAuthenticated(bool)));

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(submitIssuesButton_, SIGNAL(clicked()), this, SLOT(onSubmitIssueButtonClicked()));

	connect(issueTitleEdit_, SIGNAL(textEdited(QString)), this, SLOT(onEditsChanged()));
	connect(issueBodyEdit_, SIGNAL(textChanged()), this, SLOT(onEditsChanged()));
}

void AMGithubIssueSubmissionView::addIssueType(const QString &issueType, const QString &assigneeForType)
{
	issuesTypesAndAssignees_.append(issueType, assigneeForType);
	issueTypeComboBox_->addItems(QStringList() << issuesTypesAndAssignees_.at(issuesTypesAndAssignees_.count()-1));
}

void AMGithubIssueSubmissionView::onCancelButtonClicked()
{
	hideAndFinish();
}

void AMGithubIssueSubmissionView::onSubmitIssueButtonClicked()
{
	QString assignee = QString();

	if(issueTypeComboBox_->currentIndex() != 0)
		assignee = issuesTypesAndAssignees_.keyAt(issueTypeComboBox_->currentIndex());

	waitingBar_->show();
	messageLabel_->show();
	submitIssuesButton_->setEnabled(false);

	messageLabel_->setText("Submitting Issue...");
	issueManager_->createNewIssue(issueTitleEdit_->text(), issueBodyEdit_->document()->toPlainText(), assignee);
	connect(issueManager_, SIGNAL(issueCreated(bool)), this, SLOT(onGitIssueCreated(bool)));
}

void AMGithubIssueSubmissionView::onGitAuthenticated(bool authenticated)
{
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

void AMGithubIssueSubmissionView::onGitIssueCreated(bool issueCreated)
{
	if(issueCreated){

		issueCreatedSuccessfully_ = true;
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

void AMGithubIssueSubmissionView::onEditsChanged()
{
	if(!issueTitleEdit_->text().isEmpty() && !issueBodyEdit_->document()->toPlainText().isEmpty() && !issueCreatedSuccessfully_)
		submitIssuesButton_->setEnabled(true);
	else
		submitIssuesButton_->setEnabled(false);
}

void AMGithubIssueSubmissionView::onExitCountDownTimeout()
{
	if(exitCountDownCounter_ == 5){

		hideAndFinish();
		return;
	}

	QString goodbyeMessage = QString("Thanks for submitting your issue\nWe appreciate your help\n(Closing this window in %1 seconds)").arg(5-exitCountDownCounter_);
	issueBodyEdit_->setText(goodbyeMessage);
	exitCountDownCounter_++;
}

void AMGithubIssueSubmissionView::hideAndFinish()
{
	hide();
	emit finished();
}

#include <QCloseEvent>
void AMGithubIssueSubmissionView::closeEvent(QCloseEvent *e)
{
	e->accept();
	hideAndFinish();
}
