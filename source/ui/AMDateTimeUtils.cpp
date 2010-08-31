#include "AMDateTimeUtils.h"
#include <QDateTime>



QString AMDateTimeUtils::prettyDate(const QDate &date){

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
	return prettyDate(dateTime.date()) + " at " + dateTime.time().toString(timeFormat);
}

QString AMDateTimeUtils::prettyDate(const QDateTime& dt) { return prettyDate(dt.date()); }
