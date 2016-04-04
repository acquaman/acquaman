#include "PGMPersistentView.h"
#include "beamline/PGM/PGMBeamline.h"

#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)

{
    bpm10IDvalueX_ = new QLabel("     um");
    bpm10IDvalueX_->setAlignment(Qt::AlignCenter);
    bpm10IDvalueX_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm10IDvalueY_ = new QLabel("     um");
    bpm10IDvalueY_->setAlignment(Qt::AlignCenter);
    bpm10IDvalueY_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm11ID1valueX_ = new QLabel("     um");
    bpm11ID1valueX_->setAlignment(Qt::AlignCenter);
    bpm11ID1valueX_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm11ID1valueY_ = new QLabel("     um");
    bpm11ID1valueY_->setAlignment(Qt::AlignCenter);
    bpm11ID1valueY_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm11ID2valueX_ = new QLabel("     um");
    bpm11ID2valueX_->setAlignment(Qt::AlignCenter);
    bpm11ID2valueX_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm11ID2valueY_ = new QLabel("     um");
    bpm11ID2valueY_->setAlignment(Qt::AlignCenter);
    bpm11ID2valueY_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    bpm10IDLabel_ = new QLabel("10ID DnStr");
    bpm11ID1Label_ = new QLabel("11ID #1");
    bpm11ID2Label_ = new QLabel("11ID #2");

    bpmXValueLabel_ = new QLabel("X");
    bpmYValueLabel_ = new QLabel("Y");

    QGridLayout *bpmLayout = new QGridLayout();
    bpmLayout->addWidget(bpm10IDLabel_,   0, 1);
    bpmLayout->addWidget(bpm11ID1Label_,  0, 2);
    bpmLayout->addWidget(bpm11ID2Label_,  0, 3);
    bpmLayout->addWidget(bpmXValueLabel_, 1, 0);
    bpmLayout->addWidget(bpmYValueLabel_, 2, 0);
    bpmLayout->addWidget(bpm10IDvalueX_,  1, 1);
    bpmLayout->addWidget(bpm10IDvalueY_,  2, 1);
    bpmLayout->addWidget(bpm11ID1valueX_, 1, 2);
    bpmLayout->addWidget(bpm11ID1valueY_, 2, 2);
    bpmLayout->addWidget(bpm11ID2valueX_, 1, 2);
    bpmLayout->addWidget(bpm11ID2valueY_, 2, 2);


    connect(PGMBeamline::pgm()->exposedControlByName("bpm10IDX_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM10IDValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("bpm10IDY_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM10IDValueYChanged(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("bpm11ID1X_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11IDIValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("bpm11ID1Y_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11IDIValueYChanged(double)));

    connect(PGMBeamline::pgm()->exposedControlByName("bpm11ID2X_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID2ValueXChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("bpm11ID2Y_"), SIGNAL(valueChanged(double)), this, SLOT(onBPM11ID2ValueYChanged(double)));

   // Main layout
   QVBoxLayout *mainPanelLayout = new QVBoxLayout;
   mainPanelLayout->addLayout(bpmLayout);


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

void PGMPersistentView::onBPM10IDValueXChanged(double current){
    bpm10IDvalueX_->setText(QString("1% um").arg(current));


}

void PGMPersistentView::onBPM10IDValueYChanged(double current){
    bpm10IDvalueY_->setText(QString("1% um").arg(current));
}

void PGMPersistentView::onBPM11ID1ValueXChanged(double current){
    bpm11ID1valueX_->setText(QString("1% um").arg(current));
}

void PGMPersistentView::onBPM11ID1ValueYChanged(double current){
    bpm11ID1valueY_->setText(QString("1% um").arg(current));
}

void PGMPersistentView::onBPM11ID2ValueXChanged(double current){
    bpm11ID2valueX_->setText(QString("1% um").arg(current));
}

void PGMPersistentView::onBPM11ID2ValueYChanged(double current){
    bpm11ID2valueY_->setText(QString("1% um").arg(current));
}
