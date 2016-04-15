#include "PGMBeamStatusView.h"
#include "beamline/PGM/PGMBeamline.h"

#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>


PGMBeamStatusView::PGMBeamStatusView(QWidget *parent) :
    QWidget(parent)
{
    ringCurrentLabel_ = new QLabel("Ring Current: ");
    ringCurrentLabel_->setAlignment(Qt::AlignCenter);
    ringCurrentValue_ = new QLabel("   mA");
    ringCurrentValue_->setAlignment(Qt::AlignCenter);
    ringCurrentValue_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    QVBoxLayout *ringCurrentLayout = new QVBoxLayout();
    ringCurrentLayout->addWidget(ringCurrentLabel_);
    ringCurrentLayout->addWidget(ringCurrentValue_);

    beamLifetimeLabel_ = new QLabel("Beam Lifetime");
    beamLifetimeLabel_->setAlignment(Qt::AlignCenter);
    beamLifetimeValue_ = new QLabel("   Hrs");
    beamLifetimeValue_->setAlignment(Qt::AlignCenter);
    beamLifetimeValue_->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    QVBoxLayout *beamlifeLayout = new QVBoxLayout();
    beamlifeLayout->addWidget(beamLifetimeLabel_);
    beamlifeLayout->addWidget(beamLifetimeValue_);

    QHBoxLayout *storageRingLayout = new QHBoxLayout;
    storageRingLayout->addLayout(ringCurrentLayout);
    storageRingLayout->addLayout(beamlifeLayout);

    connect(PGMBeamline::pgm()->exposedControlByName("Ring Current"), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChange(double)));
    connect(PGMBeamline::pgm()->exposedControlByName("Beam Lifetime"), SIGNAL(valueChanged(double)), this, SLOT(onLifetimeChange(double)));

    // Main layout
    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addLayout(storageRingLayout);

    setLayout(mainPanelLayout);

}

void PGMBeamStatusView::onRingCurrentChange(double value){
    // If beam has tripped of dipped too low, mark text as red.
    if(value < 100){
        ringCurrentValue_->setStyleSheet( "color : red"  );
        ringCurrentValue_->setText(QString("%1 mA").arg(value, 0, 'f', 2));
    } else {
        ringCurrentValue_->setStyleSheet( "color : black"  );
        ringCurrentValue_->setText(QString("%1 mA").arg(value, 0, 'f', 2));
    }
}

void PGMBeamStatusView::onLifetimeChange(double value){
    // lifetime must be multiplied by apprx 1.4 to be consistent with display screen
    beamLifetimeValue_->setText(QString("%1 Hrs").arg(1.44*value, 0, 'f', 2));
}
