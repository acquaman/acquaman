/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMGenericLinuxJoystick.h"

#include <QFile>
#include <QFileInfo>
#include <QTimer>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "util/AMErrorMonitor.h"

AMGenericLinuxJoystick::AMGenericLinuxJoystick(const QString &deviceIdentifier, QObject *parent) :
    AMJoystick(deviceIdentifier, parent)
{
    buttonCount_ = -1;
    axisCount_ = -1;

    readThread_ = 0;
    fd_ = -1;

    qRegisterMetaType<QVector<AMGenericLinuxJoystickEvent> >();

    attemptConnection();
}

AMGenericLinuxJoystick::~AMGenericLinuxJoystick()
{
    if(readThread_) {
        disconnect(readThread_, SIGNAL(finished()), this, SLOT(onReadThreadFinished()));
        readThread_->stop();
        readThread_->wait();
        delete readThread_;
        readThread_ = 0;
        close(fd_);
        fd_ = -1;
    }
}

bool AMGenericLinuxJoystick::isConnected() const
{
    return (readThread_ != 0);  // whenever we are connected, there will be a valid readThread_.
}


bool AMGenericLinuxJoystick::buttonState(int buttonId) const
{
    if(buttonId < 0 || buttonId >= buttonCount_)
        return false;


    return buttonStates_.at(buttonId);
}

int AMGenericLinuxJoystick::axisPosition(int axisId) const
{
    if(axisId < 0 || axisId >= axisCount_)
        return 0;

    return axisPositions_.at(axisId);
}


bool AMGenericLinuxJoystick::reconnect(const QString &deviceIdentifier)
{
    deviceIdentifier_ = deviceIdentifier;

    return attemptConnection();
}

bool AMGenericLinuxJoystick::attemptConnection()
{
    // Do we have a connection open already? Close the reading thread, and close the file descriptor.
    if(readThread_) {
        disconnect(readThread_, SIGNAL(finished()), this, SLOT(onReadThreadFinished()));
        readThread_->stop();
        readThread_->wait();
        delete readThread_;
        readThread_ = 0;
        close(fd_);
        fd_ = -1;
        AMErrorMon::information(this, 0, QString("Disconnected from joystick: '%1' before attempting to reconnect on '%2'").arg(deviceDescription()).arg(deviceIdentifier()));
        emit connected(false);
    }

    // Try opening the joystick device
    fd_ = open(deviceIdentifier_.toAscii().constData(), O_RDONLY | O_NONBLOCK);
    if(fd_ == -1) {
        // Failed to open. Maybe not plugged in yet? Just keep trying. We'll re-attempt in 2 seconds.
        QTimer::singleShot(2000, this, SLOT(attemptConnection()));
        return false;
    }

    // Get the number of buttons and number of axes, using special ioctl() on the file descriptor.
    char numAxes;
    ioctl(fd_, JSIOCGAXES, &numAxes);
    axisCount_ = numAxes;
    axisPositions_.resize(axisCount_);
    char numButtons;
    ioctl(fd_, JSIOCGBUTTONS, &numButtons);
    buttonCount_ = numButtons;
    buttonStates_.resize(buttonCount_);

    // Get the joystick description
    char description[256];
    if(ioctl(fd_, JSIOCGNAME(sizeof(description)), description) < 0)
        deviceDescription_ = "Unknown";
    else
        deviceDescription_ = QString(description);

    // Finally, start up the thread for reading from the joystick.
    readThread_ = new AMGenericLinuxJoystickThread(fd_, this);
    connect(readThread_, SIGNAL(finished()), this, SLOT(onReadThreadFinished()));
    connect(readThread_, SIGNAL(joystickEvents(QVector<AMGenericLinuxJoystickEvent>)), this, SLOT(onJoystickEvents(QVector<AMGenericLinuxJoystickEvent>)));
    readThread_->start();
    AMErrorMon::information(this, 0, QString("Connected to joystick: '%1' on '%2'.").arg(deviceDescription()).arg(deviceIdentifier()));
    emit connected(true);
    return true;
}

void AMGenericLinuxJoystick::onReadThreadFinished()
{
    delete readThread_;
    readThread_ = 0;
    close(fd_);
    fd_ = -1;

    QTimer::singleShot(2000, this, SLOT(attemptConnection()));  // try to keep re-connecting, if it gets plugged back in.

    AMErrorMon::alert(this, -1, QString("Disconnected from joystick: '%1' because of a read error. Is it possible the joystick was unplugged?").arg(deviceDescription()));
    emit connected(false);
}

void AMGenericLinuxJoystick::onJoystickEvents(QVector<AMGenericLinuxJoystickEvent> events)
{
    for(int i=0,cc=events.count(); i<cc; i++) {
        const AMGenericLinuxJoystickEvent& event = events.at(i);

        if ((event.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON) {
            bool newState = (bool)event.value;
            if(buttonStates_.at(event.number) != newState)
                emit buttonChanged(event.number, (buttonStates_[event.number] = newState), event.time);
        }
        if((event.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS) {
            int newValue = event.value;
            if(axisPositions_.at(event.number) != newValue)
                emit axisChanged(event.number, (axisPositions_[event.number] = newValue), event.time);
        }
    }
}




 AMGenericLinuxJoystickThread::~AMGenericLinuxJoystickThread(){}
AMGenericLinuxJoystickThread::AMGenericLinuxJoystickThread(int fileDescriptor, QObject* parent) :
    QThread(parent)
{
    runFlag_ = true;
    fd_ = fileDescriptor;
}

void AMGenericLinuxJoystickThread::run()
{

    QVector<AMGenericLinuxJoystickEvent> events(255);

    while(runFlag_) {
        // do a non-blocking read, of as many events as are available. Will return error EAGAIN if no data.
        int bytesRead;
        while( (bytesRead = read(fd_, events.data(), sizeof(AMGenericLinuxJoystickEvent)*255)) > 0) {
            // process events
            int eventsRead = bytesRead / sizeof(AMGenericLinuxJoystickEvent);
            QVector<AMGenericLinuxJoystickEvent> eventsCopy(eventsRead);
            for(int i=0; i<eventsRead; i++)
                eventsCopy[i] = events[i];
            emit joystickEvents(eventsCopy);
        }

        /* EAGAIN is returned when the queue is empty */
        if(errno != EAGAIN) {
		AMErrorMon::alert(this, AMGENERICLINUXJOYSTICK_READ_ERROR_POSSIBLE_DISCONNECT, "AMGenericLinuxJoystick: Read error. Is it possible that the joystick was disconnected?");
            return;
        }
        usleep(250);
    }
}
