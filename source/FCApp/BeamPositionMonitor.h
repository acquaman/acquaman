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

    int xValueCount() const { return xValues_.count(); }
    int yValueCount() const { return yValues_.count(); }
    void SendValues(int xIndex, int yIndex)  { sendValues(xIndex, yIndex); }


signals:
   void requestedValues(int xIndexValue, int yIndexValue);

public slots:


protected slots:
    void updateValueX(double value);
    void updateValueY(double value);

    void sendValues(int xIndex, int yIndex);


protected:
    QVector<qreal> xValues_;
    QVector<qreal> yValues_;

    QString xPV_;
    QString yPV_;

};

#endif // BEAMPOSITIONMONITOR_H
