#include "BeamlineShutterView.h"
#include <QLayout>
#include <QWidget>

BeamlineShutterView::BeamlineShutterView(BeamlineShutters *beamline, QWidget *parent)
    :
    QWidget(parent)
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

    QString name = model_->nameOfBeamline_;
    name_ = new QLabel(name);

    QHBoxLayout *HLayout = new QHBoxLayout;
    HLayout->addWidget(name_, 1, Qt::AlignLeft);
    HLayout->addWidget(psh1Light_);
    HLayout->addWidget(psh2Light_);
    HLayout->addWidget(ssh1Light_);
    setLayout(HLayout);
}

//These set the Pixmap icon based on input value
// 1 = Open
// 2 = Transition
// 4 = Closed
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
