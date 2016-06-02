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
