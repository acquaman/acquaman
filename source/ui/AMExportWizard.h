#ifndef AMEXPORTWIZARD_H
#define AMEXPORTWIZARD_H

#include <QWizard>
#include <QWizardPage>

class QComboBox;
class QGroupBox;
class QLabel;
class AMFolderPathLineEdit;
class QPushButton;
class AMExporterOption;
class AMExporter;

class AMExportWizardOptionPage : public QWizardPage {
	Q_OBJECT

public:
	explicit AMExportWizardOptionPage(QWidget* parent = 0);

	virtual void initializePage();
	virtual bool validatePage();

protected slots:
	/// Called when the user chooses a different pre-saved option from the selector list.
	void onOptionSelectorIndexChanged(int index);

	/// Called when the save button is clicked to save a current option.
	bool onSaveOptionButtonClicked();

	/// Fills the option selector with available stored options. Returns true if saved, false if cancelled.
	void populateOptionSelector();

protected:
	QWidget* optionViewContainer_;
	QComboBox* optionSelector_;
	QPushButton* saveOptionButton_, *newOptionButton_;

	AMExporterOption* option_;
	AMExporter* exporter_;
	QWidget* optionView_;


};

class AMExportWizardChooseExporterPage : public QWizardPage {
	Q_OBJECT
public:
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

class AMExportController;

class AMExportWizard : public QWizard
{
	Q_OBJECT
public:
	explicit AMExportWizard(AMExportController* controller, QWidget *parent = 0);

	AMExportController* controller() { return controller_; }

signals:

public slots:

protected:
	AMExportController* controller_;

};

#endif // AMEXPORTWIZARD_H
