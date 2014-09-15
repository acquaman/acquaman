#ifndef BIOXASSIDEMONOCHROMATORVIEWBASIC_H
#define BIOXASSIDEMONOCHROMATORVIEWBASIC_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonochromatorViewBasic : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonochromatorViewBasic(QWidget *parent = 0);
    /// Desctructor.
    virtual ~BioXASSideMonochromatorViewBasic();

    /// Returns the monochromator this view is displaying.
    BioXASSideMonochromator *mono() const;

signals:

public slots:
    void setMonochromator(BioXASSideMonochromator *mono);

protected slots:
    void onEnergyChanged();
    void onMonoEnergyChanged();

protected:
    BioXASSideMonochromator *mono_;
    QDoubleSpinBox *energy_;
    QLabel *energyFeedback_;

};

#endif // BIOXASSIDEMONOCHROMATORVIEWBASIC_H
