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


#include "AMPeriodicTable.h"

#include <QFile>
#include <QTextStream>

#include "util/AMErrorMonitor.h"

// Singleton instance
AMPeriodicTable* AMPeriodicTable::instance_ = 0;

 AMPeriodicTable::~AMPeriodicTable(){}
AMPeriodicTable::AMPeriodicTable(QObject *parent) :
	QObject(parent)
{
	QFile file(":/ElementalInformation.txt");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		AMErrorMon::error(this, AMPERIODICTABLE_COULD_NOT_LOAD_ELEMENTAL_INFORMATION, "There was an error loading the elemental information for the periodic table.  Elemental information throughout the program may no longer be defined.");
		return;
	}

	QTextStream in(&file);

	while(!in.atEnd()){

		QStringList current = in.readLine().split(",");

		switch(current.size()){

		case 3:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), QStringList(), QStringList());
			break;

		case 4:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), QStringList());
			break;

		case 5:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), ((QString)(current.at(4))).split(" "));
			break;
		}
	}

	file.close();
}

AMElement *AMPeriodicTable::elementByName(QString name) const
{
	for (int i = 0; i < periodicTable_.size(); i++)
		if (periodicTable_.at(i)->name() == name)
			return periodicTable_.at(i);

	return 0;
}

AMElement *AMPeriodicTable::elementBySymbol(QString symbol) const
{
	for (int i = 0; i < periodicTable_.size(); i++)
		if (periodicTable_.at(i)->symbol() == symbol)
			return periodicTable_.at(i);

	return 0;
}
