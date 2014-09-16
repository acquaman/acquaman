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

    ///home/david/beamline/programming/acquaman/source/icons/
    psh1Light_->setPixmap(QIcon(":/RED.png").pixmap(12));
    psh2Light_->setPixmap(QIcon(":/RED.png").pixmap(12));
    ssh1Light_->setPixmap(QIcon(":/RED.png").pixmap(12));

    //Changed from beamline to model_
    connect(model_, SIGNAL(psh1Changed(int)), this, SLOT(updatePSH1Pixmap(int)));
    connect(model_, SIGNAL(psh2Changed(int)), this, SLOT(updatePSH2Pixmap(int)));
    connect(model_, SIGNAL(psh2Changed(int)), this, SLOT(updateSSH1Pixmap(int)));

    QString name = beamline->nameOfBeamline();
    name_ = new QLabel(name);
    QFont font;
    font.setPointSize(12);
    name_->setFont(font);

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
        psh1Light_->setPixmap(QIcon(":/ON.png").pixmap(12));
    else if (value == 2)
        psh1Light_->setPixmap(QIcon(":/Yellow.png").pixmap(12));
    else if (value == 4)
        psh1Light_->setPixmap(QIcon(":/RED.png").pixmap(12));
}

void BeamlineShutterView::updatePSH2Pixmap(int value){

    if( value == 1)
        psh2Light_->setPixmap(QIcon(":/ON.png").pixmap(12));
    else if (value == 2)
        psh2Light_->setPixmap(QIcon(":/Yellow.png").pixmap(12));
    else if (value == 4)
        psh2Light_->setPixmap(QIcon(":/RED.png").pixmap(12));
}

void BeamlineShutterView::updateSSH1Pixmap(int value){

    if( value == 1)
        ssh1Light_->setPixmap(QIcon(":/ON.png").pixmap(12));
    else if (value == 2)
        ssh1Light_->setPixmap(QIcon(":/Yellow.png").pixmap(12));
    else if (value == 4)
        ssh1Light_->setPixmap(QIcon(":/RED.png").pixmap(12));
}
