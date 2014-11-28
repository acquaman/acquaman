#include "WaterValveView.h"
#include <QLayout>
#include <QWidget>

WaterValveView::WaterValveView(WaterValve *beamline, QWidget *parent)
    :
QWidget(parent)
{

    model_ = beamline;

    waterLight_ = new QLabel;
    waterLight_->setPixmap(QIcon(":/RED.png").pixmap(12));

    connect( model_, SIGNAL(valveStatusChanged(bool)), this, SLOT(updatePixmap(bool)));

    QHBoxLayout *HLayout = new QHBoxLayout;
    HLayout->addWidget(waterLight_);
    setLayout(HLayout);

}


void WaterValveView::updatePixmap(bool value){

    //It seems that I can only access about 90% of each beamlines Water valve pv's so instead of making this ON/OFF it's a ON/UNKNOWN setting. Which as
   // of now isn't very helpful so the water valve view has been commented out.


    if (value == true){

        waterLight_->setPixmap(QIcon(":/ON.png").pixmap(12));

    }
    else if (value==false){
         waterLight_->setPixmap(QIcon(":/Yellow.png").pixmap(12));

    }

}
