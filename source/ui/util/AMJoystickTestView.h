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


#ifndef AMJOYSTICKTESTVIEW_H
#define AMJOYSTICKTESTVIEW_H

#include <QWidget>

namespace Ui {
class AMJoystickTestView;
}

class AMJoystick;
class QProgressBar;
class QToolButton;
class QLabel;

/// This widget provides a programming example of how to use AMJoystick, and also provides a convenient UI display for testing a joystick connection.
class AMJoystickTestView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.  You must pass in a valid \c joystick.
    AMJoystickTestView(AMJoystick* joystick, QWidget *parent = 0);
    virtual ~AMJoystickTestView();

protected:
    AMJoystick* joystick_;
    QVector<QProgressBar*> axes_;
    QVector<QToolButton*> buttons_;
    QVector<QLabel*> labels_;

protected slots:
    /// Called when the joystick connects or disconnects.
    void onJoystickConnected(bool isConnected);
    /// Called when a button is pressed.
    void onButtonPressed(int id, bool value);
    /// Called when an axis is moved.
    void onAxisMoved(int id, int position);

private:
    Ui::AMJoystickTestView *ui;
};

#endif // AMJOYSTICKTESTVIEW_H
