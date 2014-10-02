#ifndef IDEASSAMPLECAMERAPANEL_H
#define IDEASSAMPLECAMERAPANEL_H

#include <QWidget>

#include "ui/beamline/AMControlMoveButton.h"


class IDEASSampleCameraPanel : public QWidget
{
    Q_OBJECT
public:
    explicit IDEASSampleCameraPanel(QWidget *parent = 0);

signals:

public slots:

private:
    AMControlMoveButton *sampleUp_, *sampleDown_,*sampleInboard_, *sampleOutboard_, *vacuumStageDown_, *vacuumStageUp_;

};

#endif // IDEASSAMPLECAMERAPANEL_H
