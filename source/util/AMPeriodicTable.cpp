/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMPeriodicTable.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

// Singleton instance
AMPeriodicTable* AMPeriodicTable::instance_ = 0;

AMPeriodicTable::AMPeriodicTable(QObject *parent) :
	QObject(parent)
{
	QFile file(":/ElementalInformation.txt");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		emit unableToLoad(file.fileName(), file.errorString());
		return;
	}

	QTextStream in(&file);

	AMElement *temp = 0;
	QStringList current;

	while(!in.atEnd()){

		current << in.readLine().split(",");
		switch(current.size()){
		case 3:
			temp = new AMElement(current.first(), current.at(1), current.at(2), QStringList(), QStringList());
			break;
		case 4:
			temp = new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), QStringList());
			break;
		case 5:
			temp = new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), ((QString)(current.at(4))).split(" "));
			break;
		}
		periodicTable_ << temp;
		current.clear();
	}

	file.close();
}
