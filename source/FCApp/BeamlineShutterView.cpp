#include "BeamlineShutterView.h"

BeamlineShutterView::BeamlineShutterView(BeamlineShutters *beamline, QObject *parent)
    :
    QObject(parent)
{
    model_ = beamline;

    psh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));
    psh2Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));
    ssh1Light_->setPixmap(QIcon("/home/david/beamline/programming/acquaman/source/icons/RED.png").pixmap(25));


}

//These set the Pixmap icon based on input value
// 1 = Open
// 2 = Transition
// 4 = Closed
// I wonder if this should be a QIcon instead of void
void BeamlineShutterView::updatePSH1Pixmap(int value){


}

void BeamlineShutterView::updatePSH2Pixmap(int value){


}


void BeamlineShutterView::updateSSH1Pixmap(int value){



}
