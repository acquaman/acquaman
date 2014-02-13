#ifndef IDEASPERSISTENTVIEW_H
#define IDEASPERSISTENTVIEW_H

#include <QWidget>

class QPushButton;
class QLabel;

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

protected:
    /// The beam on button.
    QPushButton *beamOnButton_;
    /// The beam off button.
    QPushButton *beamOffButton_;
    /// The label for the beam status.
    QLabel *beamStatusLabel_;
};

#endif // IDEASPERSISTENTVIEW_H
