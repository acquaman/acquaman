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


#ifndef AMGENERICLINUXJOYSTICK_H
#define AMGENERICLINUXJOYSTICK_H

#include "util/AMJoystick.h"
#include <QVector>
#include <QThread>
#include <QMetaType>

#include <linux/joystick.h>

#define AMGENERICLINUXJOYSTICK_READ_ERROR_POSSIBLE_DISCONNECT 290701

/// Structure used to represent joystick events by the Linux joystick API.
//typedef struct js_event {
//    quint32 time;     /* event timestamp in milliseconds */
//    qint16 value;    /* value */
//    quint8 type;      /* event type */
//    quint8 number;    /* axis/button number */
//} AMGenericLinuxJoystickEvent;

typedef struct js_event AMGenericLinuxJoystickEvent;

Q_DECLARE_METATYPE(AMGenericLinuxJoystickEvent)
Q_DECLARE_METATYPE(QVector<AMGenericLinuxJoystickEvent>)


/// Used by AMGenericLinuxJoystick: a thread to do (blocking) reads from the joystick file
class AMGenericLinuxJoystickThread : public QThread {
    Q_OBJECT
public:
    /// Constructor. \c fileDescriptor is the open file handle to read from.
 	virtual ~AMGenericLinuxJoystickThread();
    AMGenericLinuxJoystickThread(int fileDescriptor, QObject* parent = 0);

    /// Run function (in separate thread)
    void run();
    /// Call this (from any other thread) to ask the thread to shut down
    void stop() { runFlag_ = false; }

signals:
    /// Sent for every batch of events read at once. Will be picked up and processed in the main thread.
    void joystickEvents(QVector<AMGenericLinuxJoystickEvent> events);

protected:
    /// Set this to false to shut down the thread.
    volatile bool runFlag_;
    /// The file descriptor to read from (already open by the time we get it)
    int fd_;

};

/// Implements the AMJoystick interface on Linux systems via the standard joystick driver (joydev) that creates devices in /dev/input/js0, /dev/input/js1, etc. It should work with any linux-compatible joystick.
/*! For information on the OS interface that we use, see http://www.kernel.org/doc/Documentation/input/joystick-api.txt */
class AMGenericLinuxJoystick : public AMJoystick
{
    Q_OBJECT
public:
    /// Constructor.  \c deviceIdentifier should be a device file like /dev/input/js0.  It doesn't matter if the device is connected yet or not... If the device file doesn't exist, we will silently try to keep connecting.
    AMGenericLinuxJoystick(const QString& deviceIdentifier, QObject *parent = 0);
    /// Destructor: closes the connection to the joystick, if open
    virtual ~AMGenericLinuxJoystick();

    // Polling the joystick state:
    /////////////////////////////////

    /// Returns true if the joystick is connected, otherwise returns false.
    virtual bool isConnected() const;

    /// Returns the number of buttons present on the joystick, or -1 if not connected.
    virtual int buttonCount() const { return buttonCount_; }
    /// Returns the number of axes present on the joystick, or -1 if not connected.
    virtual int axisCount() const { return axisCount_; }

    /// Returns the state a joystick button: true if pressed, false if currently released. \c buttonId must be >= 0 and < buttonCount().
    virtual bool buttonState(int buttonId) const;
    /// Returns the position of a joystick axis.  \c axisId must be >= 0 and < axisCount().
    virtual int axisPosition(int axisId) const;


    /// Returns the name/description of the particular joystick connected, or an empty string if not connected. This could be something like "Sony PLAYSTATION(R)3 Controller" or whatever else the joystick reports itself as.
    virtual QString deviceDescription() const { return deviceDescription_; }


signals:

    // Sending joystick events. Implementations should take care of emitting these signals.

    // Emitted with true when the joystick connects, and false when it disconnects.
    // void connected(bool);

    // Emitted when a button is pressed or released. \c isDown will be true when pressed, false when released.
    // defined in base class: void buttonChanged(int buttonId, bool isDown);

    // Emitted when an axis is moved. \c position will contain the most up-to-date axis position.
    // defined in base class: void axisChanged(int axisId, int position);



public slots:

    /// Attempt to connect (or disconnect and reconnect) to a joystick. \c deviceIdentifier is an implementation-defined string that specifies how to identify the joystick.  Returns true if the connection was made successfully.
    /*! Implementations can fail here and connect asynchronously later... They should just emit the connected() signal whenever their connection state changes.*/
    virtual bool reconnect(const QString& deviceIdentifier);


protected:
    QString deviceDescription_;
    int buttonCount_;
    int axisCount_;

    QVector<bool> buttonStates_;
    QVector<int> axisPositions_;

    /// A pointer to the AMGenericLinuxJoystickThread object for reading from the joystick.  0 if not connected. Valid while connected.
    AMGenericLinuxJoystickThread* readThread_;
    /// The file descriptor used to read from the joystick
    int fd_;

protected slots:

    /// Helper function to attempt to connect to the device at deviceIdentifier_.  If not successful, calls itself again in 2s.
    bool attemptConnection();

    /// Called when the read thread encounters an error (ex: joystick disconnected) and exits
    void onReadThreadFinished();

    /// Called when the read thread passes us a joystick event to process.
    void onJoystickEvents(QVector<AMGenericLinuxJoystickEvent> events);

};

#endif // AMGENERICLINUXJOYSTICK_H
