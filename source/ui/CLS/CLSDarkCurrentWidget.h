#ifndef CLSDARKCURRENTWIDGET_H
#define CLSDARKCURRENTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QDebug>
#include <QLabel>

/// This is a class that allows a user to manually instigate the chain of events leading to a dark current correction measurement.
class CLSDarkCurrentWidget : public QWidget
{
    Q_OBJECT

public:
    /// This constructor takes a double value to initialize the dwell time displayed in the dwell time entry field.
    explicit CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent = 0);
    ~CLSDarkCurrentWidget();

signals:
    /// Signal emitted when the dark current button is clicked, and the dwell time is reasonable.
    void darkCurrentButtonClicked(double dwellSeconds);

public slots:
    /// Disables the dark current button, for when there is a dark current measurement being taken.
    void disableDarkCurrentButton();
    /// Enables the dark current button, for when it's possible to take a new dark current measurement.
    void enableDarkCurrentButton();
    /// Sets the button to en/disabled, according to the boolean argument.
    void setDarkCurrentButtonEnabled(bool isEnabled);

protected slots:
    /// Checks that the dwell time entered is reasonable, assumes that it is possible to actually take a measurement now. Emits the 'darkCurrentButtonClicked' signal.
    void onDarkCurrentButtonClicked();

protected:
    /// The desired dwell time (in seconds) for the dark current measurement. Initialized to the dwellSeconds constructor argument.
    double dwellTime_;
    /// The entry widget for updating the desired dwell time.
    QLineEdit* dwellTimeEntry_;
    /// Pressing this button should start the process of taking a dark current measurement. Button should be disabled when a measurement isn't possible.
    QPushButton* darkCurrentButton_;

};

#endif // CLSDARKCURRENTWIDGET_H
