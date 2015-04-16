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

#include <QDebug>
// Static member
//////////////////////////////////////

bool AMChooseDataFolderDialog::getDataFolder(const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &dataDirectory, const QStringList &extraDataDirectory, QWidget *parent)
{
	AMUserSettings::load();

	AMChooseDataFolderDialog dialog(AMUserSettings::userDataFolder, localRootDirectory, remoteRootDirectory, dataDirectory, parent);
	dialog.exec();

	if (dialog.result() == QDialog::Accepted){

		if (!dialog.isFullPath()){

			QString dialogInput = dialog.filePath();
			QFileInfo remoteFullPath(QString("%1/%2/%3").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput));
			bool isFirstTimeUser = !remoteFullPath.exists();

			if (!remoteFullPath.exists()){

				QDir newPath(QString("%1/%2").arg(remoteRootDirectory).arg(dataDirectory));
				newPath.mkpath(QString("%1/userData").arg(dialogInput));

				foreach (QString newExtraDestination, extraDataDirectory)
					newPath.mkpath(QString("%1/%2").arg(dialogInput).arg(newExtraDestination));
			}

			if (isFirstTimeUser){

				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData/").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::remoteDataFolder = "";
				AMUserSettings::save(true);
			}

			else {

				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData/").arg(localRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::remoteDataFolder = QString("%1/%2/%3/userData/").arg(remoteRootDirectory).arg(dataDirectory).arg(dialogInput);
				AMUserSettings::save();
			}
		}

		else {

			AMUserSettings::removeRemoteDataFolderEntry();
			AMUserSettings::userDataFolder = dialog.filePath();
			AMUserSettings::save();
		}

		return true;
	}

	else
		return false;
}

//////////////////////////////////////

AMChooseDataFolderDialog::AMChooseDataFolderDialog(const QString &dataFolder, const QString &localRootDirectory, const QString &remoteRootDirectory, const QString dataDirectory, QWidget *parent)
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

	qDebug() << "==== " << folder_;
	qDebug() << "==== " << directories_;
	pathComboBox_->addItems(directories_);

	pathStatusLabel_ = new QLabel;
	QFont pathFont(this->font());
	pathFont.setItalic(true);
	pathStatusLabel_->setFont(pathFont);

	advancedCheckBox_ = new QCheckBox("Advanced");
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

	advancedCheckBox_->setChecked(shortFormPath.isEmpty());
	folderButton_->setEnabled(shortFormPath.isEmpty());

	// If small form could not be resolved, then just use a full path.  Otherwise, set the folder text to the short form.
	if (shortFormPath.isEmpty())
		shortFormPath = folder_;

	else
		folder_ = shortFormPath;

	pathComboBox_->setEditText(shortFormPath);

	QHBoxLayout *lineEditLayout = new QHBoxLayout;
	lineEditLayout->addWidget(pathComboBox_);
	lineEditLayout->addWidget(pathStatusLabel_);
	lineEditLayout->addWidget(folderButton_);
	lineEditLayout->addWidget(advancedCheckBox_);

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
	connect(advancedCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onAdvancedCheckboxToggled(bool)));

	onTextChanged(pathComboBox_->currentText());
}

void AMChooseDataFolderDialog::onTextChanged(const QString &text)
{
	advancedCheckBox_->setChecked(text.length() > 0 && text.startsWith("/"));

	QPalette palette(this->palette());
	QPalette pathPalette(this->palette());
	QString pathStatusString = pathStatus(text);

	okButton_->setDisabled(pathStatusString == "Absolute path does not exist!");

	if (pathStatusString == "Absolute path does not exist!"){

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

void AMChooseDataFolderDialog::onAdvancedCheckboxToggled(bool value)
{
	folderButton_->setEnabled(value);
	onTextChanged(pathComboBox_->currentText());
}

QString AMChooseDataFolderDialog::pathStatus(const QString &path) const
{
	if (advancedCheckBox_->isChecked())
		return QFileInfo(path).exists() ? "Absolute path okay!" : "Absolute path does not exist!";

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
