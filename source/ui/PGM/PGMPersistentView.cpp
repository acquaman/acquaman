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




    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
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

