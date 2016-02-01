#include "CSRMainWindow.h"

#include <QGroupBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTime>

#include <math.h>

#include <QDebug>
CSRMainWindow::CSRMainWindow(CSRDataModel *model, QWidget *parent)
	: QWidget(parent)
{
	model_ = model;
	computationThread_ = 0;

	getDirectoryButton_ = new QPushButton("Choose Directory");
	connect(getDirectoryButton_, SIGNAL(clicked()), this, SLOT(onGetDirectoryButtonClicked()));

	directoryLabel_ = new QLabel;

	QHBoxLayout *directoryLayout = new QHBoxLayout;
	directoryLayout->addWidget(getDirectoryButton_);
	directoryLayout->addWidget(directoryLabel_);

	computeButton_ = new QPushButton("Compute");
	computeButton_->setDisabled(true);
	connect(computeButton_, SIGNAL(clicked()), this, SLOT(onComputeClicked()));

	configurationLayout_ = new QVBoxLayout;

	addButton_ = new QPushButton("Add File Type");
	connect(addButton_, SIGNAL(clicked()), this, SLOT(onFileConfigurationAddRequested()));
	configurationLayout_->addWidget(addButton_, 0, Qt::AlignRight);

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++){

		CSRFileConfigurationView *view = new CSRFileConfigurationView(model_->fileConfigurationAt(i));
		fileConfigurationViews_ << view;
		configurationLayout_->addWidget(view);
		connect(view, SIGNAL(deleteRequested(CSRFileConfigurationView*)), this, SLOT(onFileConfigurationDeleteRequested(CSRFileConfigurationView*)));
	}

	fileInformationLabel_ = new QLabel("No directory chosen.");

	progressLabel_ = new QLabel("");
	progressBar_ = new QProgressBar;
	progressBar_->hide();

	QHBoxLayout *progressLayout = new QHBoxLayout;
	progressLayout->addWidget(progressLabel_);
	progressLayout->addWidget(progressBar_);
	progressLayout->addWidget(computeButton_, 0, Qt::AlignRight);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(directoryLayout);
	mainLayout->addLayout(configurationLayout_);
	mainLayout->addWidget(fileInformationLabel_, 0, Qt::AlignLeft);
	mainLayout->addLayout(progressLayout);

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
		QStringList tempFileNames = destinationDirectory_.entryList(QStringList() << "*.txt");

		foreach (QString tempFileName, tempFileNames)
			fileNames_ << QString("%1/%2").arg(directory).arg(tempFileName);
	}

	computeButton_->setDisabled(directory.isEmpty());

	QVector<int> fileInformationCounters = QVector<int>(model_->fileConfigurationCount()+1, 0);

	foreach (QString filename, fileNames_){

		bool fileAccountedFor = false;
		QFileInfo info(filename);

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
	QTime shortEstimatedTime = QTime::currentTime();
	QTime longEstimatedTime = shortEstimatedTime;

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++){

		fileOutputString.append(QString("%1 file%3 of %2 MB (~%4 s or ~%5 s per file)\n").arg(fileInformationCounters.at(i)).arg(int(model_->fileConfigurationAt(i)->fileSize()/pow(2, 20))).arg(fileInformationCounters.at(i) > 1 ? "s" : "").arg(1.7*model_->fileConfigurationAt(i)->fileSize()/(700*pow(2, 20)), 0, 'g', 2).arg(52.0*model_->fileConfigurationAt(i)->fileSize()/(700*pow(2, 20)), 0, 'g', 3));
		shortEstimatedTime = shortEstimatedTime.addSecs(int(1.7*model_->fileConfigurationAt(i)->fileSize()/(700*pow(2, 20)))*fileInformationCounters.at(i));
		longEstimatedTime = longEstimatedTime.addSecs(int(52.0*model_->fileConfigurationAt(i)->fileSize()/(700*pow(2, 20)))*fileInformationCounters.at(i));
	}

	if (fileInformationCounters.last() != 0)
		fileOutputString.append(QString("%1 file%2 that %3 unnaccounted.\n").arg(fileInformationCounters.last()).arg(fileInformationCounters.last() > 1 ? "s" : "").arg(fileInformationCounters.last() > 1 ? "were" : "was"));

	fileOutputString.append(QString("Estimated time to finish between %1 and %2.\n").arg(shortEstimatedTime.toString("hh:mm")).arg(longEstimatedTime.toString("hh:mm")));
	fileInformationLabel_->setText(fileOutputString);
}

void CSRMainWindow::onComputeClicked()
{
	computeButton_->setDisabled(true);
	getDirectoryButton_->setDisabled(true);
	addButton_->setDisabled(true);

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++)
		fileConfigurationViews_.at(i)->setDisabled(true);

	if (!destinationDirectory_.exists("dataStore"))
		destinationDirectory_.mkdir("dataStore");

	if (!destinationDirectory_.exists("AnalyzedFiles"))
		destinationDirectory_.mkdir("AnalyzedFiles");

	model_->setFileNames(fileNames_);
	progressBar_->setRange(0, fileNames_.size());
	progressBar_->setValue(0);
	progressBar_->show();
	progressLabel_->setText(QString("Finished %1 of %2 files.").arg(0).arg(fileNames_.size()));

	CSRDataModel *threadModel = new CSRDataModel(*model_);
	connect(threadModel, SIGNAL(progress(int)), this, SLOT(onComputationModelUpdate(int)));
	connect(this, SIGNAL(startComputation()), threadModel, SLOT(start()));
	connect(threadModel, SIGNAL(finished()), threadModel, SLOT(deleteLater()));

	computationThread_ = new QThread;
	threadModel->moveToThread(computationThread_);
	computationThread_->start();

	connect(threadModel, SIGNAL(finished()), this, SLOT(onCompuatationThreadFinished()));

	emit startComputation();
}

void CSRMainWindow::onFileConfigurationDeleteRequested(CSRFileConfigurationView *view)
{
	configurationLayout_->removeWidget(view);
	fileConfigurationViews_.removeOne(view);
	model_->removeFileConfiguration(view->fileConfiguration());
	view->deleteLater();
}

void CSRMainWindow::onFileConfigurationAddRequested()
{
	CSRFileConfiguration *info = new CSRFileConfiguration(0, 0, 0);
	CSRFileConfigurationView *view = new CSRFileConfigurationView(info);
	model_->addFileConfiguration(info);
	fileConfigurationViews_ << view;
	configurationLayout_->addWidget(view);
	connect(view, SIGNAL(deleteRequested(CSRFileConfigurationView*)), this, SLOT(onFileConfigurationDeleteRequested(CSRFileConfigurationView*)));
}

void CSRMainWindow::onCompuatationThreadFinished()
{
	computeButton_->setEnabled(true);
	getDirectoryButton_->setEnabled(true);
	addButton_->setEnabled(true);

	for (int i = 0, size = model_->fileConfigurationCount(); i < size; i++)
		fileConfigurationViews_.at(i)->setEnabled(true);

	progressBar_->hide();
	progressLabel_->setText("");

	connect(computationThread_, SIGNAL(finished()), computationThread_, SLOT(deleteLater()));
	computationThread_->quit();
}

void CSRMainWindow::onComputationModelUpdate(int fileFinished)
{
	progressBar_->setValue(fileFinished);
	progressLabel_->setText(QString("Finished %1 of %2 files.").arg(fileFinished).arg(fileNames_.size()));
}
