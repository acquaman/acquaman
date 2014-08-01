#ifndef FCAPP_H
#define FCAPP_H

#include <QWidget>
#include <QLCDNumber>
#include <QLabel>

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


};

#endif // FCAPP_H
