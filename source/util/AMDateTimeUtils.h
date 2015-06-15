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


#ifndef AMDATETIMEUTILS_H
#define AMDATETIMEUTILS_H
#include <QString>
#include <QDateTime>
#include <QStringList>

namespace AMDateTimeUtils
{
	/// Returns the date and time in a preferred, user-friendly format (ex: "Today at 3:30pm", "Yesterday at 12:15am", "Two days ago at 5:30pm", "March 18 (1985) at 12:01am" )
	QString prettyDateTime(const QDateTime &dateTime, const QString& timeFormat = "h:mmap");

	/// Returns the date in a relative, user-friendly format (ex: "Today", "Yesterday", "2 days ago", "March 18" (in current year), or "February 12 (1985)"
	QString prettyDate(const QDate& date);

	/// Returns the date in a relative, user-friendly format (ex: "Today", "Yesterday", "2 days ago", "March 18" (in current year), or "February 12 (1985)"
	QString prettyDate(const QDateTime& dt);

	/// Returns a compact date range (ex: "yesterday and today", "September 4 - 9 (2010)", "Aug 4 - Sept 9 (2010)", or "Aug 4 (2009) - Sept 9 (2010)"
	QString prettyDateRange(const QDateTime& start, const QDateTime& end);

	/// Formats an approximate duration, depending on the size. (ex: "6 seconds", "3 minutes and 5 seconds", "1 hour and 13 minutes", "1 day and 4.5 hours"). If you want to save space, ask for \c shortForm = true (ex: "6s", "3m 5s", "1h 13m", "1d 4.5h")
	QString prettyDuration(const QDateTime& start, const QDateTime& end, bool shortForm = false);

	/// Formats a QStringList of items into a QString that is a gramatically correct list (using commas and the word "and")
	QString gramaticallyCorrectList(const QStringList &listItems);

	/// Method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);
}

#endif // AMDATETIMEUTILS_H
