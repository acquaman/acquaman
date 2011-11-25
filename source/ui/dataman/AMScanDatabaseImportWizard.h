#ifndef AMSCANDATABASEIMPORTWIZARD_H
#define AMSCANDATABASEIMPORTWIZARD_H

#include <QWizard>
#include "dataman/import/AMScanDatabaseImportController.h"

class QProgressBar;
class QLabel;
class QLineEdit;
class QListWidget;

class AMScanDatabaseImportWizardChooseSourcePage : public QWizardPage {
	Q_OBJECT
public:
	AMScanDatabaseImportWizardChooseSourcePage(AMScanDatabaseImportController* controller, QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();
	virtual bool isComplete() const { return isComplete_; }

protected slots:
	void displaySourcePathDialog();
	void reviewComplete();

protected:
	QProgressBar* progressBar_;
	QLabel* progressLabel_;

	QLineEdit* sourcePathLineEdit_;
	QPushButton* browseButton_;
	QLabel* feedbackLabel_;

	bool isComplete_;
	AMScanDatabaseImportController* controller_;
};

class AMScanDatabaseImportWizardReviewRunsPage : public QWizardPage {
	Q_OBJECT
public:
	AMScanDatabaseImportWizardReviewRunsPage(AMScanDatabaseImportController* controller, QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();

protected slots:

protected:
	QProgressBar* progressBar_;
	QLabel* progressLabel_;

	QListWidget* runsFoundList_, *runsToMergeList_;
	AMScanDatabaseImportController* controller_;
};

class AMScanDatabaseImportWizardReviewExperimentsPage : public QWizardPage {
	Q_OBJECT
public:
	AMScanDatabaseImportWizardReviewExperimentsPage(AMScanDatabaseImportController* controller, QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();

protected slots:

protected:
	QProgressBar* progressBar_;
	QLabel* progressLabel_;

	QListWidget* itemsFoundList_, *itemsToMergeList_;
	AMScanDatabaseImportController* controller_;
};


class AMScanDatabaseImportWizardReviewSamplesPage : public QWizardPage {
	Q_OBJECT
public:
	AMScanDatabaseImportWizardReviewSamplesPage(AMScanDatabaseImportController* controller, QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();

protected slots:

protected:
	QProgressBar* progressBar_;
	QLabel* progressLabel_;

	QListWidget* itemsFoundList_, *itemsToMergeList_;
	AMScanDatabaseImportController* controller_;
};

class AMScanDatabaseImportWizardDoImportPage : public QWizardPage {
	Q_OBJECT
public:
	AMScanDatabaseImportWizardDoImportPage(AMScanDatabaseImportController* controller, QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();
	virtual bool isComplete() const;

protected slots:
	void startImport();

protected:
	QProgressBar* progressBar_;
	QLabel* progressLabel_;

	AMScanDatabaseImportController* controller_;

};


class AMScanDatabaseImportWizard : public QWizard
{
	Q_OBJECT
public:
	explicit AMScanDatabaseImportWizard(AMScanDatabaseImportController* controller, QWidget *parent = 0);

	AMScanDatabaseImportController* controller() { return controller_; }

signals:

public slots:

protected slots:

	virtual void done(int result);

protected:
	AMScanDatabaseImportController* controller_;

};

#endif // AMSCANDATABASEIMPORTWIZARD_H
