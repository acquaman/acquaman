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


#ifndef AMJOYSTICK_H
#define AMJOYSTICK_H

#include <QObject>

/// Defines the interface for joystick connections to interact with Acquaman programs. This interface should be implemented by operating-system and hardware-specific (if necessary) implementations.
class AMJoystick : public QObject
{
    Q_OBJECT
public:
    /// Constructor.  \c deviceIdentifier is an arbitrary string that can be used by implementations to locate or identify the joystick.  Implementations should try to connect to the joystick when constructed.
    AMJoystick(const QString& deviceIdentifier, QObject *parent = 0);
    virtual ~AMJoystick() {}

    // Polling the joystick state:
    /////////////////////////////////

    /// Returns true if the joystick is connected, otherwise returns false.
    virtual bool isConnected() const = 0;

    /// Returns the number of buttons present on the joystick, or -1 if not connected.
    virtual int buttonCount() const = 0;
    /// Returns the number of axes present on the joystick, or -1 if not connected.
    virtual int axisCount() const = 0;

    /// Returns the state a joystick button: true if pressed, false if currently released. \c buttonId must be >= 0 and < buttonCount().
    virtual bool buttonState(int buttonId) const = 0;
    /// Returns the position of a joystick axis.  \c axisId must be >= 0 and < axisCount().
    virtual int axisPosition(int axisId) const = 0;



    /// Returns the string that will be used (by the implementation) to try to locate a joystick when calling reconnect(). The format of the string could depend on the implementation, for example, on Linux this could be a device file name like /dev/input/js0.
    QString deviceIdentifier() const { return deviceIdentifier_; }

    /// Returns the name/description of the particular joystick connected, or an empty string if not connected. This could be something like "Sony PLAYSTATION(R)3 Controller" or whatever else the joystick reports itself as.
    virtual QString deviceDescription() const = 0;


signals:

    // Sending joystick events. Implementations should take care of emitting these signals.

    /// Emitted with true when the joystick connects, and false when it disconnects.
    void connected(bool);

    /// Emitted when a button is pressed or released. \c isDown will be true when pressed, false when released. \c eventTimeMs is the precise time when the button event happened, measured in ms from some arbitrary but constant time in the past.
    void buttonChanged(int buttonId, bool isDown, quint32 eventTimeMs);
    /// Emitted when an axis is moved. \c position will contain the most up-to-date axis position. \c eventTimeMs is the precise time when the axis event happened, measured in ms from some arbitrary but constant time in the past.
    void axisChanged(int axisId, int position, quint32 eventTimeMs);



public slots:

    /// Attempt to connect (or disconnect and reconnect) to a joystick. \c deviceIdentifier is an implementation-defined string that specifies how to identify the joystick.  Returns true if the connection was made successfully.
    /*! Implementations can fail here and connect asynchronously later... They should just emit the connected() signal whenever their connection state changes.

    The base class implementation just sets deviceIdentifier_ and returns false.
*/
    virtual bool reconnect(const QString& deviceIdentifier) { deviceIdentifier_ = deviceIdentifier; return false; }



protected:
    QString deviceIdentifier_;

};

#endif // AMJOYSTICK_H
