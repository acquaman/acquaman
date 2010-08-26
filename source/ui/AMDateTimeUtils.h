#ifndef AMDATETIMEUTILS_H
#define AMDATETIMEUTILS_H
#include <QString>
#include <QDateTime>

class AMDateTimeUtils
{
public:
	AMDateTimeUtils();
	QString checkAndModifyDate(const QDateTime &dateTime);
};

#endif // AMDATETIMEUTILS_H
