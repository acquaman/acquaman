#include "AMJoystick.h"

AMJoystick::AMJoystick(const QString &deviceIdentifier, QObject *parent) :
    QObject(parent), deviceIdentifier_(deviceIdentifier)
{
}
