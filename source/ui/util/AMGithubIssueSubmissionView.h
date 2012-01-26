#ifndef AMGITHUBISSUESUBMISSIONVIEW_H
#define AMGITHUBISSUESUBMISSIONVIEW_H

#include <QWidget>
#include "util/AMOrderedSet.h"
#include "util/AMGithubManager.h"

class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
class QLabel;
class QTimer;

class AMGithubIssueSubmissionView : public QWidget
{
Q_OBJECT

public:
	AMGithubIssueSubmissionView(QWidget *parent = 0);

public slots:
	void addIssueType(const QString &issueType, const QString &assigneeForType);

signals:
	void finished();

protected slots:
	void onCancelButtonClicked();
	void onSubmitIssueButtonClicked();

	void onGitAuthenticated(bool authenticated);
	void onGitIssueCreated(bool issueCreated);

	void onEditsChanged();

	void onExitCountDownTimeout();
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
};

#endif // AMGITHUBISSUESUBMISSIONVIEW_H
