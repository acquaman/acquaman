#include "PGMPersistentView.h"

#include "beamline/PGM/PGMBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{

//    exitSlitGapBranchAControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitGapBranchA());
//    exitSlitGapBranchBControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitSlitGapBranchB());
//    exitPositionBranchAControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitPositionBranchA());
//    exitPositionBranchBControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->exitPositionBranchB());

//    QVBoxLayout *exitSlitLayout = new QVBoxLayout;
//    exitSlitLayout->addWidget(exitSlitGapBranchAControlEditor_);
//    exitSlitLayout->addWidget(exitSlitGapBranchBControlEditor_);

//    QVBoxLayout *exitPositionLayout = new QVBoxLayout;
//    exitPositionLayout->addWidget(exitPositionBranchAControlEditor_);
//    exitPositionLayout->addWidget(exitPositionBranchBControlEditor_);

    upperBladeCurrentALabel_ = new QLabel("Branch A Upper: ");
    upperBladeCurrentA_ = new QLabel("  A");

    lowerBladeCurrentALabel_ = new QLabel("Branch A Lower: ");
    lowerBladeCurrentA_ = new QLabel("  A");

    upperBladeCurrentBLabel_ = new QLabel("Branch B Upper: ");
    upperBladeCurrentB_ = new QLabel("  A");

    lowerBladeCurrentBLabel_ = new QLabel("Branch B Lower: ");
    lowerBladeCurrentB_ = new QLabel("  A");

    QHBoxLayout *upperBladeALayout = new QHBoxLayout;
    upperBladeALayout->addWidget(upperBladeCurrentALabel_);
    upperBladeALayout->addWidget(upperBladeCurrentA_);
    QHBoxLayout *lowerBladeALayout = new QHBoxLayout;
    lowerBladeALayout->addWidget(lowerBladeCurrentALabel_);
    lowerBladeALayout->addWidget(lowerBladeCurrentA_);
    QHBoxLayout *upperBladeBLayout = new QHBoxLayout;
    upperBladeBLayout->addWidget(upperBladeCurrentBLabel_);
    upperBladeBLayout->addWidget(upperBladeCurrentB_);
    QHBoxLayout *lowerBladeBLayout = new QHBoxLayout;
    lowerBladeBLayout->addWidget(lowerBladeCurrentBLabel_);
    lowerBladeBLayout->addWidget(lowerBladeCurrentB_);

    QVBoxLayout *bladeALayout = new QVBoxLayout;
    bladeALayout->addLayout(upperBladeALayout);
    bladeALayout->addLayout(lowerBladeALayout);
    QVBoxLayout *bladeBLayout = new QVBoxLayout;
    bladeBLayout->addLayout(upperBladeBLayout);
    bladeBLayout->addLayout(lowerBladeBLayout);

    QHBoxLayout *mainBladeLayout = new QHBoxLayout;
    mainBladeLayout->addLayout(bladeALayout);
    mainBladeLayout->addLayout(bladeBLayout);

    QGroupBox *bladeInfoGroupBox_ = new QGroupBox("Exit Blade Current");
    bladeInfoGroupBox_->setLayout(mainBladeLayout);


    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitLowerBladeCurrentA"), SIGNAL(valueChanged(double)), this, SLOT(onUpperBladeCurrentAChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitUpperBladeCurrentA"), SIGNAL(valueChanged(double)), this, SLOT(onLowerBladeCurrentAChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitLowerBladeCurrentB"), SIGNAL(valueChanged(double)), this, SLOT(onUpperBladeCurrentBChanged(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("exitSlitUpperBladeCurrentB"), SIGNAL(valueChanged(double)), this, SLOT(onLowerBladeCurrentBChanged(double)));



    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addWidget(bladeInfoGroupBox_);
    //mainPanelLayout->addLayout(exitSlitLayout);
    //mainPanelLayout->addLayout(exitPositionLayout);


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


void PGMPersistentView::onUpperBladeCurrentAChanged(double value){
    upperBladeCurrentA_->setText(QString("%1 A").arg(value));
}

void PGMPersistentView::onLowerBladeCurrentAChanged(double value){
    lowerBladeCurrentA_->setText(QString("%1 A").arg(value));
}

void PGMPersistentView::onUpperBladeCurrentBChanged(double value){
    upperBladeCurrentB_->setText(QString("%1 A").arg(value));
}

void PGMPersistentView::onLowerBladeCurrentBChanged(double value){
    lowerBladeCurrentB_->setText(QString("%1 A").arg(value));
}

