#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class QLabel;
class PGMBeamStatusView;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

    /// Storing the average (x, y) values for the beam position for 10ID, 11ID-1 and 11ID-2
    /// When the bpm changes, they are allowed to change within +/- 50 or a flag will be raised (color change in text).
    int avg10IDx_, avg10IDy_, avg11ID1x_, avg11ID1y_, avg11ID2x_, avg11ID2y_;
    int bpmVariance_;


    PGMBeamStatusView *beamStatusView_;

signals:

public slots:

protected slots:
    /// Handles updating change in BPM value from 10ID.
    void onBPM10IDValueXChanged(double value);
    void onBPM10IDValueYChanged(double value);
    /// Handles updating change in BPM value from 11ID-#1.
    void onBPM11ID1ValueXChanged(double value);
    void onBPM11ID1ValueYChanged(double value);
    /// Handles updating change in BPM value from 11ID-#2.
    void onBPM11ID2ValueXChanged(double value);
    void onBPM11ID2ValueYChanged(double value);

protected:

    /// Labels representing current BPM value.
    QLabel *bpm10IDvalueX_;
    QLabel *bpm10IDvalueY_;
    QLabel *bpm11ID1valueX_;
    QLabel *bpm11ID1valueY_;
    QLabel *bpm11ID2valueX_;
    QLabel *bpm11ID2valueY_;
    /// BPM description labels
    QLabel *bpmXValueLabel_;
    QLabel *bpmYValueLabel_;
    QLabel *bpm10IDLabel_;
    QLabel *bpm11ID1Label_;
    QLabel *bpm11ID2Label_;


};

#endif // PGMPERSISTENTVIEW_H
