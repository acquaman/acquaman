#ifndef BIOXASMAINMONOCRYSTALCHANGEVIEW_H
#define BIOXASMAINMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>

#include "beamline/BioXAS/BioXASMainMonochromator.h"
#include "source/ui/BioXAS/BioXASMainMonochromatorCrystalChangeControlView.h"

class BioXASMainMonochromatorCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeView();

signals:
    /// Notifier that the monochromator being viewed has changed.
    void monoChanged(BioXASMainMonochromator *newMono);

public slots:
    /// Sets the monochromator being viewed.
    void setMono(BioXASMainMonochromator *newMono);
    /// Updates the view by polling the mono for its current states.
    void updateView();

protected slots:
    /// Handles updating the entire view when the mono connection state has changed.
    void onMonoConnectedChanged();
    /// Handles updating the region view when the region status changes.
    void onRegionStatusChanged();
    /// Handles updating the slits closed view when the closed status changes.
    void onSlitsClosedChanged();
    /// Handles updating the paddle view when the paddle in/out status changes.
    void onPaddleOutChanged();
    /// Handles updating the key status view when the key is turned.
    void onKeyStatusChanged();
    /// Handles updating the bragg motor view when the motor position changes.
    void onBraggMotorPositionChanged();
    /// Handles updating the bragg motor view when the motor arrives at/leaves the crystal change position.
    void onBraggMotorAtCrystalChangePositionChanged();
    /// Handles updating the brake status view when the brake switch is flipped.
    void onBrakeStatusChanged();
    /// Handles updating the crystal change motor cw limit status view.
    void onCrystalChangeMotorCWLimitStatusChanged();
    /// Handles updating the crystal change motor ccw limit status view.
    void onCrystalChangeMotorCCWLimitStatusChanged();
    /// Handles creating and running the crystal change control view, in response to the crystal change button clicked.
    void onCrystalChangeButtonClicked();

protected:
    /// The monochromator being viewed.
    BioXASMainMonochromator *mono_;
    /// The text displayed when a particular component is disconnected.
    QString disconnectedText_;
    /// Label contains the region status.
    QLabel *region_;
    /// Label contains the slits closed status.
    QLabel *slitsClosed_;
    /// Label contains the paddle out status.
    QLabel *paddleOut_;
    /// Label contains the key status.
    QLabel *keyStatus_;
    /// Label contains the bragg motor position.
    QLabel *braggMotorPosition_;
    /// Label contains whether the bragg motor is at the crystal change position.
    QLabel *braggMotorAtCrystalChangePosition_;
    /// Label contains the brake status.
    QLabel *brakeStatus_;
//    /// Label contains the crystal change motor position.
//    QLabel *crystalChangeMotorPosition_;
    /// Label contains the mono crystal change motor CW limit status.
    QLabel *crystalChangeMotorCWLimit_;
    /// Label contains the mono crystal change motor CCW limit status.
    QLabel *crystalChangeMotorCCWLimit_;
    /// Button that opens the crystal change control view. It should be invalid/hidden if the mono is invalid or disconnected.
    QPushButton *crystalChangeButton_;
};



#endif // BIOXASMAINMONOCRYSTALCHANGEVIEW_H
