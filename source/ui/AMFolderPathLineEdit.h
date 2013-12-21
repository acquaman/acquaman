/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMFOLDERPATHLINEEDIT_H
#define AMFOLDERPATHLINEEDIT_H

#include <QLineEdit>
#include <QValidator>


/// If you want to ensure that the content entered in a QLineEdit is a valid path to a folder, set an AMFolderPathValidator as its validator.  Beyond the QValidator functionality, this also gives you a validChanged() signal when the path becomes valid/not valid.
class AMFolderPathValidator : public QValidator {

	Q_OBJECT

public:
 	virtual ~AMFolderPathValidator();
	AMFolderPathValidator(QObject* parent = 0) : QValidator(parent) {}
	State validate ( QString & input, int & pos ) const;

signals:
	void validChanged(bool isValid) const;
};



/// This is a replacement for QLineEdit that validates and auto-completes folder paths, as well as providing user feedback when the path entered is valid.
class AMFolderPathLineEdit : public QLineEdit
{
	Q_OBJECT
public:
 	virtual ~AMFolderPathLineEdit();
	explicit AMFolderPathLineEdit(QWidget *parent = 0);

signals:

public slots:

protected slots:
	void onValidChanged(bool isValid);

};

#endif // AMFOLDERPATHLINEEDIT_H
