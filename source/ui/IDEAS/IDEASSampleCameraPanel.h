#ifndef IDEASSAMPLECAMERAPANEL_H
#define IDEASSAMPLECAMERAPANEL_H

#include <QWidget>

class AMMotorGroupObjectView;

class IDEASSampleCameraPanel : public QWidget
{
    Q_OBJECT
public:
    explicit IDEASSampleCameraPanel(QWidget *parent = 0);

signals:

public slots:

private:
    AMMotorGroupObjectView *sampleMotorGroupView_;
    AMMotorGroupObjectView *vacuumMotorGroupView_;


};

#endif // IDEASSAMPLECAMERAPANEL_H
