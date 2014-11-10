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


#ifndef CLSDARKCURRENTWIDGET_H
#define CLSDARKCURRENTWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLayout>
#include <QDebug>
#include <QLabel>

/// This is a class that allows a user to manually instigate the chain of events leading to a dark current correction measurement.
class CLSDarkCurrentWidget : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent = 0);
    /// Destructor.
    virtual ~CLSDarkCurrentWidget();

signals:
    /// Signal emitted when the dark current button is clicked, and the dwell time is reasonable.
    void collectDarkCurrentMeasurement(double dwellSeconds);

public slots:
    /// Sets the dark current measurement button to en/disabled, according to the boolean argument.
    void setDarkCurrentButtonEnabled(bool isEnabled);

protected slots:
    /// Checks that the dwell time entered is reasonable, assumes that it is possible to actually take a measurement now. Emits the 'collectDarkCurrentMeasurement' signal.
    void onDarkCurrentButtonClicked();

protected:
    /// The desired dwell time (in seconds) for the dark current measurement. Initialized to the dwellSeconds constructor argument.
    double dwellTime_;
    /// The entry widget for updating the desired dwell time.
    QDoubleSpinBox* dwellTimeEntry_;
    /// Pressing this button should start the process of taking a dark current measurement. Button should be disabled when a measurement isn't possible.
    QPushButton* darkCurrentButton_;

};

#endif // CLSDARKCURRENTWIDGET_H
