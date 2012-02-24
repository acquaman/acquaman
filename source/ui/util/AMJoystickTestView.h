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
    ~AMJoystickTestView();

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
