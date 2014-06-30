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


//#include "StripTool0DVariableInfo.h"

//#include <QColor>
//#include <QDebug>

//StripTool0DVariableInfo::StripTool0DVariableInfo(QObject *parent) :
//    StripToolBasicVariableInfo(parent)
//{
//    setGranularity(2);
//    setColorName(QColor(Qt::red).name());
//}



//QString StripTool0DVariableInfo::timeUnits() const {
////    if (hasTimeUnits())
////        return timeUnits_;
////    else
////        return "";
//    return timeUnits_;
//}



//int StripTool0DVariableInfo::granularity() const {
//    if (hasGranularity())
//        return granularity_;
//    else
//        return 0;
//}



//QString StripTool0DVariableInfo::colorName() const {
//    if (hasColor())
//        return colorName_;
//    else
//        return "";
//}



//void StripTool0DVariableInfo::setTimeUnits(const QString &newUnits) {
//}



//void StripTool0DVariableInfo::setGranularity(int newGranularity) {
//    if (newGranularity <= 0) {
//        qDebug() << "StripTool0DVariableInfo :: attempting to set granularity of" << name() << "to be less than or equal to zero. No changes made.";

//    } else if (granularity_ == newGranularity) {
//        qDebug() << "StripTool0DVariableInfo :: attempting to set granularity of" << name() << "to be the same value as it already is. No changes made.";

//    } else {
//        granularity_ = newGranularity;
//        hasGranularity_ = true;
//        emit infoChanged();
//    }

//}



//void StripTool0DVariableInfo::setColorName(const QString &newColor) {
//    if (!QColor(newColor).isValid()) {
//        qDebug() << "StripTool0DVariableInfo :: attempting to set color of" << name() << "to be an invalid color! No changes made.";

//    } else if (colorName_ == newColor) {
//        qDebug() << "StripTool0DVariableInfo :: attempting to set the color of" << name() << "to value identical to the old value. No changes made.";

//    } else {
//        colorName_ = newColor;
//        hasColor_ = true;
//        emit infoChanged();
//    }
//}
