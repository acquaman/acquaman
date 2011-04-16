#include "AMExportWizard.h"
#include "dataman/AMExportController.h"
#include "dataman/AMExporter.h"
#include "dataman/AMExporterOption.h"

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
	addPage(new AMExportWizardOptionPage);

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


AMExportWizardOptionPage::AMExportWizardOptionPage(QWidget *parent)
{

	option_ = 0;
	optionView_ = 0;

	optionSelector_ = new QComboBox();
	optionViewContainer_ = new QWidget();
	optionViewContainer_->setLayout(new QVBoxLayout());
	optionViewContainer_->layout()->setContentsMargins(0,0,0,0);
	saveOptionButton_ = new QPushButton("Save Settings");

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(optionSelector_);
	hl->addStretch();
	hl->addWidget(saveOptionButton_);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addLayout(hl);
	vl->addWidget(optionViewContainer_);

	setLayout(vl);

	connect(optionSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionSelectorIndexChanged(int)));

	setTitle("Choose Export Options");
	setSubTitle(("The options available here depend on the file format you've selected.  You can save a set of options as a template for next time using the 'Save Settings' button."));
}

bool AMExportWizardOptionPage::validatePage()
{
	return true;
}

#include "dataman/AMDbObjectSupport.h"


void AMExportWizardOptionPage::initializePage()
{
	exporter_ = qobject_cast<AMExportWizard*>(wizard())->controller()->exporter();

	delete option_;	// might be 0 if no option yet... but that's okay.
	delete optionView_;	// might be 0 if no option yet... or if last option couldn't create an editor. but that's okay.

	saveOptionButton_->setDisabled(true);
	populateOptionSelector();

	// if we don't have any existing options... make a default one.
	if(optionSelector_->count() == 0) {
		option_ = exporter_->createDefaultOption();
	}
	else {
		option_ = qobject_cast<AMExporterOption*>(
					AMDbObjectSupport::createAndLoadObjectAt(
						AMDatabase::userdb(),
						AMDbObjectSupport::tableNameForClass(exporter_->exporterOptionClassName()),
						optionSelector_->itemData(optionSelector_->currentIndex()).toInt()));
	}

	qDebug() << "Option: " << option_;

	optionView_ = option_->createEditorWidget();
	if(optionView_)
		optionViewContainer_->layout()->addWidget(optionView_);

	connect(option_, SIGNAL(modifiedChanged(bool)), saveOptionButton_, SLOT(setEnabled(bool)));
}

void AMExportWizardOptionPage::onOptionSelectorIndexChanged(int index)
{
	delete option_;
	delete optionView_;

	option_ = qobject_cast<AMExporterOption*>(
				AMDbObjectSupport::createAndLoadObjectAt(
					AMDatabase::userdb(),
					AMDbObjectSupport::tableNameForClass(exporter_->exporterOptionClassName()),
					optionSelector_->itemData(optionSelector_->currentIndex()).toInt()));

	optionView_ = option_->createEditorWidget();
	if(optionView_)
		optionViewContainer_->layout()->addWidget(optionView_);
}

void AMExportWizardOptionPage::populateOptionSelector()
{
	optionSelector_->blockSignals(true);
	optionSelector_->clear();
	// fill option combo box
	QSqlQuery q = AMDbObjectSupport::select(AMDatabase::userdb(), exporter_->exporterOptionClassName(), "id, name");
	while(q.next()) {
		optionSelector_->addItem(q.value(1).toString(),
								 q.value(0).toInt());	// note: putting the database id in Qt::UserRole.
	}
	optionSelector_->setCurrentIndex(optionSelector_->count()-1);
	optionSelector_->blockSignals(false);
}
