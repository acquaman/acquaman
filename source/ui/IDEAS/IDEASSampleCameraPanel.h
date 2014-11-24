#ifndef IDEASSAMPLECAMERAPANEL_H
#define IDEASSAMPLECAMERAPANEL_H

#include <QWidget>

class IDEASSampleManipulatorMotorGroupView;

class IDEASSampleCameraPanel : public QWidget
{
    Q_OBJECT
public:
    explicit IDEASSampleCameraPanel(QWidget *parent = 0);

signals:

public slots:

private:
    IDEASSampleManipulatorMotorGroupView *sampleMotorGroupView_, *vacuumMotorGroupView_;


};

#endif // IDEASSAMPLECAMERAPANEL_H
