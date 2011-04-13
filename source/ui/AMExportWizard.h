#ifndef AMEXPORTWIZARD_H
#define AMEXPORTWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>


class AMExportWizardChooseExporterPage : public QWizardPage {
	Q_OBJECT
public:
	explicit AMExportWizardChooseExporterPage(QWidget* parent = 0);

protected slots:
	/// Displays the long-form description message when the exporter is changed.
	void onExporterComboBoxIndexChanged(int index);


protected:
	/// ComboBox to choose a file format ("exporter")
	QComboBox* exporterComboBox_;
	/// Text (long description) of the exporter and its capabilities
	QGroupBox* groupBox_;
	QLabel* descriptionLabel_;

};

class AMExportController;

class AMExportWizard : public QWizard
{
	Q_OBJECT
public:
	explicit AMExportWizard(AMExportController* controller, QWidget *parent = 0);

signals:

public slots:

protected:
	AMExportController* controller_;

};

#endif // AMEXPORTWIZARD_H
