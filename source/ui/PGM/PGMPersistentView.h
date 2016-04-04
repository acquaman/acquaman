#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class QLabel;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles updating change in BPM value from 10ID.
    void onBPM10IDValueXChanged(double current);
    void onBPM10IDValueYChanged(double current);
    /// Handles updating change in BPM value from 11ID-#1.
    void onBPM11ID1ValueXChanged(double current);
    void onBPM11ID1ValueYChanged(double current);
    /// Handles updating change in BPM value from 11ID-#2.
    void onBPM11ID2ValueXChanged(double current);
    void onBPM11ID2ValueYChanged(double current);

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
