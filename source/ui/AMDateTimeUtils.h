#ifndef AMDATETIMEUTILS_H
#define AMDATETIMEUTILS_H
#include <QString>
#include <QDateTime>

namespace AMDateTimeUtils
{
	/// Returns the date and time in a preferred, user-friendly format (ex: "Today at 3:30pm", "Yesterday at 12:15am", "Two days ago at 5:30pm", "March 18 (1985) at 12:01am" )
	QString prettyDateTime(const QDateTime &dateTime, const QString& timeFormat = "h:mmap");

	/// Returns the date in a relative, user-friendly format (ex: "Today", "Yesterday", "2 days ago", "March 18" (in current year), or "February 12 (1985)"
	QString prettyDate(const QDate& date);

	/// Returns the date in a relative, user-friendly format (ex: "Today", "Yesterday", "2 days ago", "March 18" (in current year), or "February 12 (1985)"
	QString prettyDate(const QDateTime& dt);


};

#endif // AMDATETIMEUTILS_H
