#include "FCApp.h"
#include "BeamlineShutters.h"
#include "BeamlineShutterView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QWidget>


FCApp::FCApp(QWidget *parent) :
    QWidget(parent)
{

    AMProcessVariable *StorageRingPV = new AMProcessVariable( "PCT1402-01:mA:fbk",true,  this);
    label_ = new QLabel("Ring Current: ");
    currentValue_ = new QLCDNumber;
    currentValue_->setSegmentStyle(QLCDNumber::Filled);
    currentValue_->setFixedSize(150, 100);
    connect(StorageRingPV, SIGNAL(valueChanged(double)), currentValue_, SLOT(display(double)));


    ///Beamline Shutter PVs
    BeamlineShutters *bmitShutters = new BeamlineShutters("BMIT",
                                                             "PSH1405-B10-01:state",
                                                             "PSH1405-B10-02:state",
                                                             "SSH1405-B10-01:state",
                                                             this);

    BeamlineShutters *sylmandShutters = new BeamlineShutters("SyLMAND",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);

    BeamlineShutters *hxmaShutters = new BeamlineShutters("HXMA",
                                                             "PSH1406-I00-01:state",
                                                             "PSH1406-I00-02:state",
                                                             "SSH1406-I00-01:state",
                                                             this);

    BeamlineShutters *sxrmbShutters = new BeamlineShutters("SXRMB",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);


    BeamlineShutters *vespersShutters = new BeamlineShutters("VESPERS",
                                                             "PSH1408-B20-01:state",
                                                             "PSH1408-B20-02:state",
                                                             "SSH1408-B20-01:state",
                                                             this);

    BeamlineShutters *cmcfidShutters = new BeamlineShutters("CMCF-ID",
                                                             "PSH1408-I00-01:state",
                                                             "PSH1408-I00-02:state",
                                                             "SSH1408-I00-01:state",
                                                             this);

    BeamlineShutters *cmcfbmShutters = new BeamlineShutters("CMCF-BM",
                                                             "PSH1408-B10-01:state",
                                                             "PSH1408-B10-02:state",
                                                             "SSH1408-B10-01:state",
                                                             this);

    BeamlineShutters *ideasShutters = new BeamlineShutters("IDEAS",
                                                             "PSH1409-B20-01:state",
                                                             "PSH1409-B20-02:state",
                                                             "SSH1409-B20-01:state",
                                                             this);

    BeamlineShutters *smShutters = new BeamlineShutters("SXRMB",
                                                             "PSH1410-I00-01:state",
                                                             "PSH1410-I00-02:state",
                                                             "SSH1410-I00-01:state",
                                                             this);

    BeamlineShutters *sgmpgmShutters = new BeamlineShutters("PGM/SGM",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);


//////End Beamline shutter PVs

    BeamlineShutterView *bmitView = new BeamlineShutterView(bmitShutters);
    BeamlineShutterView *sylmandView = new BeamlineShutterView(sylmandShutters);
    BeamlineShutterView *hxmaView = new BeamlineShutterView(hxmaShutters);
    BeamlineShutterView *sxrmbView = new BeamlineShutterView(sxrmbShutters);
    BeamlineShutterView *vespersView = new BeamlineShutterView(vespersShutters);
    BeamlineShutterView *cmcfidView = new BeamlineShutterView(cmcfidShutters);
    BeamlineShutterView *cmcfbmView = new BeamlineShutterView(cmcfbmShutters);
    BeamlineShutterView *ideasView = new BeamlineShutterView(ideasShutters);
    BeamlineShutterView *smView = new BeamlineShutterView(smShutters);
    BeamlineShutterView *sgmpgmView = new BeamlineShutterView(sgmpgmShutters);


    QHBoxLayout *storageRingLayout = new QHBoxLayout;
    storageRingLayout->addWidget(label_);
    storageRingLayout->addWidget(currentValue_);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(storageRingLayout);
   // mainLayout->addLayout(shutterTypeLayout);
    mainLayout->addWidget(bmitView);
    mainLayout->addWidget(sylmandView);
    mainLayout->addWidget(hxmaView);
    mainLayout->addWidget(sxrmbView);
    mainLayout->addWidget(vespersView);
    mainLayout->addWidget(cmcfidView);
    mainLayout->addWidget(cmcfbmView);
    mainLayout->addWidget(ideasView);
    mainLayout->addWidget(smView);
    mainLayout->addWidget(sgmpgmView);
    setLayout(mainLayout);



}
