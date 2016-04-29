#ifndef PGMBPMSTATUSVIEW_H
#define PGMBPMSTATUSVIEW_H

#include <QWidget>

class QLabel;
class QGroupBox;
class QGridLayout;

/// Provides a widget view of the beam position monitor information coming from PGMBPMControl.
/// Communicates with PGMBPMControl to change display if values go above or below a bound.

class PGMBPMStatusView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMBPMStatusView(QWidget *parent = 0);


    /// Setup layout and widgets for PGMBpmControls
    void setupBPMDisplay();
    /// Setup the connections required for updating bpm widgets
    void setupBPMConnections();


signals:

public slots:

protected slots:
    /// If BPM value falls out of range the state is set to false and the appriopriate label
    /// turns red. Otherwise set true and the color is black.
    /// Handles updating change in BPM state from 10ID.
    void onBPM10IDxStatusChanged(bool state);
    void onBPM10IDyStatusChanged(bool state);
    /// Handles updating change in BPM state from 11ID-#1.
    void onBPM11ID1xStatusChanged(bool state);
    void onBPM11ID1yStatusChanged(bool state);
    /// Handles updating change in BPM state from 11ID-#2.
    void onBPM11ID2xStatusChanged(bool state);
    void onBPM11ID2yStatusChanged(bool state);

protected:
    /// Grid layout for the 6 different BPM values.
    QGridLayout *bpmLayout_;

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

#endif // PGMBPMSTATUSVIEW_H
