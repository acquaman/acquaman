#ifndef FCAPP_H
#define FCAPP_H

#include <QWidget>
#include <QLCDNumber>
#include <QLabel>
#include <QPushButton>

class FCApp : public QWidget
{
    Q_OBJECT
public:
    explicit FCApp(QWidget *parent = 0);

signals:

public slots:

protected:
    QLabel *label_;
    QLCDNumber *currentValue_;

    //BPM
    QLabel *xAxisLabel_;
    QLabel *yAxisLabel_;
    QLCDNumber *xBMP_;
    QLCDNumber *yBMP_;

    QPushButton *bpmButton_;


};

#endif // FCAPP_H
