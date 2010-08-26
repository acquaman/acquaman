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
