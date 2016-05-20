#include "PGMSlitControlView.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

#include "beamline/PGM/PGMBeamline.h"
#include "ui/beamline/AMSlitView.h"


PGMSlitControlView::PGMSlitControlView(QWidget *parent) :
    QWidget(parent)
{

    entranceLabel_ = new QLabel("Entrance Width:");
    entranceWidthEdit_= new QLineEdit();

    positionSlitView_ = new AMSlitView(PGMBeamline::pgm()->positionSlit(), false, this);
    widthSlitView_ = new AMSlitView(PGMBeamline::pgm()->gapSlit(), false, this);

    QHBoxLayout *positionSlitLayout = new QHBoxLayout;
    positionSlitLayout->addWidget(positionSlitView_);
    QHBoxLayout *widthGapLayout = new QHBoxLayout;
    widthGapLayout->addWidget(widthSlitView_);

    QGroupBox *positionBox = new QGroupBox("Position Slit");
    positionBox->setLayout(positionSlitLayout);
    QGroupBox *widthBox = new QGroupBox("Gap Width");
    widthBox->setLayout(widthGapLayout);

    QHBoxLayout *entranceLayout = new QHBoxLayout;
    entranceLayout->addWidget(entranceLabel_);
    entranceLayout->addWidget(entranceWidthEdit_);
    entranceLayout->addSpacing(50);

    QHBoxLayout *slitsLayout = new QHBoxLayout;
    slitsLayout->addWidget(positionBox);
    slitsLayout->addWidget(widthBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(entranceLayout);
    mainLayout->addLayout(slitsLayout);

    setLayout(mainLayout);

    connect(PGMBeamline::pgm()->entranceSlit(), SIGNAL(valueChanged(double)), this, SLOT(onEntranceSlitChanged(double)));
    connect(entranceWidthEdit_, SIGNAL(returnPressed()), this, SLOT(setEntranceSlitValue()));

}

void PGMSlitControlView::onEntranceSlitChanged(double newValue){

    entranceWidthEdit_->setText(QString("%1 mm").arg(newValue, 0, 'f', 2));
}

bool PGMSlitControlView::setEntranceSlitValue(){

    // Grab value from edit field and check to see if we are already at that value.
    double setpoint = entranceWidthEdit_->text().toDouble();
    if(setpoint = PGMBeamline::pgm()->entranceSlit()->value())
        return false;

    AMControl::FailureExplanation failureExplanation = PGMBeamline::pgm()->entranceSlit()->move(setpoint);

    return (failureExplanation == AMControl::NoFailure);
}
