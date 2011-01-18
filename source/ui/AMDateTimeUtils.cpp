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


#include "AMDateTimeUtils.h"
#include <QDateTime>



QString AMDateTimeUtils::prettyDate(const QDate &date){

	if(!date.isValid())
		return "??";

	QDate current = QDate::currentDate();
	int daysTo = date.daysTo(current);

	if (daysTo==0)
		return "today";
	else if (daysTo == 1)
		return "yesterday";
	else if (daysTo == 2)
		return "two days ago";
	else if (date.year() == current.year())
		return date.toString("MMM d");
	else return date.toString("MMM d (yyyy)");

}

QString AMDateTimeUtils::prettyDateTime(const QDateTime &dateTime, const QString &timeFormat) {
	if(!dateTime.isValid())
		return "??";
	return prettyDate(dateTime.date()) + " at " + dateTime.time().toString(timeFormat);
}

QString AMDateTimeUtils::prettyDate(const QDateTime& dt) { return prettyDate(dt.date()); }


/// Returns a compact date range (ex: "yesterday and today", "September 4 - 9 (2010)", "Aug 4 - Sept 9 (2010)", or "Aug 4 (2009) - Sept 9 (2010)"
QString AMDateTimeUtils::prettyDateRange(const QDateTime& start, const QDateTime& end) {

	if(!start.isValid() || !end.isValid())
		return prettyDate(start) + " - " + prettyDate(end);


	QDate current = QDate::currentDate();

	int startDaysTo = start.date().daysTo(current);
	int endDaysTo = end.date().daysTo(current);

	// ending today:
	if(endDaysTo == 0) {
		if(startDaysTo == 0)
			return "today";
		if(startDaysTo == 1)
			return "yesterday and today";
		return prettyDate(start) + " - " + "today";
	}

	// years equal
	if(start.date().year() == end.date().year()) {

		// months equal
		if(start.date().month() == end.date().month()) {
			if(start.date().year() == current.year())
				return start.toString("MMM d") + " - " + end.toString("d");
			return start.toString("MMM d") + " - " + end.toString("d (yyyy)");
		}

		// months not equal
		if(start.date().year() == current.year())
			return start.toString("MMM d") + " - " + end.toString("MMM d");
		return start.toString("MMM d") + " - " + end.toString("MMM d (yyyy)");
	}

	// years not equal
	return start.toString("MMM d (yyyy)") + " - " + end.toString("MMM d (yyyy)");
}
