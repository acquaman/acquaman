#ifndef BEAMPOSITIONMONITOR_H
#define BEAMPOSITIONMONITOR_H

#include <QObject>
#include "beamline/AMProcessVariable.h"
#include "MPlot/MPlotSeriesData.h"

/// This is the model class that retreives and stores the data coming from a PV, this case the BPM data.  The constructor BeamPositionMonitor() sets
/// the connections to the pv and stores it in QVector xValues_ and yValues_


class BeamPositionMonitor : public QObject
{
    Q_OBJECT
public:

    //Constructor for 2-dim bpm value, at first this just stored data from one pv but I will
    //attempt to have the constructor deal with x, y PV
    explicit BeamPositionMonitor(const QString &pvXbpm, const QString &pvYbpm, QObject *parent = 0);

    QList<double> xList() { return xValuesModel_; }
    QList<double> yList() { return yValuesModel_; }
    int NumberOfPoints() { return numberOfPoints; }

signals:
    void newValuesFromPV();


public slots:


protected slots:
    void updateXListfromPV(double value);
    void updateYListfromPV(double value);


protected:
    //The values that are retrieved from the PV are stored individually in here
    //using <double> as that's what the signal/slot for the PV class is set up for
    QList<double> xValuesModel_;
    QList<double> yValuesModel_;

    int numberOfPoints;

};

#endif // BEAMPOSITIONMONITOR_H
