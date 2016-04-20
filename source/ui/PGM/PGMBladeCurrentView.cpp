#include "PGMBladeCurrentView.h"
#include "beamline/PGM/PGMBeamline.h"

#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>

PGMBladeCurrentView::PGMBladeCurrentView(QWidget *parent) :
    QWidget(parent)
{
    exitSlitLowerBladeCurrentALabel_ = new QLabel("Exit A Lower:");
    exitSlitLowerBladeCurrentAValue_ = new QLabel("  nA");

    exitSlitUpperBladeCurrentALabel_ = new QLabel("Exit A Upper:");
    exitSlitUpperBladeCurrentAValue_ = new QLabel("  nA");

    exitSlitLowerBladeCurrentBLabel_ = new QLabel("Exit B Lower:");
    exitSlitLowerBladeCurrentBValue_ = new QLabel("  nA");

    exitSlitUpperBladeCurrentBLabel_ = new QLabel("Exit B Upper:");
    exitSlitUpperBladeCurrentBValue_ = new QLabel("  nA");

    entranceSlitLowerBladeCurrentLabel_ = new QLabel("Ent. Lower:");
    entranceSlitLowerBladeCurrentValue_ = new QLabel("  nA");

    entranceSlitUpperBladeCurrentLabel_ = new QLabel("Ent. Upper:");
    entranceSlitUpperBladeCurrentValue_ = new QLabel("  nA");

    teyBladeCurrentLabel_ = new QLabel("TEY:");
    teyBladeCurrentValue_ = new QLabel("  nA");

    flyBladeCurrentLabel_ = new QLabel("FLY:");
    flyBladeCurrentValue_ = new QLabel("  nA");

    i0EndstationBladeCurrentLabel_ = new QLabel("I0 Endstation:");
    i0EndstationBladeCurrentValue_ = new QLabel("  nA");

    i0BeamlineBladeCurrentLabel_ = new QLabel("I0 Beamline:");
    i0BeamlineBladeCurrentValue_ = new QLabel("  nA");

    photodiodeBladeCurrentLabel_ = new QLabel("Photodiode:");
    photodiodeBladeCurrentValue_ = new QLabel("  nA");


    QGridLayout *bladeCurrentLayout = new QGridLayout();
    bladeCurrentLayout->addWidget(entranceSlitUpperBladeCurrentLabel_, 0, 0, 1, 1);
    bladeCurrentLayout->addWidget(entranceSlitUpperBladeCurrentValue_, 0, 1, 1, 1);
    bladeCurrentLayout->addWidget(entranceSlitLowerBladeCurrentLabel_, 0, 2, 1, 1);
    bladeCurrentLayout->addWidget(entranceSlitLowerBladeCurrentValue_, 0, 3, 1, 1);

    bladeCurrentLayout->addWidget(exitSlitUpperBladeCurrentALabel_, 1, 0, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitUpperBladeCurrentAValue_, 1, 1, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitLowerBladeCurrentALabel_, 1, 2, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitLowerBladeCurrentAValue_, 1, 3, 1, 1);

    bladeCurrentLayout->addWidget(exitSlitUpperBladeCurrentBLabel_, 2, 0, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitUpperBladeCurrentBValue_, 2, 1, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitLowerBladeCurrentBLabel_, 2, 2, 1, 1);
    bladeCurrentLayout->addWidget(exitSlitLowerBladeCurrentBValue_, 2, 3, 1, 1);

    bladeCurrentLayout->addWidget(teyBladeCurrentLabel_, 3, 0, 1, 1);
    bladeCurrentLayout->addWidget(teyBladeCurrentValue_, 3, 1, 1, 1);
    bladeCurrentLayout->addWidget(flyBladeCurrentLabel_, 3, 2, 1, 1);
    bladeCurrentLayout->addWidget(flyBladeCurrentValue_, 3, 3, 1, 1);

    bladeCurrentLayout->addWidget(i0EndstationBladeCurrentLabel_, 4, 0, 1, 1);
    bladeCurrentLayout->addWidget(i0EndstationBladeCurrentValue_, 4, 1, 1, 1);
    bladeCurrentLayout->addWidget(i0BeamlineBladeCurrentLabel_, 4, 2, 1, 1);
    bladeCurrentLayout->addWidget(i0BeamlineBladeCurrentValue_, 4, 3, 1, 1);

    bladeCurrentLayout->addWidget(photodiodeBladeCurrentLabel_, 5, 0, 1, 1);
    bladeCurrentLayout->addWidget(photodiodeBladeCurrentValue_, 5, 1, 1, 1);


    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitLowerBladeCurrentA"), SIGNAL(valueChanged(double)), this, SLOT(onExitSlitLowerBladeCurrentAChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitUpperBladeCurrentA"), SIGNAL(valueChanged(double)), this, SLOT(onExitSlitUpperBladeCurrentAChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitLowerBladeCurrentB"), SIGNAL(valueChanged(double)), this, SLOT(onExitSlitLowerBladeCurrentBChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitUpperBladeCurrentB"), SIGNAL(valueChanged(double)), this, SLOT(onExitSlitUpperBladeCurrentBChange(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("entranceSlitLowerBladeCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onEntranceSlitLowerBladeCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("entranceSlitUpperBladeCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onEntranceSlitUpperBladeCurrentChange(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("teyBladeCurrentControl"), SIGNAL(valueChanged(double)), this, SLOT(onTEYBladeCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("flyBladeCurrentControl"), SIGNAL(valueChanged(double)), this, SLOT(onFLYBladeCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("i0EndstationBladeCurrentControl"), SIGNAL(valueChanged(double)), this, SLOT(onI0EndstationBladeCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("i0BeamlineBladeCurrentControl"), SIGNAL(valueChanged(double)), this, SLOT(onI0BeamlineBladeCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("photodiodeBladeCurrentControl"), SIGNAL(valueChanged(double)), this, SLOT(onPhotodiodeBladeCurrentChange(double)));

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




