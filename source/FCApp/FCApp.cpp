#include "FCApp.h"
#include "BeamlineShutters.h"
#include "BeamlineShutterView.h"

#include "WaterValve.h"
#include "WaterValveView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QWidget>


FCApp::FCApp(QWidget *parent) :
    QWidget(parent)
{
    ///Storage Ring Current
    AMProcessVariable *storageRingPV = new AMProcessVariable( "PCT1402-01:mA:fbk",true,  this);

    QFont font;
    font.setPointSize(12);

    QFont footerFont;
    footerFont.setPointSize(10);
    footerFont.setBold(true);

    label_ = new QLabel("Ring Current: ");
    footer_ = new QLabel("Powered by Acquaman");

    QPalette labelPal;
    labelPal.setColor(QPalette::WindowText, Qt::magenta);
    label_->setPalette(labelPal);

    QPalette footPal;
    footPal.setColor(QPalette::WindowText, Qt::cyan);
    footer_->setPalette(footPal);


    label_->setFont(font);
    footer_->setFont(footerFont);
    currentValue_ = new QLCDNumber;
    currentValue_->setPalette(labelPal);
    currentValue_->setSegmentStyle(QLCDNumber::Flat);
    currentValue_->setFixedSize(70, 50);
    connect(storageRingPV, SIGNAL(valueChanged(double)), currentValue_, SLOT(display(double)));

    ///Beamline Water Pvs

    QList<QString> bmitWater;
    bmitWater << "SWF1605-1-B10-01"
                          <<  "SWF1605-1-B10-02"
                          <<  "SWF1605-1-B10-03"
                           << "SWF1605-1-B10-05"
                          <<  "SWF1605-1-B10-08";

    WaterValve *bmitWaterModel = new WaterValve("BMIT", bmitWater, this);


    QList<QString> sylmandWater;
    bmitWater << "SWF1605-5-B20-01"
                          <<  "SWF1605-5-B20-02"
                          <<  "SWF1605-5-B20-03"
                           << "SWF1605-5-B20-04"
                           << "SWF1605-5-B20-05"
                           << "SWF1605-5-B20-06"
                           <<  "SWF1605-5-B20-07";

    WaterValve *sylmandWaterModel = new WaterValve("SyLMAND", sylmandWater, this);


    QList<QString> hxmaWater;
    hxmaWater << "SWF1606-1-I10-01"
                          <<  "SWF1606-1-I10-02"
                          <<  "SWF1606-1-I10-03"
                           << "SWF1606-1-I10-04"
                          <<  "SWF1606-1-I10-05"
                           << "SWF1606-1-I10-06"
                           << "SWF1606-1-I10-07"
                           << "SWF1606-1-I10-08"
                           << "SWF1606-1-I10-09"
                           << "SWF1606-1-I10-10"
                           << "SWF1606-1-I10-11"
                           << "SWF1606-1-I10-12"
                           << "SWF1606-1-I10-13"
                           << "SWF1606-1-I10-14"
                           << "SWF1606-1-I10-15"
                           << "SWF1606-1-I10-16"
                           << "SWF1606-1-I10-17"
                           << "SWF1606-1-I10-18"
                           << "SWF1606-1-I10-19"
                           << "SWF1606-1-I10-20"
                           << "SWF1606-1-I10-21"
                           << "SWF1606-1-I10-22"
                           << "SWF1606-1-I10-23"
                           << "SWF1606-1-I10-24"
                           << "SWF1606-1-I10-25"
                           << "SWF1606-1-I10-26";

 WaterValve *hxmaWaterModel = new WaterValve("HXMA", hxmaWater, this);



    QList<QString> sxrmbWater;
    sxrmbWater  << "SWF1606-4-B10-01"
                <<  "SWF1606-4-B10-02"
                <<  "SWF1606-4-B10-03"
                 << "SWF1606-4-B10-04"
                <<  "SWF1606-4-B10-05"
                 << "SWF1606-4-B10-06"
                 << "SWF1606-4-B10-07"
                 << "SWF1606-4-B10-08"
                 << "SWF1606-4-B10-09"
                 << "SWF1606-4-B10-10";

    WaterValve *sxrmbWaterModel = new WaterValve("SXRMB", sxrmbWater, this);



    QList<QString> vespersWater;
    vespersWater << "SWF1607-1-B20-02"
                          <<  "SWF1607-1-B20-03"
                          <<  "SWF1607-1-B22-01"
                           << "SWF1607-1-B21-01"
                          <<  "SWF1607-1-B20-04"
                          <<  "SWF1607-1-B20-05"
                          <<  "SWF1607-1-B21-02"
                          <<  "SWF1607-1-B21-03"
                          <<  "SWF1607-1-B22-02"
                          <<  "SWF1607-1-B21-04"
                          <<  "SWF1607-2-B21-01";


    WaterValve *vespersWaterModel = new WaterValve("Vespers", vespersWater, this);


    QList<QString> cmcfWater;
    cmcfWater << "SWF1608-2-I10-01"
                          <<  "SWF1608-2-I10-03";


    WaterValve *cmcfWaterModel = new WaterValve("CMCF", cmcfWater, this);



    QList<QString> ideasWater;
    ideasWater << "SWF1608-9-B20-01"
                           <<  "SWF1608-9-B20-02"
                           <<  "SWF1608-9-B20-03";

    WaterValve *ideasWaterModel = new WaterValve("IDEAS", ideasWater, this);

    QList<QString> smWater;
    smWater << "SWF1610-1-I00-03"
                       <<  "SWF1610-1-I00-04"
                       <<  "SWF1610-1-I10-01"
                       <<  "SWF1610-1-I10-02"
                       <<  "SWF1610-1-I10-03"
                       <<  "SWF1610-1-I10-04"
                       <<  "SWF1610-1-I10-05"
                       <<  "SWF1610-1-I10-06"
                       <<  "SWF1610-1-I10-08";

    WaterValve *smWaterModel = new WaterValve("SM", smWater, this);

    QList<QString> sgmpgmWater;
    sgmpgmWater << "SWF1611-3-I00-02"
                          <<  "SWF1611-3-I10-01"
                          <<  "SWF1611-3-I10-02"
                          <<  "SWF1611-3-I10-03"
                          <<  "SWF1611-3-I10-04'"
                           <<  "SWF1611-3-I10-05"
                           <<  "SWF1611-3-I10-06"
                            <<  "SWF1611-3-I10-07"
                            <<  "SWF1611-3-I10-08"
                            <<  "SWF1611-3-I10-09"
                            <<  "SWF1611-3-I10-10"
                            <<  "SWF1611-3-I20-01"
                            <<  "SWF1611-3-I20-02"
                            <<  "SWF1611-3-I20-03"
                            <<  "SWF1611-3-I20-04"
                            <<  "SWF1611-3-I20-05"
                            <<  "SWF1611-3-I20-06"
                           <<  "SWF1611-3-I20-07"
                           <<  "SWF1611-3-I20-08";


    WaterValve *sgmpgmWaterModel = new WaterValve("SGM/PGM", sgmpgmWater, this);


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

    BeamlineShutters *smShutters = new BeamlineShutters("SM",
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


    WaterValveView *bmitWaterView = new WaterValveView(bmitWaterModel);
    WaterValveView *sylmandWaterView = new WaterValveView(sylmandWaterModel);
    WaterValveView *hxmaWaterView = new WaterValveView(hxmaWaterModel);
    WaterValveView *sxrmbWaterView = new WaterValveView(sxrmbWaterModel);
   WaterValveView *vespersWaterView = new WaterValveView(vespersWaterModel);
   WaterValveView *cmcfidWaterView = new WaterValveView(cmcfWaterModel);
   WaterValveView *cmcfbmWaterView = new WaterValveView(cmcfWaterModel);
   WaterValveView *ideasWaterView = new WaterValveView(ideasWaterModel);
   WaterValveView *smWaterView = new WaterValveView(smWaterModel);
   WaterValveView *sgmpgmWaterView = new WaterValveView(sgmpgmWaterModel);


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



    QVBoxLayout *mainLayout = new QVBoxLayout;


    QHBoxLayout *storageRingLayout = new QHBoxLayout;
    storageRingLayout->addWidget(label_);
    storageRingLayout->addWidget(currentValue_);

    mainLayout->addLayout(storageRingLayout);





    QHBoxLayout *bmit = new QHBoxLayout;
    bmit->addWidget(bmitView);
   // bmit->addWidget(bmitWaterView);
    bmit->setAlignment(Qt::AlignTop);
    mainLayout->addLayout(bmit);

    QHBoxLayout *sylmand = new QHBoxLayout;
    sylmand->addWidget(sylmandView);
   // sylmand->addWidget(sylmandWaterView);
    mainLayout->addLayout(sylmand);

    QHBoxLayout *hxma = new QHBoxLayout;
    hxma->addWidget(hxmaView);
    //hxma->addWidget(hxmaWaterView);
    mainLayout->addLayout(hxma);

    QHBoxLayout *sxrmb = new QHBoxLayout;
    sxrmb->addWidget(sxrmbView);
    //sxrmb->addWidget(sxrmbWaterView);
    mainLayout->addLayout(sxrmb);

    QHBoxLayout *vespers = new QHBoxLayout;
    vespers->addWidget(vespersView);
    //vespers->addWidget(vespersWaterView);
    mainLayout->addLayout(vespers);

    QHBoxLayout *cmcfid = new QHBoxLayout;
    cmcfid->addWidget(cmcfidView);
    //cmcfid->addWidget(cmcfidWaterView);
    mainLayout->addLayout(cmcfid);

    QHBoxLayout *cmcfbm = new QHBoxLayout;
    cmcfbm->addWidget(cmcfbmView);
    //cmcfbm->addWidget(cmcfbmWaterView);
    mainLayout->addLayout(cmcfbm);

    QHBoxLayout *ideas = new QHBoxLayout;
    ideas->addWidget(ideasView);
    //ideas->addWidget(ideasWaterView);
    mainLayout->addLayout(ideas);

    QHBoxLayout *sm = new QHBoxLayout;
    sm->addWidget(smView);
    //sm->addWidget(smWaterView);
    mainLayout->addLayout(sm);

    QHBoxLayout *sgmpgm = new QHBoxLayout;
    sgmpgm->addWidget(sgmpgmView);
    //sgmpgm->addWidget(sgmpgmWaterView);
    mainLayout->addLayout(sgmpgm);




    mainLayout->setAlignment(Qt::AlignLeft);

    footer_->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(footer_);

    setLayout(mainLayout);




}
