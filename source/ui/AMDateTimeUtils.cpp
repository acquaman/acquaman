#include "AMDateTimeUtils.h"
#include <QDateTime>

AMDateTimeUtils::AMDateTimeUtils()
{

}

//Is dateTime always stored in description role?
//How/where do I get the original dateTime?
//
QString AMDateTimeUtils::checkAndModifyDate(const QDateTime &dateTime){
	QDateTime currentDT = QDateTime::currentDateTime();
	if (dateTime.daysTo(currentDT)==0)
		return QString("today at ")+dateTime.time().toString("h:mm ap");
	else if (dateTime.daysTo(currentDT) == 1)
		return QString("yesterday at ")+dateTime.time().toString("h:mm ap");
		else if (dateTime.daysTo(currentDT)== 2)
			return QString("two days ago at ")+dateTime.time().toString("h:mm ap");
			else if (dateTime.daysTo(currentDT) == 3)
				return QString("three days ago at ")+dateTime.time().toString("h:mm ap");
				else return currentDT.toString("MMM d, (yyyy) h:mm ap");
				//change the date to two days ago

}
