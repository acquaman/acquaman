#include "AMExportWizard.h"
#include "dataman/AMExportController.h"

#include <QHashIterator>
#include <QFormLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include "ui/AMFolderPathLineEdit.h"
#include <QFileDialog>
#include <QDir>

#include "acquaman.h"



#include <QDebug>

AMExportWizard::AMExportWizard(AMExportController* controller, QWidget *parent) :
	QWizard(parent)
{
	controller_ = controller;

	addPage(new AMExportWizardChooseExporterPage);
	addPage(new AMExportWizardChooseDestinationFolderPage);

	setWindowTitle(QString("Exporting %1 Scans...").arg(controller_->scanCount()));
}


AMExportWizardChooseExporterPage::AMExportWizardChooseExporterPage(QWidget *parent)
	: QWizardPage(parent)
{
	exporterComboBox_ = new QComboBox();
	destinationFolder_ = new AMFolderPathLineEdit();
	browseButton_ = new QPushButton("Browse...");

	populateExporterComboBox();
	connect(exporterComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterComboBoxIndexChanged(int)));
	connect(browseButton_, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));

	registerField("exporterComboBoxIndex", exporterComboBox_);
	registerField("exportLocation", destinationFolder_);


	QFormLayout* fl = new QFormLayout();
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(destinationFolder_);
	hl->addWidget(browseButton_);

	fl->addRow("Export location", hl);
	fl->addRow("Export as", exporterComboBox_);

	groupBox_ = new QGroupBox();
	descriptionLabel_ = new QLabel();
	descriptionLabel_->setWordWrap(true);
	QVBoxLayout* il = new QVBoxLayout();
	il->addWidget(descriptionLabel_);
	groupBox_->setLayout(il);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addStretch();
	vl->addLayout(fl);
	vl->addStretch();
	vl->addWidget(groupBox_);

	setLayout(vl);

	setTitle("Choose an Export Location and File Format");
	setSubTitle("Acquaman can help you export your data in a variety of file formats.  On the next page, you'll be able to specify more options on how the data is formatted.");

	if(exporterComboBox_->count())
		onExporterComboBoxIndexChanged(0);

}



void AMExportWizardChooseExporterPage::onExporterComboBoxIndexChanged(int index)
{
	groupBox_->setTitle(exporterComboBox_->itemData(index, Qt::DisplayRole).toString());
	descriptionLabel_->setText(exporterComboBox_->itemData(index, AM::DescriptionRole).toString());
}

bool AMExportWizardChooseExporterPage::validatePage()
{
	AMExportController* c = qobject_cast<AMExportWizard*>(wizard())->controller();

	QString exporterClassName = exporterComboBox_->itemData(exporterComboBox_->currentIndex(), AM::UserRole).toString();
	return c->chooseExporter(exporterClassName);
}



void AMExportWizardChooseExporterPage::populateExporterComboBox()
{
	QHashIterator<QString, AMExporterInfo> iRegisteredExporters(AMExportController::registeredExporters());

	while(iRegisteredExporters.hasNext()) {
		iRegisteredExporters.next();
		const AMExporterInfo& exporter = iRegisteredExporters.value();
		exporterComboBox_->addItem(exporter.description); /// \todo Icon
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, exporter.longDescription, AM::DescriptionRole);
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, iRegisteredExporters.key(), AM::UserRole);
	}
}

void AMExportWizardChooseExporterPage::initializePage()
{
	if(destinationFolder_->text().isEmpty()) {
		AMExportController* c = qobject_cast<AMExportWizard*>(wizard())->controller();
		destinationFolder_->setText(c->destinationFolderPath());
	}
}

void AMExportWizardChooseExporterPage::onBrowseButtonClicked()
{
	destinationFolder_->setText(QFileDialog::getExistingDirectory(this, "Export Location", destinationFolder_->text()));
}


AMExportWizardChooseDestinationFolderPage::AMExportWizardChooseDestinationFolderPage(QWidget *parent)
{

}

bool AMExportWizardChooseDestinationFolderPage::validatePage()
{
	return true;
}
