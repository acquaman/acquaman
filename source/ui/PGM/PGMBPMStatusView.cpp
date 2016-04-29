#include "PGMBPMStatusView.h"
#include "beamline/PGM/PGMBeamline.h"
#include "beamline/PGM/PGMBPMControl.h"

#include <QLabel>
#include <QVBoxLayout>

PGMBPMStatusView::PGMBPMStatusView(QWidget *parent) :
    QWidget(parent)
{

    // Set widgets and layout for PGMBpmControls
    setupBPMDisplay();

    // Set connections for PGMBpmContols
    setupBPMConnections();

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(bpmLayout_);

    setLayout(mainLayout);


}

void PGMBPMStatusView::setupBPMDisplay(){

    bpm10IDvalueX_ = new QLabel("   um");
    bpm10IDvalueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm10IDvalueY_ = new QLabel("   um");
    bpm10IDvalueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID1valueX_ = new QLabel("   um");
    bpm11ID1valueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID1valueY_ = new QLabel("   um");
    bpm11ID1valueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID2valueX_ = new QLabel("   um");
    bpm11ID2valueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID2valueY_ = new QLabel("   um");
    bpm11ID2valueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm10IDLabel_ = new QLabel("10ID DnStr");
    bpm10IDLabel_->setAlignment(Qt::AlignLeft);
    bpm11ID1Label_ = new QLabel("11ID #1");
    bpm11ID1Label_->setAlignment(Qt::AlignLeft);
    bpm11ID2Label_ = new QLabel("11ID #2");
    bpm11ID2Label_->setAlignment(Qt::AlignLeft);

    bpmXValueLabel_ = new QLabel("X: ");
    bpmXValueLabel_->setAlignment(Qt::AlignCenter);
    bpmYValueLabel_ = new QLabel("Y: ");
    bpmYValueLabel_->setAlignment(Qt::AlignCenter);

    bpmLayout_ = new QGridLayout();

    bpmLayout_->addWidget(bpm10IDLabel_,   0, 1, 1, 1);
    bpmLayout_->addWidget(bpm11ID1Label_,  0, 2, 1, 1);
    bpmLayout_->addWidget(bpm11ID2Label_,  0, 3, 1, 1);
    bpmLayout_->addWidget(bpmXValueLabel_, 1, 0, 1, 1);
    bpmLayout_->addWidget(bpmYValueLabel_, 2, 0, 1, 1);
    bpmLayout_->addWidget(bpm10IDvalueX_,  1, 1, 1, 1);
    bpmLayout_->addWidget(bpm10IDvalueY_,  2, 1, 1, 1);
    bpmLayout_->addWidget(bpm11ID1valueX_, 1, 2, 1, 1);
    bpmLayout_->addWidget(bpm11ID1valueY_, 2, 2, 1, 1);
    bpmLayout_->addWidget(bpm11ID2valueX_, 1, 3, 1, 1);
    bpmLayout_->addWidget(bpm11ID2valueY_, 2, 3, 1, 1);

    bpmLayout_->setColumnMinimumWidth(1, 100);
    bpmLayout_->setColumnMinimumWidth(2, 100);
    bpmLayout_->setColumnMinimumWidth(3, 100);

}

void PGMBPMStatusView::setupBPMConnections(){
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm10IDxControl(), SIGNAL(onBPMValueChanged(QString)), bpm10IDvalueX_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm10IDxControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM10IDxStatusChanged(bool)));
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm10IDyControl(), SIGNAL(onBPMValueChanged(QString)), bpm10IDvalueY_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm10IDyControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM10IDyStatusChanged(bool)));
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm11ID1xControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID1valueX_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm11ID1xControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM11ID1xStatusChanged(bool)));
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm11ID1yControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID1valueY_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm11ID1yControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM11ID1yStatusChanged(bool)));
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm11ID2xControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID2valueX_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm11ID2xControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM11ID2xStatusChanged(bool)));
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm11ID2yControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID2valueY_, SLOT(setText(QString)));
    connect(PGMBeamline::pgm()->bpm11ID2yControl(), SIGNAL(stateValidityChanged(bool)), this, SLOT(onBPM11ID2yStatusChanged(bool)));
}


void PGMBPMStatusView::onBPM10IDxStatusChanged(bool state){

    if(state){
         bpm10IDvalueX_->setStyleSheet( "color : black"  );
    } else {
         bpm10IDvalueX_->setStyleSheet( "color : red"  );
    }
}

void PGMBPMStatusView::onBPM10IDyStatusChanged(bool state){

    if(state){
         bpm10IDvalueY_->setStyleSheet( "color : black"  );
    } else {
         bpm10IDvalueY_->setStyleSheet( "color : red"  );
    }
}


void PGMBPMStatusView::onBPM11ID1xStatusChanged(bool state){

    if(state){
         bpm11ID1valueX_->setStyleSheet( "color : black"  );
    } else {
         bpm11ID1valueX_->setStyleSheet( "color : red"  );
    }
}

void PGMBPMStatusView::onBPM11ID1yStatusChanged(bool state){

    if(state){
         bpm11ID1valueY_->setStyleSheet( "color : black"  );
    } else {
         bpm11ID1valueY_->setStyleSheet( "color : red"  );
    }
}

void PGMBPMStatusView::onBPM11ID2xStatusChanged(bool state){

    if(state){
         bpm11ID2valueX_->setStyleSheet( "color : black"  );
    } else {
         bpm11ID2valueX_->setStyleSheet( "color : red"  );
    }
}

void PGMBPMStatusView::onBPM11ID2yStatusChanged(bool state){

    if(state){
         bpm11ID2valueY_->setStyleSheet( "color : black"  );
    } else {
         bpm11ID2valueY_->setStyleSheet( "color : red"  );
    }

}
