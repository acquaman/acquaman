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


#include "AMDateTimeModifier.h"
#include <QDateTime>

AMDateTimeModifier::AMDateTimeModifier()
{

}

//Is dateTime always stored in description role?
//How/where do I get the original dateTime?
//
QString AMDateTimeModifier::checkAndModifyDate(const QDateTime &dateTime){
	QDateTime &currentDT = currentDateTime();
	if (dateTime.daysTo(currentDT)==0)
		//change the date to "today"
		return QString("today at");
	else if (dateTime.daysTo(currentDT) == 1)
		//change the date to "yesterday"
		return QString("yesterday at");
		else if (dateTime.daysTo(currentDT)== 2)
			//change the date to "two days ago"
			return QString("two days ago at");
			else if (dateTime.daysTo(currentDt) == 3)
				//change the date to three days ago
				return QString("three days ago at")

}
