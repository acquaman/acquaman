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


#ifndef TIMEENTRYWIDGET_H
#define TIMEENTRYWIDGET_H

#include <QWidget>
#include <QtGui>

class TimeEntryWidget : public QWidget
{
    Q_OBJECT

public:
    enum TimeUnits { Seconds = 0,
                     Minutes,
                     Hours
                   };
    explicit TimeEntryWidget(QWidget *parent = 0);
    ~TimeEntryWidget();

signals:
    void timeAmountChanged(int newAmount);
    void timeUnitsChanged(TimeEntryWidget::TimeUnits newUnits);

public:
    int timeAmount() const;
    TimeEntryWidget::TimeUnits timeUnits() const;

public slots:
    void setTimeAmount(int amount);
    void setTimeUnits(TimeEntryWidget::TimeUnits newUnits);

protected slots:
    void onTimeUnitsChanged(int newUnitsIndex);

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    int secondsMax_;
    int minutesMax_;
    int hoursMax_;

    QLabel *timeLabel_;
    QSpinBox *timeAmount_;
    QComboBox *timeUnits_;

};

#endif // TIMEENTRYWIDGET_H
