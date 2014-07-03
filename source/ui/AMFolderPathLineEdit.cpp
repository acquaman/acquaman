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


#include "AMFolderPathLineEdit.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QDir>

 AMFolderPathLineEdit::~AMFolderPathLineEdit(){}
AMFolderPathLineEdit::AMFolderPathLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	QCompleter* folderCompleter = new QCompleter(this);
	QFileSystemModel* folderModel = new QFileSystemModel(this);
	folderModel->setRootPath(QDir::rootPath());
	folderModel->setFilter(QDir::AllDirs);
	folderCompleter->setModel(folderModel);
	setCompleter(folderCompleter);

	AMFolderPathValidator* v = new AMFolderPathValidator(this);
	setValidator(v);

	connect(v, SIGNAL(validChanged(bool)), this, SLOT(onValidChanged(bool)));
}

void AMFolderPathLineEdit::onValidChanged(bool isValid)
{
	QPalette p = palette();
	if(isValid)
		p.setColor(QPalette::Text, Qt::black);
	else
		p.setColor(QPalette::Text, Qt::red);

	setPalette(p);
}

QValidator::State AMFolderPathValidator::validate(QString &input, int &pos) const
{
	Q_UNUSED(pos)

	QDir dir(input);
	if(dir.exists()) {
		emit validChanged(true);
		return QValidator::Acceptable;
	}

	emit validChanged(false);
	return QValidator::Intermediate;
}
 AMFolderPathValidator::~AMFolderPathValidator(){}
