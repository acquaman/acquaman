#ifndef TIMEENTRYWIDGET_H
#define TIMEENTRYWIDGET_H

#include <QWidget>
#include <QtGui>

class TimeEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimeEntryWidget(QWidget *parent = 0);
    ~TimeEntryWidget();

signals:
    void timeAmountChanged(int newAmount);
    void timeUnitsChanged(const QString &newUnits);
    void setUnits(int index);

public:


protected:
    int secondsMax_;
    int minutesMax_;
    int hoursMax_;

    QLabel *timeLabel_;
    QSpinBox *timeAmount_;
    QComboBox *timeUnits_;

protected:
    void initialize();

protected slots:
    void toTestSignal(const QString &signalText);
    void timeUpdateRequested();

};

#endif // TIMEENTRYWIDGET_H