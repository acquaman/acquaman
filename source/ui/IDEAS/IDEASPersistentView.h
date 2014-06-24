#ifndef IDEASPERSISTENTVIEW_H
#define IDEASPERSISTENTVIEW_H

#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"

#include <QWidget>
#include <QTimer>


class QPushButton;
class QLabel;
class QProgressBar;

class AMExtendedControlEditor;

/// The persistent panel that lives on the right side of the application.
class IDEASPersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit IDEASPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles the state change to turn the beam on for the beamline.
    void onBeamOnClicked();
    /// Handles the state change to turn the beam off for the beamline.
    void onBeamOffClicked();
    /// Handles changing the state of the beam on and beam off buttons based on the state of the shutters.
    void onShutterStatusChanged(bool state);
    /// Calls dialogue to calibrate beamline energy
    void onCalibrateClicked();


    /// Handles updating Old ion chamber current bar
    void onOldCountsChanged();
    /// Handles updating I0 ion chamber current bar
    void onI0CountsChanged();
    /// Handles updating Sample ion chamber current bar
    void onSampleCountsChanged();
    /// Handles updating Reference ion chamber current bar
    void onReferenceCountsChanged();

    /// Handles updating displayed crystal and energy range
    void onCrystalChanged();

    /// Handles updating displayed ring current
    void onRingCurrentChanged(double);



protected:
    /// The beam on button.
    QPushButton *beamOnButton_;
    /// The beam off button.
    QPushButton *beamOffButton_;
    /// The label for the beam status.
    QLabel *beamStatusLabel_;
    /// The beam on button.
    QPushButton *calibrateButton_;


    AMExtendedControlEditor *energyControlEditor_;

    QLabel *ringCurrent_;
    QLabel *monoCrystal_;
    QLabel *monoEnergyRange_;

    QLabel *IOldLabel_;
    QLabel *I0Label_;
    QLabel *ISampleLabel_;
    QLabel *IReferenceLabel_;

    QLabel *IOldValueLabel_;
    QLabel *I0ValueLabel_;
    QLabel *ISampleValueLabel_;
    QLabel *IReferenceValueLabel_;

    QProgressBar *IOldBar_;
    QProgressBar *I0Bar_;
    QProgressBar *ISampleBar_;
    QProgressBar *IReferenceBar_;

    AMBeamlineCameraWidgetWithSourceTabs *cameraWidget;

//    QTimer *crystalTimer_;



};

#endif // IDEASPERSISTENTVIEW_H
