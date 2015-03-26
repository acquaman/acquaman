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

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QDebug>
#include <QLabel>
#include <QDoubleSpinBox>

#define DARK_CURRENT_DWELL_TIME_MIN 0
#define DARK_CURRENT_DWELL_TIME_MAX 1000

/// This is a class that allows a user to manually instigate the chain of events leading to a dark current correction measurement.
class CLSDarkCurrentWidget : public QGroupBox
{
    Q_OBJECT

public:
    /// This constructor takes a double value to initialize the dwell time displayed in the dwell time entry field.
    explicit CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent = 0);
	/// Destructor.
    virtual ~CLSDarkCurrentWidget();

signals:
    /// Emitted when the collect button is clicked and the dwell time entered is positive and non-zero.
    void collectButtonClicked(double dwellSeconds);

public slots:
    /// Sets the collect button as enabled or disabled, according to the boolean argument.
    void setCollectButtonEnabled(bool isEnabled);

protected slots:
    /// Handles emitting collectButtonClicked() signal when the collect button is clicked, with the dwell time entered argument. Signal is emitted only if the time entered is greater than zero.
    void onCollectButtonClicked();

protected:
    /// The entry widget for updating the desired dwell time.
    QDoubleSpinBox* timeEntry_;
    /// Pressing this button should start the process of taking a dark current measurement. Button should be disabled when a measurement isn't possible.
    QPushButton* collectButton_;

};

#endif // CLSDARKCURRENTWIDGET_H
