#include "PGMPersistentView.h"
#include "beamline/PGM/PGMBeamline.h"
#include "ui/PGM/PGMBeamStatusView.h"

#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)

{
    // The average values for each bpm.
    avg10IDx_ = 665;
    avg10IDy_ = -245;
    avg11ID1x_ = -400;
    avg11ID1y_ = -970;
    avg11ID2x_ = -505;
    avg11ID2y_ = -245;
    // Allowed variance in each bpm value.
    bpmVariance_ = 50;


    beamStatusView_ = new PGMBeamStatusView();


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

    QGridLayout *bpmLayout = new QGridLayout();

    bpmLayout->addWidget(bpm10IDLabel_,   0, 1, 1, 1);
    bpmLayout->addWidget(bpm11ID1Label_,  0, 2, 1, 1);
    bpmLayout->addWidget(bpm11ID2Label_,  0, 3, 1, 1);
    bpmLayout->addWidget(bpmXValueLabel_, 1, 0, 1, 1);
    bpmLayout->addWidget(bpmYValueLabel_, 2, 0, 1, 1);
    bpmLayout->addWidget(bpm10IDvalueX_,  1, 1, 1, 1);
    bpmLayout->addWidget(bpm10IDvalueY_,  2, 1, 1, 1);
    bpmLayout->addWidget(bpm11ID1valueX_, 1, 2, 1, 1);
    bpmLayout->addWidget(bpm11ID1valueY_, 2, 2, 1, 1);
    bpmLayout->addWidget(bpm11ID2valueX_, 1, 3, 1, 1);
    bpmLayout->addWidget(bpm11ID2valueY_, 2, 3, 1, 1);

    bpmLayout->setColumnMinimumWidth(1, 100);
    bpmLayout->setColumnMinimumWidth(2, 100);
    bpmLayout->setColumnMinimumWidth(3, 100);

    QGroupBox *bpmBox = new QGroupBox("Beam Position Monitor");
    bpmBox->setLayout(bpmLayout);

    connect(PGMBeamline::pgm()->exposedControlByName("BPM 10ID Downstream-X"), SIGNAL(valueChanged(double)), this, SLOT(onBPM10IDValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("BPM 10ID Downstream-Y"), SIGNAL(valueChanged(double)), this, SLOT(onBPM10IDValueYChanged(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("BPM 11ID #1-X"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID1ValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("BPM 11ID #1-Y"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID1ValueYChanged(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("BPM 11ID #2-X"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID2ValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("BPM 11ID #2-Y"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID2ValueYChanged(double)));


   // Main layout
   QVBoxLayout *mainPanelLayout = new QVBoxLayout;
   mainPanelLayout->addWidget(beamStatusView_);
   mainPanelLayout->addWidget(bpmBox);
   mainPanelLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

   // Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
   // the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.
   QGroupBox *persistentPanel = new QGroupBox("VLS-PGM Beamline");
   persistentPanel->setLayout(mainPanelLayout);

   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addWidget(persistentPanel);

   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
   setMaximumHeight(1000);
   setMinimumHeight(800);
   setMaximumWidth(400);
   setMinimumWidth(400);

   setLayout(mainLayout);


}

void PGMPersistentView::onBPM10IDValueXChanged(double value){

    if( value > (avg10IDx_ + bpmVariance_) || value < (avg10IDx_ - bpmVariance_) ){
        bpm10IDvalueX_->setStyleSheet( "color : red"  );
        bpm10IDvalueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm10IDvalueX_->setStyleSheet( "color : black"  );
        bpm10IDvalueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }


}

void PGMPersistentView::onBPM10IDValueYChanged(double value){

    if( value > (avg10IDy_ + bpmVariance_) || value < (avg10IDy_ - bpmVariance_) ){
        bpm10IDvalueY_->setStyleSheet( "color : red"  );
        bpm10IDvalueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm10IDvalueY_->setStyleSheet( "color : black"  );
        bpm10IDvalueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }
}

void PGMPersistentView::onBPM11ID1ValueXChanged(double value){

    if( value > (avg11ID1x_ + bpmVariance_) || value < (avg11ID1x_ - bpmVariance_) ){
        bpm11ID1valueX_->setStyleSheet( "color : red"  );
        bpm11ID1valueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm11ID1valueX_->setStyleSheet( "color : black"  );
        bpm11ID1valueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }

}

void PGMPersistentView::onBPM11ID1ValueYChanged(double value){

    if( value > (avg11ID1y_ + bpmVariance_) || value < (avg11ID1y_ - bpmVariance_) ){
        bpm11ID1valueY_->setStyleSheet( "color : red"  );
        bpm11ID1valueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm11ID1valueY_->setStyleSheet( "color : black"  );
        bpm11ID1valueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }

}

void PGMPersistentView::onBPM11ID2ValueXChanged(double value){

    if( value > avg11ID2x_ + bpmVariance_ || value < avg11ID2x_ - bpmVariance_ ){
        bpm11ID2valueX_->setStyleSheet( "color : red"  );
        bpm11ID2valueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm11ID2valueX_->setStyleSheet( "color : black"  );
        bpm11ID2valueX_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }

}

void PGMPersistentView::onBPM11ID2ValueYChanged(double value){

    if( value > (avg11ID2y_ + bpmVariance_) || value < (avg11ID2y_ - bpmVariance_) ){
        bpm11ID2valueY_->setStyleSheet( "color : red"  );
        bpm11ID2valueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    } else {
        bpm11ID2valueY_->setStyleSheet( "color : black"  );
        bpm11ID2valueY_->setText(QString("%1 um").arg(value, 0, 'f', 0));
    }


}

