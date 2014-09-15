#ifndef BIOXASSIDEMONOCHROMATORCRYSTALCHANGEVIEW_H
#define BIOXASSIDEMONOCHROMATORCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonochromatorCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    explicit BioXASSideMonochromatorCrystalChangeView(BioXASSideMonochromator *mono, QWidget *parent = 0);
    virtual ~BioXASSideMonochromatorCrystalChangeView();

signals:
    void monochromatorChanged(BioXASSideMonochromator *newMono);

public slots:
    void setMonochromator(BioXASSideMonochromator *mono);

protected:
    void setBrakeStatus(bool on);
    void setPositionStatus(bool inPosition);
    void setKeyStatus(bool turned);

    void onMonoBrakeStatusChanged();
    void onMonoCrystalChangePositionStatusChanged();
    void onMonoKeyStatusChanged();

protected:
    BioXASSideMonochromator *mono_;

    QLabel *brakeStatus_;
    QLabel *positionStatus_;
    QLabel *keyStatus_;

};

#endif // BIOXASSIDEMONOCHROMATORCRYSTALCHANGEVIEW_H
