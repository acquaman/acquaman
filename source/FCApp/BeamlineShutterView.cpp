#include "BeamlineShutterView.h"

BeamlineShutterView::BeamlineShutterView(BeamlineShutters *beamline, QObject *parent)
    :
    QObject(parent)
{
    model_ = beamline;

    psh1Light_ = new QLabel;
    psh2Light_ = new QLabel;
    ssh1Light_ = new QLabel;

    psh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));
    psh2Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));
    ssh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));


    connect(beamline, SIGNAL(psh1Changed(int)), this, SLOT(updatePSH1Pixmap(int)));
    connect(beamline, SIGNAL(psh2Changed(int)), this, SLOT(updatePSH2Pixmap(int)));
    connect(beamline, SIGNAL(psh2Changed(int)), this, SLOT(updateSSH1Pixmap(int)));




}

//These set the Pixmap icon based on input value
// 1 = Open
// 2 = Transition
// 4 = Closed
// I wonder if this should be a QIcon instead of void
void BeamlineShutterView::updatePSH1Pixmap(int value){

     if( value == 1)
        psh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/ON.png").pixmap(25));

    else if (value == 2)
        psh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/Yellow.png").pixmap(25));

    else if (value == 4)
        psh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));

}

void BeamlineShutterView::updatePSH2Pixmap(int value){

    if( value == 1)
       psh2Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/ON.png").pixmap(25));

   else if (value == 2)
       psh2Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/Yellow.png").pixmap(25));

   else if (value == 4)
      psh2Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));


}

void BeamlineShutterView::updateSSH1Pixmap(int value){

    if( value == 1)
       ssh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/ON.png").pixmap(25));

   else if (value == 2)
       ssh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/Yellow.png").pixmap(25));

   else if (value == 4)
      ssh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));

}
