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


#ifndef AMEXPORTWIZARD_H
#define AMEXPORTWIZARD_H

#include <QWizard>
#include <QWizardPage>

class QComboBox;
class QGroupBox;
class QLabel;
class QProgressBar;
class AMFolderPathLineEdit;
class QPushButton;
class AMExporterOption;
class AMExporter;
class AMExportController;


/// This widget provides the final screen in an export wizard, showing a status view and progress bar as the scans are exported, and providing a cancel button to interrupt the process.
class AMExportWizardProgressPage : public QWizardPage {
	Q_OBJECT

public:
 	virtual ~AMExportWizardProgressPage();
	explicit AMExportWizardProgressPage(QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();
	virtual bool isComplete() const;

protected slots:
	void onControllerStateChanged(int);
	void onControllerProgressChanged(int, int);

protected:
	AMExportController* controller_;

	QProgressBar* progressBar_;
	QLabel* progressLabel_;
	QLabel* statusLabel_;
};

/// This widget provides the second screen in an export wizard, allowing the user to configure the options for the exporter they selected in AMExportWizardChooseExporterPage.
class AMExportWizardOptionPage : public QWizardPage {
	Q_OBJECT

public:
 	virtual ~AMExportWizardOptionPage();
	explicit AMExportWizardOptionPage(QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();

public slots:
	/// Sets the list of databases we'll look in for exporter options
	void setOptionsDatabases(const QStringList &optionsDatabases);

protected slots:
	/// Called when the user chooses a different pre-saved option from the selector list.
	void onOptionSelectorIndexChanged(int index);

	/// Called when the save button is clicked to save a current option.
	bool onSaveOptionButtonClicked();

	/// Called when the "save as" button is clicked to make a copy of the current option
	bool onSaveAsOptionButtonClicked();

	/// Fills the option selector with available stored options. Returns true if saved, false if cancelled.
	void populateOptionSelector();

protected:
	QWidget* optionViewContainer_;
	QComboBox* optionSelector_;
	QPushButton* saveOptionButton_, *newOptionButton_, *saveAsOptionButton_;

	AMExporterOption* option_;
	AMExporter* exporter_;
	QWidget* optionView_;
	AMExportController* controller_;

	QStringList optionsDatabases_;
};

/// This widget provides the first screen in an export wizard, allowing the user to choose an exporter (General Ascii, Excel, etc.) from the set of registered exporters.
class AMExportWizardChooseExporterPage : public QWizardPage {
	Q_OBJECT
public:
 	virtual ~AMExportWizardChooseExporterPage();
	explicit AMExportWizardChooseExporterPage(QWidget* parent = 0);

	virtual bool validatePage();
	virtual void initializePage();

protected slots:
	/// Displays the long-form description message when the exporter is changed.
	void onExporterComboBoxIndexChanged(int index);
	/// Open a file browse when the user clicks the browse button
	void onBrowseButtonClicked();


protected:
	/// ComboBox to choose a file format ("exporter")
	QComboBox* exporterComboBox_;
	/// Destination folder entry box
	AMFolderPathLineEdit* destinationFolder_;
	QPushButton* browseButton_;

	/// Text (long description) of the exporter and its capabilities
	QGroupBox* groupBox_;
	QLabel* descriptionLabel_;

	/// Helper function to fill the combo box with available exporters
	void populateExporterComboBox();

};



class AMExportWizard : public QWizard
{
	Q_OBJECT
public:
	explicit AMExportWizard(AMExportController* controller, QWidget *parent = 0);

	AMExportController* controller() { return controller_; }

	QStringList optionsDatabases() const { return optionsDatabases_; }

signals:

public slots:
	void setOptionsDatabases(const QStringList &optionsDatabases);

protected slots:
	void onControllerDeleted() {
		controller_ = 0;
	}

	virtual void done(int result);

protected:
	AMExportController* controller_;

	AMExportWizardOptionPage *optionsPage_;

	QStringList optionsDatabases_;

};

#endif // AMEXPORTWIZARD_H
