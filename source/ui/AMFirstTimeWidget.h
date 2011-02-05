/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include <QPushButton>
#include <QValidator>
#include <QFileDialog>

#include <util/AMSettings.h>

class AMFolderPathValidator : public QValidator {

	Q_OBJECT

public:
	AMFolderPathValidator(QObject* parent = 0) : QValidator(parent) {}

	State validate ( QString & input, int & pos ) const {
		Q_UNUSED(pos)

		QDir dir(input);
		if(dir.exists()) {
			emit validChanged(true);
			return QValidator::Acceptable;
		}

		emit validChanged(false);
		return QValidator::Intermediate;
	}

signals:
	void validChanged(bool isValid) const;

};

#include <QCompleter>
#include <QDirModel>

// Needed for getenv():
#include <cstdlib>


class AMFirstTimeWizardPage : public QWizardPage
 {
	 Q_OBJECT

 public:
	AMFirstTimeWizardPage(QWidget *parent = 0) : QWizardPage(parent) {
		setTitle("Welcome to Acquman");
		setSubTitle("This looks like the first time you're using Acquaman. We need some information to get you started.");


		QFormLayout *layout = new QFormLayout();
		userName = new QLineEdit();
		userDataFolder = new QLineEdit();
		folderCompleter = new QCompleter(this);
		folderCompleter->setModel(new QDirModel(folderCompleter));
		userDataFolder->setCompleter(folderCompleter);

		AMFolderPathValidator* v = new AMFolderPathValidator(userDataFolder);
		userDataFolder->setValidator(v);
		browseButton = new QPushButton("Choose...");

		QHBoxLayout* hl = new QHBoxLayout();
		hl->addWidget(userDataFolder);
		hl->addWidget(browseButton);

		layout->addRow("Your Name:", userName);
		layout->addRow("Where to store your data:", hl);
		setLayout(layout);

		registerField("userName*", userName);
		registerField("userDataFolder*", userDataFolder);

		connect(browseButton, SIGNAL(clicked()), this, SLOT(onBrowseButtonClicked()));
		connect(v, SIGNAL(validChanged(bool)), this, SLOT(onFolderPathValid(bool)));

	}

protected slots:
	void onBrowseButtonClicked() {
		userDataFolder->setText(QFileDialog::getExistingDirectory(this, "Acquaman Data Folder", userDataFolder->text()));
	}

	void onFolderPathValid(bool isValid) {
		if(isValid)
			userDataFolder->setStyleSheet("color: black;");
		else
			userDataFolder->setStyleSheet("color: red;");
	}

 private:
	 QLineEdit *userName, *userDataFolder;
	 QCompleter* folderCompleter;
	 QPushButton* browseButton;


 };


/// This class provides a wizard used to gather initial user information
class AMFirstTimeWizard : public QWizard
 {
	 Q_OBJECT

 public:
	AMFirstTimeWizard(QWidget *parent = 0) : QWizard(parent) {
		addPage(new AMFirstTimeWizardPage);
		setWindowTitle("Welcome to Acquaman");

	}

	 // void accept();


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
