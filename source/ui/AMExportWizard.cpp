#include "AMExportWizard.h"
#include "dataman/AMExportController.h"
#include <QHashIterator>
#include <QFormLayout>
#include <QComboBox>
#include "acquaman.h"
#include <QBoxLayout>

AMExportWizard::AMExportWizard(AMExportController* controller, QWidget *parent) :
	QWizard(parent)
{
	controller_ = controller;

	addPage(new AMExportWizardChooseExporterPage);

	setWindowTitle(QString("Exporting %1 Scans...").arg(controller_->scanCount()));
}

#include <QDebug>

AMExportWizardChooseExporterPage::AMExportWizardChooseExporterPage(QWidget *parent)
	: QWizardPage(parent)
{
	exporterComboBox_ = new QComboBox();

	QHashIterator<QString, AMExporterInfo> iRegisteredExporters(AMExportController::registeredExporters());

	while(iRegisteredExporters.hasNext()) {
		iRegisteredExporters.next();
		const AMExporterInfo& exporter = iRegisteredExporters.value();
		exporterComboBox_->addItem(exporter.description); /// \todo Icon
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, exporter.longDescription, AM::DescriptionRole);
		exporterComboBox_->setItemData(exporterComboBox_->count()-1, iRegisteredExporters.key(), AM::UserRole);
	}

	connect(exporterComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onExporterComboBoxIndexChanged(int)));
	registerField("exporterComboBoxIndex", exporterComboBox_);

	QFormLayout* fl = new QFormLayout();
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

	setTitle("Choose a File Format");
	setSubTitle("Acquaman can help you export your data in a variety of file formats.  On the next page, you'll be able to specify more options on how the data is formatted.");

	if(exporterComboBox_->count())
		onExporterComboBoxIndexChanged(0);

}



void AMExportWizardChooseExporterPage::onExporterComboBoxIndexChanged(int index)
{
	groupBox_->setTitle(exporterComboBox_->itemData(index, Qt::DisplayRole).toString());
	descriptionLabel_->setText(exporterComboBox_->itemData(index, AM::DescriptionRole).toString());
}
