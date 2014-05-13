#include "BioXASSideBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideBeamline::BioXASSideBeamline()
	: AMBeamline("BioXAS Beamline - Side Endstation")
{
    wasConnected_ = false;

	setupSynchronizedDwellTime();
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}

void BioXASSideBeamline::setupDiagnostics()
{

}

void BioXASSideBeamline::setupSampleStage()
{

}

void BioXASSideBeamline::setupMotorGroup()
{

}

void BioXASSideBeamline::setupDetectors()
{

}

void BioXASSideBeamline::setupControlSets()
{

}

void BioXASSideBeamline::setupMono()
{

}

void BioXASSideBeamline::setupSynchronizedDwellTime()
{

}

void BioXASSideBeamline::setupComponents()
{
    scaler_ = new CLSSIS3820Scaler("BL07ID-Side:mcs", this);

    connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnectedChanged(bool)) );

    scaler_->channelAt(0)->setCustomChannelName("Test channel");

    m1UpperSlit_ = new CLSMAXvMotor("M1 Upper Slit", "SMTR1607-5-I22-08", "M1 Upper Slit Motor", true, 0.05, 2.0, this);
    m1UpperSlit_->setContextKnownDescription("Upper slit");

    connect( m1UpperSlit_, SIGNAL(connected(bool)), this, SLOT(onM1UpperSlitConnectedChanged(bool)) );
}

void BioXASSideBeamline::setupControlsAsDetectors()
{

}

void BioXASSideBeamline::setupExposedControls()
{

}

void BioXASSideBeamline::setupExposedDetectors()
{

}

BioXASSideBeamline::~BioXASSideBeamline()
{

}

CLSSIS3820Scaler* BioXASSideBeamline::scaler()
{
    return scaler_;
}

CLSMAXvMotor* BioXASSideBeamline::m1UpperSlit()
{
    return m1UpperSlit_;
}

bool BioXASSideBeamline::isConnected() const
{
    return scaler_->isConnected() && m1UpperSlit_->isConnected();
}

void BioXASSideBeamline::onScalerConnectedChanged(bool connectionState)
{
    Q_UNUSED(connectionState)

    if (wasConnected_ && !isConnected()) {
        emit connected(false);
        wasConnected_ = false;

    } else if (!wasConnected_ && isConnected()) {
        emit connected(true);
        wasConnected_ = true;
    }

}

void BioXASSideBeamline::onM1UpperSlitConnectedChanged(bool connectionState)
{
    Q_UNUSED(connectionState)

    if (wasConnected_ && !isConnected()) {
        emit connected(false);
        wasConnected_ = false;

    } else if (!wasConnected_ && isConnected()) {
        emit connected(true);
        wasConnected_ = true;
    }
}
