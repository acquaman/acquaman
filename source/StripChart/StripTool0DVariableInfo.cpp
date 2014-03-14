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
