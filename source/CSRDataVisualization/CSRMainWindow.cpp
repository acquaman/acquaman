#include "CSRMainWindow.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QHBoxLayout>
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

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(directoryLayout);
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
}

void CSRMainWindow::onComputeClicked()
{
	if (!destinationDirectory_.exists("dataStore"))
		destinationDirectory_.mkdir("dataStore");

	if (!destinationDirectory_.exists("AnalyzedFiles"))
		destinationDirectory_.mkdir("AnalyzedFiles");


}

