#ifndef BIOXASSIDEMONOBASICENERGYVIEW_H
#define BIOXASSIDEMONOBASICENERGYVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonoBasicEnergyView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoBasicEnergyView(BioXASSideMonochromator *mono, QWidget *parent = 0);
    /// Desctructor.
    virtual ~BioXASSideMonoBasicEnergyView();

    /// Returns the monochromator this view is displaying.
    BioXASSideMonochromator *mono() const;

signals:

public slots:
    void setMono(BioXASSideMonochromator *mono);

protected slots:
    void onEnergySetpointChanged(double newEnergy);
    void onMonoConnectedChanged();
    void onMonoEnergyChanged(double newEnergy);

protected:
    BioXASSideMonochromator *mono_;

    QDoubleSpinBox *energySetpoint_;
    QLabel *energy_;

};

#endif // BIOXASSIDEMONOBASICENERGYVIEW_H
