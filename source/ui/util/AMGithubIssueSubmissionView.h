/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMGITHUBISSUESUBMISSIONVIEW_H
#define AMGITHUBISSUESUBMISSIONVIEW_H

#include <QDialog>
#include "util/AMOrderedSet.h"
#include "util/AMGithubManager.h"

class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
class QLabel;
class QTimer;

class AMGithubIssueSubmissionView : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the standard Github submission view dialog.
 	virtual ~AMGithubIssueSubmissionView();
	AMGithubIssueSubmissionView(QWidget *parent = 0);

public slots:
	/// Adds to the end of the issue type list a new issue type and who would be assigned to it.
	void addIssueType(const QString &issueType, const QString &assigneeForType);

signals:
	/// Notifier that the creation of a new issue has finished (whether it is successful or not).
	void finished();

protected slots:
	/// Helper slot handling the cancel.
	void onCancelButtonClicked();
	/// Helper slot that handles when a request to submit a new issue is submitted.
	void onSubmitIssueButtonClicked();

	/// Helper slot that handles when the setting up the dialog based on whether its successfully authenticated or not to Github.
	void onGitAuthenticated(bool authenticated);
	/// Helper slot handling the behaviour of hte dialog if a new issue is created or not.
	void onGitIssueCreated(bool issueCreated);

	/// Handles enabling the submit button based on whether there is text in the title and body of the issue.
	void onEditsChanged();

	/// Handles updating the small widget that says thanks to the user for their input and after 5 seconds it closes itself.
	void onExitCountDownTimeout();
	/// Handles hiding the dialog and emits the finished signal.
	void hideAndFinish();

protected:
	virtual void closeEvent(QCloseEvent *);

protected:
	AMGithubManager *issueManager_;
	AMOrderedSet<QString, QString> issuesTypesAndAssignees_;

	QLineEdit *issueTitleEdit_;
	QTextEdit *issueBodyEdit_;
	QComboBox *issueTypeComboBox_;
	QPushButton *submitIssuesButton_;
	QPushButton *cancelButton_;

	QProgressBar *waitingBar_;
	QLabel *messageLabel_;

	QTimer *exitCountDownTimer_;
	int exitCountDownCounter_;

	bool issueCreatedSuccessfully_;
};

#endif // AMGITHUBISSUESUBMISSIONVIEW_H
