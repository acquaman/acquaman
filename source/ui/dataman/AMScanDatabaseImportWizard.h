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
 	virtual ~AMScanDatabaseImportWizardChooseSourcePage();
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
 	virtual ~AMScanDatabaseImportWizardReviewRunsPage();
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
 	virtual ~AMScanDatabaseImportWizardReviewExperimentsPage();
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
 	virtual ~AMScanDatabaseImportWizardReviewSamplesPage();
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
 	virtual ~AMScanDatabaseImportWizardDoImportPage();
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
