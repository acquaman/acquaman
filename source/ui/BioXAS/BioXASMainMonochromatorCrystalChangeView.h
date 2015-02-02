#ifndef BIOXASMAINMONOCRYSTALCHANGEVIEW_H
#define BIOXASMAINMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QMessageBox>

#include "ui/beamline/AMExtendedControlEditor.h"

#include "beamline/BioXAS/BioXASMainMonochromator.h"

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

protected:
    /// The monochromator being viewed.
    BioXASMainMonochromator *mono_;
    /// Label contents indicate whether the mono slits are closed.
    QLabel *slitsClosed_;
    /// Label contents indicate whether the mono phosphor paddle is out.
    QLabel *paddleOut_;
    /// Label contents indicate whether the mono key is turned to 'Enabled'.
    QLabel *keyStatus_;
    /// Control view shows the position of the mono bragg motor.
    AMExtendedControlEditor *braggMotorPosition_;
    /// Label contents indicate whether the bragg motor is at the crystal change position.
    QLabel *braggMotorAtCrystalChangePosition_;
    /// Label contents indicate whether the mono brake is enabled.
    QLabel *brakeStatus_;
    /// Control view shows the position of the mono crystal change motor.
    AMExtendedControlEditor *crystalChangeMotorPosition_;
    /// Label contents indicate whether the mono crystal change motor is at a CW limit.
    QLabel *crystalChangeMotorCWLimit_;
    /// Label contents indicate whether the mono crystal change motor is at a CCW limit.
    QLabel *crystalChangeMotorCCWLimit_;
};



#endif // BIOXASMAINMONOCRYSTALCHANGEVIEW_H
