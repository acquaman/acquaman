#include "BioXASSidePersistentView.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "ui/beamline/AMCurrentAmplifierView.h"


BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
    QWidget(parent)
{
    motorControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->m1UpperSlit());
    motorControlEditor_->setControlFormat('g', 4);

    keithleyView_ = BioXASSideBeamline::bioXAS()->keithley()->createView();
    keithleyView_->setParent(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(motorControlEditor_);
    layout->addWidget(keithleyView_);
    layout->addStretch();

    setLayout(layout);
}
