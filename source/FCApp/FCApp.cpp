#include "FCApp.h"
#include "BeamlineShutters.h"
#include "BeamlineShutterView.h"
//#include "BeamPositionMonitor.h"
#include "BeamPositionMonitorView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QWidget>

FCApp::FCApp(QWidget *parent) :
    QWidget(parent)
{
    ///Storage Ring Current
    AMProcessVariable *storageRingPV = new AMProcessVariable( "PCT1402-01:mA:fbk",true,  this);

    /// Beam Position Monitor
    BeamPositionMonitorView *bpmView = new BeamPositionMonitorView(this);


    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    label_ = new QLabel("Ring Current: ");

    label_->setFont(font);


    QPalette Pal_(palette());
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::black);
    Pal.setColor(QPalette::WindowText, Qt::white);

    //label_->setAutoFillBackground(true);

    label_->setPalette(Pal);



    label_->setFont(font);
    currentValue_ = new QLCDNumber;

    currentValue_->setSegmentStyle(QLCDNumber::Flat);



    QPalette palette_(palette());

    palette_ = currentValue_->palette();

    currentValue_->setAutoFillBackground(true);
    palette_.setColor(palette_.Background, QColor(0, 0, 0, 255));
    palette_.setColor(palette_.WindowText, QColor(57, 87, 238, 255));

    palette_.setColor(palette_.Light, QColor(162, 183, 213));
    palette_.setColor(palette_.Dark, QColor(255, 255, 0));

    currentValue_->setPalette(palette_);



    currentValue_->setFixedSize(70, 50);
    connect(storageRingPV, SIGNAL(valueChanged(double)), currentValue_, SLOT(display(double)));


    ///Beamline Shutter PVs
    ///
    /*
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
*/


//////End Beamline shutter PVs
/*
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
*/

    QHBoxLayout *storageRingLayout = new QHBoxLayout;
    storageRingLayout->addWidget(label_);
    storageRingLayout->addWidget(currentValue_);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->addLayout(storageRingLayout);

/*

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
*/

    mainLayout->addWidget(bpmView);

/*
    hxmaView->setAutoFillBackground(true);
    hxmaView->setPalette(Pal);
    sxrmbView->setAutoFillBackground(true);
    sxrmbView->setPalette(Pal);

    vespersView->setAutoFillBackground(true);
    vespersView->setPalette(Pal);
    ideasView->setAutoFillBackground(true);
    ideasView->setPalette(Pal);


    smView->setAutoFillBackground(true);
    smView->setPalette(Pal);
    sgmpgmView->setAutoFillBackground(true);
    sgmpgmView->setPalette(Pal);



    bmitView->setAutoFillBackground(true);
    bmitView->setPalette(Pal);
    sylmandView->setAutoFillBackground(true);
    sylmandView->setPalette(Pal);

    cmcfidView->setAutoFillBackground(true);
    cmcfidView->setPalette(Pal);
    cmcfbmView->setAutoFillBackground(true);
    cmcfbmView->setPalette(Pal);


*/



    setLayout(mainLayout);



}
