#ifndef BEAMPOSITIONMONITOR_H
#define BEAMPOSITIONMONITOR_H

#include <QObject>
#include "beamline/AMProcessVariable.h"


class BeamPositionMonitor : public QObject
{
    Q_OBJECT
public:
    //Constructor for 1-dim bpm value
    explicit BeamPositionMonitor(const QString &name,
                                 const QString &pv,
                                 QObject *parent = 0);

    QString axisName() const { return axisName_; }

signals:
    void rmsChanged(double emitValue);


public slots:


protected slots:
    void bpmPVChanged(double value);

protected:
    QString axisName_;
    QString pvAxisRMS_;


};

#endif // BEAMPOSITIONMONITOR_H
