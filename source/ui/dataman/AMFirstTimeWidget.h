/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMFIRSTTIMEWIDGET_H
#define AMFIRSTTIMEWIDGET_H

#include <QObject>
#include <QWizard>
#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QDir>
#include <QFileInfo>
#include <QPushButton>
#include <QCheckBox>

#include <QFileDialog>
#include <QMessageBox>

#include <util/AMSettings.h>
#include <ui/AMFolderPathLineEdit.h>

#include <QDebug>


// Needed for getenv():
#include <cstdlib>


class AMFirstTimeWizardPage : public QWizardPage
{
	Q_OBJECT

public:
	AMFirstTimeWizardPage(bool userBasedDataStorage=true, QWidget *parent = 0) : QWizardPage(parent) {
		setTitle("Welcome to Acquman");
		setSubTitle("This looks like the first time you're using Acquaman. We need some information to get you started.");


		QFormLayout *layout = new QFormLayout();
		userName_ = new QLineEdit();

		userDataFolder_ = new AMFolderPathLineEdit();
		browseButton_ = new QPushButton("Choose...");

		useLocalStorage_ = new QCheckBox("Store Data Locally for Faster Performance");

		QHBoxLayout* hl = new QHBoxLayout();
		if (userBasedDataStorage) {
			hl->addWidget(userDataFolder_);
			hl->addWidget(browseButton_);

		} else {
			// for none user-account based storage, the user data path is already chosen by the AmChooseDataFolderDialog.
			// for simplification, we won't allow the user to change the path again here.
			userDataFolder_->setEnabled(false);
			useLocalStorage_->setEnabled(false);
			hl->addWidget(userDataFolder_);
		}

		layout->addRow("Your Name:", userName_);
		layout->addRow("Where to store your data:", hl);
		setLayout(layout);

		registerField("userName*", userName_);
		registerField("userDataFolder*", userDataFolder_);

		connect(browseButton_, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));

		QDir rootDir("/");
		QFileInfoList allRootEntries = rootDir.entryInfoList();
		for(int x = 0, size = allRootEntries.count(); x < size; x++){
			if(allRootEntries.at(x).fileName() == "AcquamanLocalData" && allRootEntries.at(x).isDir()){
				QFile::Permissions filePermissions = allRootEntries.at(x).permissions();
				if(filePermissions.testFlag(QFile::ReadOwner) && filePermissions.testFlag(QFile::WriteOwner)
						&& filePermissions.testFlag(QFile::ReadGroup) && filePermissions.testFlag(QFile::WriteGroup)
						&& filePermissions.testFlag(QFile::ReadOther) && filePermissions.testFlag(QFile::WriteOther)){
					layout->addRow("", useLocalStorage_);
					registerField("useLocalStorage", useLocalStorage_);
				}
			}
		}
	}

	virtual ~AMFirstTimeWizardPage(){}

protected slots:
	void onBrowseButtonClicked() {
		userDataFolder_->setText(QFileDialog::getExistingDirectory(this, "Acquaman Data Folder", userDataFolder_->text()));
	}

protected:
	virtual bool validatePage(){
		QString userDatabaseFolder = userDataFolder_->text();
		if(userDatabaseFolder.endsWith('/'))
			userDatabaseFolder.remove(userDatabaseFolder.count()-1, 1);
		if(userDatabaseFolder == QDir::homePath()){
			QMessageBox badDatbaseFolderChoice;
			badDatbaseFolderChoice.setText(QString("This may be a bad place to save your data"));
			badDatbaseFolderChoice.setInformativeText(QString("Your home folder is not the best place to save your data.\nYou can select Ok to proceed, but you really should make a folder to put your data in."));
			badDatbaseFolderChoice.setIcon(QMessageBox::Question);
			badDatbaseFolderChoice.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			badDatbaseFolderChoice.setDefaultButton(QMessageBox::Ok);
			badDatbaseFolderChoice.setEscapeButton(QMessageBox::Cancel);

			if(badDatbaseFolderChoice.exec() == QMessageBox::Ok)
				return true;
			else
				return false;
		}
		return true;
	}

private:
	QLineEdit *userName_;
	QLineEdit *userDataFolder_;
	QPushButton* browseButton_;
	QCheckBox *useLocalStorage_;
};


/// This class provides a wizard used to gather initial user information
class AMFirstTimeWizard : public QWizard
{
	Q_OBJECT

public:
	AMFirstTimeWizard(bool userBasedDataStorage=true, QWidget *parent = 0) : QWizard(parent) {
		addPage(new AMFirstTimeWizardPage(userBasedDataStorage));
		setWindowTitle("Welcome to Acquaman");

	}


protected:
	void initializePage ( int id ) {
		QWizard::initializePage(id);

		if(id == 0) {
			setField("userName", getenv("USER"));
			setField("userDataFolder", AMUserSettings::userDataFolder);
		}
	}

};

#endif // AMFIRSTTIMEWIDGET_H
