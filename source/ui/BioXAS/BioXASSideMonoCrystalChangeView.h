#ifndef BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
#define BIOXASSIDEMONOCRYSTALCHANGEVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonoCrystalChangeSteps;



class BioXASSideMonoCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    explicit BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent = 0);
    virtual ~BioXASSideMonoCrystalChangeView();

protected slots:
    void onStepChanged();

protected:
    BioXASSideMonoCrystalChangeSteps *steps_;

};



class BioXASSideMonoCrystalChangeSteps : public QObject
{
    Q_OBJECT

public:
    enum Step { BeforeBrakeOn, BrakeOn, AtCrystalChangePosition, KeyTurned, BrakeOff, Complete};
    explicit BioXASSideMonoCrystalChangeSteps(BioXASSideMonochromator* mono, QObject *parent = 0);
    virtual ~BioXASSideMonoCrystalChangeSteps();

    BioXASSideMonoCrystalChangeSteps::Step currentStep() const;
    BioXASSideMonochromator* mono() const;

signals:
    void stepChanged(Step newCurrentStep);
    void monoChanged(BioXASSideMonochromator* newMono);

public slots:
    void reset();
    void setMono(BioXASSideMonochromator* newMono);

protected slots:
    void setNextStep(Step nextStep);

    void onMonoBrakeStatusChanged();
    void onMonoPositionStatusChanged();
    void onMonoKeyStatusChanged();

protected:
    Step currentStep_;
    BioXASSideMonochromator* mono_;

};

#endif // BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
