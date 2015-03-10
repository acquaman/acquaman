#include "CSRMainWindow.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <math.h>

#include <QDebug>
CSRMainWindow::CSRMainWindow(CSRDataModel *model, QWidget *parent)
	: QWidget(parent)
{
	model_ = model;

	QPushButton *getDirectoryButton = new QPushButton("Choose Directory");
	connect(getDirectoryButton, SIGNAL(clicked()), this, SLOT(onGetDirectoryButtonClicked()));

	directoryLabel_ = new QLabel;

	QHBoxLayout *directoryLayout = new QHBoxLayout;
	directoryLayout->addWidget(getDirectoryButton);
	directoryLayout->addWidget(directoryLabel_);

	computeButton_ = new QPushButton("Compute");
	computeButton_->setDisabled(true);
	connect(computeButton_, SIGNAL(clicked()), this, SLOT(onComputeClicked()));

	QVBoxLayout *configurationLayout = new QVBoxLayout;

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++){

		CSRFileConfigurationView *view = new CSRFileConfigurationView(model_->fileConfigurationAt(i));
		configurationLayout->addWidget(view);
	}

	fileInformationLabel_ = new QLabel("No directory chosen.");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(directoryLayout);
	mainLayout->addLayout(configurationLayout);
	mainLayout->addWidget(fileInformationLabel_, 0, Qt::AlignLeft);
	mainLayout->addWidget(computeButton_, 0, Qt::AlignRight);

	setLayout(mainLayout);
}

CSRMainWindow::~CSRMainWindow()
{
}

void CSRMainWindow::onGetDirectoryButtonClicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, "Choose data files directory...");
	directoryLabel_->setText(directory);

	if (!directory.isEmpty()){

		destinationDirectory_ = QDir(directory);
		fileNames_ = destinationDirectory_.entryList(QStringList() << "*.txt");
	}

	computeButton_->setDisabled(directory.isEmpty());

	QVector<int> fileInformationCounters = QVector<int>(model_->fileConfigurationCount()+1, 0);

	foreach (QString filename, fileNames_){

		bool fileAccountedFor = false;
		QFileInfo info(QString("%1/%2").arg(destinationDirectory_.path()).arg(filename));

		for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++){

			if (abs(info.size()-model_->fileConfigurationAt(i)->fileSize()) < int(model_->fileConfigurationAt(i)->fileSize()*0.05)){

				fileAccountedFor = true;
				fileInformationCounters[i]++;
			}
		}

		if (!fileAccountedFor)
			fileInformationCounters[fileInformationCounters.size()-1]++;
	}

	QString fileOutputString = "Directory Statistics\n";

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++)
		fileOutputString.append(QString("%1 file%3 of %2 MB\n").arg(fileInformationCounters.at(i)).arg(int(model_->fileConfigurationAt(i)->fileSize()/pow(2, 20))).arg(fileInformationCounters.at(i) > 1 ? "s" : ""));

	if (fileInformationCounters.last() != 0)
		fileOutputString.append(QString("%1 file%2 that %3 unnaccounted.\n").arg(fileInformationCounters.last()).arg(fileInformationCounters.last() > 1 ? "s" : "").arg(fileInformationCounters.last() > 1 ? "were" : "was"));

	fileInformationLabel_->setText(fileOutputString);
}

void CSRMainWindow::onComputeClicked()
{
	if (!destinationDirectory_.exists("dataStore"))
		destinationDirectory_.mkdir("dataStore");

	if (!destinationDirectory_.exists("AnalyzedFiles"))
		destinationDirectory_.mkdir("AnalyzedFiles");


}

