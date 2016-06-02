#include "PGMBladeCurrentView.h"

#include "beamline/PGM/PGMBeamline.h"
#include "ui/PGM/PGMPicoAmmeterView.h"

#include <QVBoxLayout>

PGMBladeCurrentView::PGMBladeCurrentView(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *bladeCurrentLayout = new QVBoxLayout;
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->exitSlitLowerBladeCurrentA()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->exitSlitUpperBladeCurrentA()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->exitSlitLowerBladeCurrentB()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->exitSlitUpperBladeCurrentB()));

    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->entranceSlitLowerBladeCurrent()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->entranceSlitUpperBladeCurrent()));

    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->teyBladeCurrentControl()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->flyBladeCurrentControl()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->i0EndstationBladeCurrentControl()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->i0BeamlineBladeCurrentControl()));
    bladeCurrentLayout->addWidget(new PGMPicoAmmeterView(PGMBeamline::pgm()->photodiodeBladeCurrentControl()));

    setLayout(bladeCurrentLayout);
}


void PGMBladeCurrentView::onExitSlitLowerBladeCurrentAChange(double value){

    exitSlitLowerBladeCurrentAValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}

void PGMBladeCurrentView::onExitSlitUpperBladeCurrentAChange(double value){

    exitSlitUpperBladeCurrentAValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}


void PGMBladeCurrentView::onExitSlitLowerBladeCurrentBChange(double value){

    exitSlitLowerBladeCurrentBValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}


void PGMBladeCurrentView::onExitSlitUpperBladeCurrentBChange(double value){

    exitSlitUpperBladeCurrentBValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}


void PGMBladeCurrentView::onEntranceSlitLowerBladeCurrentChange(double value){

    entranceSlitLowerBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'e', 2));
}


void PGMBladeCurrentView::onEntranceSlitUpperBladeCurrentChange(double value){

    entranceSlitUpperBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'e', 2));
}

void PGMBladeCurrentView::onTEYBladeCurrentChange(double value){

    teyBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}

void PGMBladeCurrentView::onFLYBladeCurrentChange(double value){

    flyBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}

void PGMBladeCurrentView::onI0EndstationBladeCurrentChange(double value){

    i0EndstationBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}


void PGMBladeCurrentView::onI0BeamlineBladeCurrentChange(double value){

    i0BeamlineBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}

void PGMBladeCurrentView::onPhotodiodeBladeCurrentChange(double value){

    photodiodeBladeCurrentValue_->setText(QString("%1 nA").arg(value, 0, 'f', 2));
}




