#include "STTime.h"

STTime::STTime(int value, Units units, QObject *parent) : QObject(parent)
{
    value_ = value;
    units_ = units;
}

STTime::~STTime()
{

}

int STTime::value() const
{
    return value_;
}

STTime::Units STTime::units() const
{
    return units_;
}

QStringList STTime::unitsList()
{
    return QStringList() << "mSeconds" << "Seconds" << "Minutes" << "Hours";
}

double STTime::convertValue(int initialValue, STTime::Units initialUnits, STTime::Units finalUnits)
{
    double seconds = 0.0;
    double result = 0.0;

    if (initialUnits == STTime::mSeconds) {
        seconds = initialValue / 1000.0;

    } else if (initialUnits == STTime::Seconds) {
        seconds = initialValue;

    } else if (initialUnits == STTime::Minutes) {
        seconds = initialValue * 60.0;

    } else if (initialUnits == STTime::Hours) {
        seconds = initialValue * 60.0 * 60.0;

    } else {
        seconds = 0.0;
    }

    if (finalUnits == STTime::mSeconds) {
        result = seconds * 1000.0;

    } else if (finalUnits == STTime::Seconds) {
        result = seconds;

    } else if (finalUnits == STTime::Minutes) {
        result = seconds / 60.0;

    } else if (finalUnits == STTime::Hours) {
        result = seconds / 60.0 / 60.0;

    } else {
        result = 0.0;
    }

    return result;
}

QString STTime::unitsToString(STTime::Units units)
{
    if (units == mSeconds)
        return "mSeconds";

    if (units == Seconds)
        return "Seconds";

    if (units == Minutes)
        return "Minutes";

    if (units == Hours)
        return "Hours";

    return "";
}

STTime::Units STTime::stringToUnits(const QString &string)
{
    if (string == "mSeconds")
        return mSeconds;

    if (string == "Seconds")
        return Seconds;

    if (string == "Minutes")
        return Minutes;

    if (string == "Hours")
        return Hours;

    qDebug() << "STTime::stringToUnits // String not recognized. Returning default Seconds.";
    return Seconds;
}

void STTime::setValue(int newValue)
{
    if (value_ != newValue && newValue > 0) {
        value_ = newValue;
        emit valueChanged(value_);
    }
}

void STTime::setUnits(STTime::Units newUnits)
{
    if (units_ != newUnits) {
        units_ = newUnits;
        emit unitsChanged(units_);
    }
}
