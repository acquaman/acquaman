#include "AMChooseDataFolderDialog.h"

#include <QLabel>
#include <QFileDialog>
#include <QStringList>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QStringBuilder>
#include <QComboBox>

// Static member
//////////////////////////////////////

bool AMChooseDataFolderDialog::getDataFolder(const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &dataDirectory, const QStringList &extraDataDirectory, QWidget *parent)
{
	AMUserSettings::load();

	AMChooseDataFolderDialog dialog(AMUserSettings::userDataFolder, localRootDirectory, remoteRootDirectory, dataDirectory, parent);
	dialog.exec();

	if (dialog.result() == QDialog::Accepted){

		// when we are using this dialog, we are not user-account based storage
		AMUserSettings::userBasedDataStorage = false;

		QString dialogInput = dialog.filePath();

		if (!dialog.isFullPath()){

			QFileInfo remoteFullPath(QString("%1/%2/%3").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput));
			QFileInfo localFullPath(QString("%1/%2/%3").arg(localRootDirectory).arg(dataDirectory).arg(dialogInput));

			bool isFirstTimeUser = !remoteFullPath.exists();
			bool successfulFirstTimeStartup = localFullPath.exists();
			if (isFirstTimeUser){

				QDir newPath(QString("%1/%2").arg(remoteRootDirectory).arg(dataDirectory));
				newPath.mkpath(QString("%1/userData").arg(dialogInput));

				foreach (QString newExtraDestination, extraDataDirectory)
					newPath.mkpath(QString("%1/%2").arg(dialogInput).arg(newExtraDestination));

				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData/").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::remoteDataFolder = "";
				AMUserSettings::save(true);
			}

			else if(successfulFirstTimeStartup){

				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData/").arg(localRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::remoteDataFolder = QString("%1/%2/%3/userData/").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::save();
			}
			else{
				//If this is not the first startup and first startup was not successful:
				//Reset .ini user and remote folders to first-startup settings.
				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData/").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::remoteDataFolder = "";
				AMUserSettings::save(true);
			}
		}

		else {

			QString userDataPath;
			if (dialogInput.endsWith("/userData") || dialogInput.endsWith("/userData/")) {
				userDataPath = dialogInput;
			} else {
				userDataPath = QString("%1/userData").arg(dialogInput);
			}

			QFileInfo remoteFullPath(userDataPath);
			if (!remoteFullPath.exists()) {
				QDir newPath(dialogInput);
				newPath.mkpath(userDataPath);
			}

			if (!userDataPath.endsWith("/"))
				userDataPath.append("/");

			AMUserSettings::userDataFolder = userDataPath;

			//Find the data folder name and see if it exists in the remoteDataFolder
			//Then if the user switches from a short name data folder to full path data
			//folder we avoid the case where the remote data folder .ini entry is not
			//updated.
			QDir testRemotePath(userDataPath);
			testRemotePath.cdUp();
			if(!AMUserSettings::remoteDataFolder.contains(testRemotePath.dirName())){
					AMUserSettings::removeRemoteDataFolderEntry();
			}
			AMUserSettings::save();
		}

		return true;
	}

	else
		return false;
}

//////////////////////////////////////

AMChooseDataFolderDialog::AMChooseDataFolderDialog(const QString &dataFolder, const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &dataDirectory, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Choose Data Ouput Destination");

	QString dataDestinationPath = dataDirectory.startsWith("/") ? dataDirectory : QString("/%1").arg(dataDirectory);
	QString localDataDirectory = QString("%1%2").arg(localRootDirectory).arg(dataDestinationPath);
	QString remoteDataDirectory = QString("%1%2").arg(remoteRootDirectory).arg(dataDestinationPath);
	folder_ = dataFolder;

	QLabel *explanation = new QLabel("Enter the proposal number or the absolute path to where you want your data to be saved.");
	pathComboBox_ = new QComboBox;
	pathComboBox_->setEditable(true);

	if (folder_.contains(dataDestinationPath)){

		QString tempFolder = folder_;
		tempFolder = tempFolder.left( tempFolder.indexOf(QRegExp(dataDestinationPath))+dataDestinationPath.length() );
		QDir dir(tempFolder);

		directories_ = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	}

	pathComboBox_->addItems(directories_);

	pathStatusLabel_ = new QLabel;
	QFont pathFont(this->font());
	pathFont.setItalic(true);
	pathStatusLabel_->setFont(pathFont);

	folderButton_ = new QToolButton;
	folderButton_->setIcon(QIcon(":/22x22/folder.png"));
	okButton_ = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");
	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	QString shortFormPath = "";

	if (folder_.contains(localDataDirectory) || folder_.contains(remoteDataDirectory)){

		shortFormPath = folder_;
		shortFormPath = shortFormPath.remove(QRegExp(QString("%1|%2").arg(localDataDirectory).arg(remoteDataDirectory)));
		shortFormPath = shortFormPath.remove("/userData");
		shortFormPath = shortFormPath.remove("/");
	}

	// If small form could not be resolved, then just use a full path.  Otherwise, set the folder text to the short form.
	if (shortFormPath.isEmpty())
		shortFormPath = folder_;

	else
		folder_ = shortFormPath;

	pathComboBox_->setEditText(shortFormPath);

	QHBoxLayout *lineEditLayout = new QHBoxLayout;
	lineEditLayout->addWidget(pathComboBox_);
	lineEditLayout->addWidget(pathStatusLabel_, 0, Qt::AlignCenter);
	lineEditLayout->addWidget(folderButton_);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton_);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(explanation);
	layout->addLayout(lineEditLayout);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	connect(folderButton_, SIGNAL(clicked()), this, SLOT(getFilePath()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pathComboBox_, SIGNAL(editTextChanged(QString)), this, SLOT(onTextChanged(QString)));

	onTextChanged(pathComboBox_->currentText());
}

bool AMChooseDataFolderDialog::isFullPath() const
{
	return folder_.contains("/");
}

void AMChooseDataFolderDialog::onTextChanged(const QString &text)
{
	QPalette palette(this->palette());
	QPalette pathPalette(this->palette());
	QString pathStatusString = pathStatus(text);

	okButton_->setDisabled(text.isEmpty() || pathStatusString == "Absolute path does not exist!");

	if (pathStatusString.contains("does not exist", Qt::CaseInsensitive)){

		palette.setColor(QPalette::Text, Qt::red);
		pathPalette.setColor(QPalette::WindowText, Qt::darkRed);
	}

	else if (pathStatusString.contains("Valid") || pathStatusString.contains("okay"))
		pathPalette.setColor(QPalette::WindowText, Qt::darkGreen);

	pathStatusLabel_->setText(pathStatusString);
	pathStatusLabel_->setPalette(pathPalette);
	pathComboBox_->setPalette(palette);
	folder_ = text;
}

QString AMChooseDataFolderDialog::pathStatus(const QString &path) const
{
	bool isPath = path.contains("/");
	bool pathExists = QFileInfo(path).exists();

	if (path.isEmpty())
		return "Does not exist!";

	else if (isPath && pathExists)
		return "Absolute path okay!";

	else if (isPath && !pathExists)
		return "Absolute path does not exist!";

	else if (isProposalNumber(path))
		return "Valid proposal number!";

	else if (directories_.contains(path))
		return "Valid existing experiment!";

	else
		return "New experiment!";
}

bool AMChooseDataFolderDialog::isProposalNumber(const QString &path) const
{
	return path.contains(QRegExp("^\\d{2,2}-\\d{4,4}$"));
}

void AMChooseDataFolderDialog::getFilePath()
{
	QString path = folder_;
	path.chop(1);
	path = path.remove("/userData");
	QString dir = QFileDialog::getExistingDirectory(this, "Choose a destination folder for your data.", path, QFileDialog::ShowDirsOnly);

	if (!dir.isEmpty())
		pathComboBox_->setEditText(dir);
}
