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
