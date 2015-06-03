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


// Returns a compact date range (ex: "yesterday and today", "September 4 - 9 (2010)", "Aug 4 - Sept 9 (2010)", or "Aug 4 (2009) - Sept 9 (2010)"
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


QString AMDateTimeUtils::prettyDuration(const QDateTime& start, const QDateTime& end, bool shortForm) {
	if(!start.isValid() || !end.isValid())
		return "??";

	int seconds = start.secsTo(end);
	if(seconds == 0) {
		return shortForm ? QString("%1 ms").arg(start.msecsTo(end)) : QString("%1 milliseconds").arg(start.msecsTo(end));
	}
	else if(seconds == 1)
		return shortForm ? "1 s" : "1 second";
	else if(seconds < 60)
		return shortForm ? QString("%1 s").arg(seconds) : QString("%1 seconds").arg(seconds);
	else if(seconds < 3600) {
		int minutes = int(seconds / 60.0);
		int remainingSeconds = seconds - minutes*60;
		if(minutes == 1)
			return shortForm ? QString("1m %1s").arg(remainingSeconds) : QString("1 minute and %1 sec").arg(remainingSeconds);
		else
			return shortForm ? QString("%1m %2s").arg(minutes).arg(remainingSeconds) : QString("%1 minutes and %2 sec").arg(minutes).arg(remainingSeconds);
	}
	else if(seconds < 3600*24) {
		int hours = int(seconds / 3600.0);
		int remainingMinutes = int((seconds - hours*3600.0)/60.0);
		if(hours == 1)
			return shortForm ? QString("1h %1m").arg(remainingMinutes) : QString("1 hour and %1 min").arg(remainingMinutes);
		else
			return shortForm ? QString("%1h %2m").arg(hours).arg(remainingMinutes) : QString("%1 hours and %2 min").arg(hours).arg(remainingMinutes);
	}
	else {
		int days = int(seconds / (3600.0*24.0));
		double remainingHours = (seconds - days*3600*24)/3600.0;
		if(days == 1)
			return shortForm ? QString("1d %1h").arg(remainingHours, 0, 'f', 1) : QString("1 day, %1 hours").arg(remainingHours, 0, 'f', 1);
		else
			return shortForm ? QString("1d %1h").arg(remainingHours, 0, 'f', 1) : QString("%1 days, %2 hours").arg(days).arg(remainingHours, 0, 'f', 1);
	}
}

QString AMDateTimeUtils::gramaticallyCorrectList(const QStringList &listItems){
	QString retVal = "";
	if(listItems.count() == 0){
		//do nothing
	}
	else if(listItems.count() == 1)
		retVal = listItems.first();
	else if(listItems.count() == 2)
		retVal = QString("%1 and %2").arg(listItems.first()).arg(listItems.last());
	else{
		for(int x = 0; x < listItems.count()-1; x++)
			retVal.append(QString("%1, ").arg(listItems.at(x)));
		retVal.append(QString("and %1").arg(listItems.last()));
	}
	return retVal;
}

/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
QString AMDateTimeUtils::convertTimeToString(double time)
{
	QString timeString;

	int days = int(time/3600.0/24.0);

	if (days > 0){

		time -= days*3600.0*24;
		timeString += QString::number(days) + "d:";
	}

	int hours = int(time/3600.0);

	if (hours > 0){

		time -= hours*3600;
		timeString += QString::number(hours) + "h:";
	}

	int minutes = int(time/60.0);

	if (minutes > 0){

		time -= minutes*60;
		timeString += QString::number(minutes) + "m:";
	}

	int seconds = ((int)time)%60;
	timeString += QString::number(seconds) + "s";

	return timeString;
}

