#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class QPushButton;
class AMAction3;


/// A user interface class providing user access to core controls on
/// the VLS-PGM beamline.
class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

protected slots:
    /// Turning beam on when button clicked. Talks to PGMBeamline class to open valves and shutters.
    void onBeamOnButtonClicked();
    /// Disconnects signals and deletes beam action when action on is finished/failed.
    void onBeamOnActionFinished();
    /// Turning beam off when button clicked.
    void onBeamOffButtonClicked();
    /// Disconnects signals and deletes beam action when action off is finished/failed.
    void onBeamOffActionFinished();

protected:
    /// Button to turn on beam.
    QPushButton *beamOnButton_;
    /// Button to turn off beam.
    QPushButton *beamOffButton_;
    /// Beam On Action. Instansiated in the onBeamOnButtonClicked slot.
    AMAction3 *beamOnAction_;
    /// Beam Off Action. Instansiated in the onBeamOffButtonClicked slot.
    AMAction3 *beamOffAction_;


    /// Control Editor for Exit Slit Gap
    AMExtendedControlEditor *exitSlitGapControlEditor_;
    /// Control Editor for Entrance Slit Gap
    AMExtendedControlEditor *entranceSlitGapControlEditor_;

};

#endif // PGMPERSISTENTVIEW_H
