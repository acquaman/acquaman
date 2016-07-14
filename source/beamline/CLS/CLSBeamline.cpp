#include "CLSBeamline.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/CLS/CLSBeamlineStatus.h"
#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"

#include "util/AMErrorMonitor.h"

/// ==================== constructors / destructor  =======================
CLSBeamline::CLSBeamline(const QString &beamlineName, const QString &controlName) :
	AMBeamline(controlName)
{
	setBeamlineName(beamlineName);

	wasConnected_ = false;
	wasBeamOn_ = false;

	beamlineStatus_ = 0;
	beamlineShutters_ = 0;
	beamlineValves_ = 0;

	beamOnAction_ = 0;
	beamOffAction_ = 0;

	requiredControls_ = new AMControlSet(this);
	requiredDetector_ = new AMDetectorSet(this);

	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(updateBeamStatus()));
	connect(requiredControls_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()));
	connect(requiredDetector_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()));
}

CLSBeamline::~CLSBeamline() { }

/// ==================== public slots =======================

bool CLSBeamline::isConnected() const
{
	return	(requiredControls_->isEmpty() || requiredControls_->isConnected())
		 && (requiredDetector_->isEmpty() || requiredDetector_->isConnnected());
}

CLSBeamlineStatus* CLSBeamline::beamlineStatus() const
{
	return beamlineStatus_;
}

/// ==================== public slots =======================
void CLSBeamline::onTurningBeamOnRequested(){
	if(beamOnAction_)
		return;

	beamOnAction_ = createBeamOnActions();
	if (beamOnAction_) {
		connect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
		connect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
		beamOnAction_->start();
	}
}

void CLSBeamline::onTurningBeamOffRequested(){
	if(beamOffAction_)
		return;

	beamOffAction_ = createBeamOffActions();
	if (beamOffAction_) {
		connect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
		connect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
		beamOffAction_->start();
	}
}


/// ==================== protected slots =======================

void CLSBeamline::onBeamlineComponentConnected()
{
	bool currentConnectedStatus = isConnected();
	if (wasConnected_ != currentConnectedStatus) {
		wasConnected_ = currentConnectedStatus;
		emit connected(wasConnected_);
	}
}

void CLSBeamline::updateBeamStatus()
{
	bool beamOn = CLSStorageRing::sr1()->beamAvailable();
	if (beamlineStatus_) {
		beamOn &= beamlineStatus_->isOn();
	}

	if (wasBeamOn_ != beamOn) {
		wasBeamOn_ = beamOn;
		emit beamAvaliabilityChanged(beamOn);
	}
}

void CLSBeamline::onBeamOnActionFinished(){
	disconnect( beamOnAction_, 0, this, 0 );

	beamOnAction_->deleteLater();
	beamOnAction_ = 0; //NULL
}

void CLSBeamline::onBeamOnActionFailed(){
	AMErrorMon::error(this, CLSBEAMLINE_ERR_BEAM_ON_FAILED, QString("Failed to execute the beam on actions with message: %1.").arg(beamOnAction_->failureMessage()), true);
	onBeamOnActionFinished();
}

void CLSBeamline::onBeamOffActionFinished(){
	disconnect( beamOffAction_, 0, this, 0 );

	beamOffAction_->deleteLater();
	beamOffAction_ = 0; //NULL
}

/// ==================== protected methods =======================

void CLSBeamline::createBeamlineStatus(CLSShutters *shutters, CLSValves *valves)
{
	if (shutters)
		beamlineShutters_ = shutters;
	else
		beamlineShutters_ = new CLSShutters(QString("%1 Shutters").arg(beamlineName_), this);

	if (valves)
		beamlineValves_ = valves;
	else
		beamlineValves_ = new CLSValves(QString("%1 Valves").arg(beamlineName_), this);

	CLSBeamlineStatus *beamlineStatus = new CLSBeamlineStatus(QString("%1 BeamlineStatus").arg(beamlineName_), this);
	beamlineStatus->addShutterControl(beamlineShutters_, CLSShutters::Open);
	beamlineStatus->addValveControl(beamlineValves_, CLSValves::Open);

	setBeamlineStatus(beamlineStatus);
}

void CLSBeamline::initializeBeamline()
{
	if (beamlineStatus_ && beamlineStatus_->isConnected()) {
		updateBeamStatus();
	}

	onBeamlineComponentConnected();
}

void CLSBeamline::setBeamlineStatus(CLSBeamlineStatus *beamlineStatus)
{
	if (beamlineStatus_ != beamlineStatus) {
		if (beamlineStatus_) {
			requiredControls_->removeControl(beamlineStatus_);
			disconnect(beamlineStatus, 0, this, 0);
		}

		beamlineStatus_ = beamlineStatus;
		if (beamlineStatus_) {
			requiredControls_->addControl(beamlineStatus_);

			connect(beamlineStatus_, SIGNAL(beamStatusChanged(bool)), this, SLOT(updateBeamStatus()));
			connect(beamlineStatus_, SIGNAL(connected(bool)), this, SLOT(updateBeamStatus()));
		}

		updateBeamStatus();
	}
}

AMListAction3* CLSBeamline::createBeamOnActions() const
{
	if (beamlineStatus_->isOn()) {
		AMErrorMon::error(this, CLSBEAMLINE_ERR_BEAM_ON_ALREADY_ON, QString("Failed to create the beam on actions because the beam is already ready."), true);
		return 0;
	}

	if(!beamlineShutters_->isConnected() || !beamlineValves_->isConnected()) {
		AMErrorMon::error(this, CLSBEAMLINE_ERR_BEAM_ON_UNCONNECTED_PV, QString("Failed to create the beam on actions due to unconnected shutter/valve PVs."), true);
		return 0;
	}

	if (!beamlineShutters_->isSafetyShutterOpen()) {
		// safety shutter is NOT open. We can't turn beam on now for safety reason
		AMErrorMon::alert(this, CLSBEAMLINE_ERR_BEAM_ON_CLOSED_SAFETY_SHUTTER, QString("The safety shutter is closed. We can't turn beam on for safety reason."), true);
		return 0;
	}

	// create the beam on action list.
	AMListAction3 *beamOnActionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Beam On").arg(beamlineName_), QString("%1 Beam On").arg(beamlineName_)), AMListAction3::Sequential);

	AMAction3 *openValvesActionsList = beamlineValves_->createBeamOnActionList();
	if (openValvesActionsList) {
		beamOnActionsList->addSubAction(openValvesActionsList);
	}

	AMAction3 *openPhotonShutterActionsList = beamlineShutters_->createBeamOnActionList();
	if (openPhotonShutterActionsList) {
		beamOnActionsList->addSubAction(openPhotonShutterActionsList);
	}

	return beamOnActionsList;
}

AMListAction3* CLSBeamline::createBeamOffActions() const
{
	if(beamlineStatus_->isOff()) {
		AMErrorMon::error(this, CLSBEAMLINE_ERR_BEAM_OFF_ALREADY_OFF, QString("Failed to create the beam off actions because beam is already off."));
		return 0;
	}

	if(!beamlineShutters_->isConnected() || !beamlineValves_->isConnected()) {
		AMErrorMon::error(this, CLSBEAMLINE_ERR_BEAM_OFF_UNCONNECTED_PV, QString("Failed to create the beam off actions due to unconnected PVs."));
		return 0;
	}

	// create the beam off action list.
	AMListAction3 *beamOffActionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Beam Off").arg(beamlineName_), QString("%1 Beam Off").arg(beamlineName_)), AMListAction3::Sequential);

	AMListAction3 * closeShuttersActionsList = beamlineShutters_->createBeamOffActionList();
	if (closeShuttersActionsList) {
		beamOffActionsList->addSubAction(closeShuttersActionsList);
	} else {
		beamOffActionsList->deleteLater();
		beamOffActionsList = 0;
	}

	return beamOffActionsList;
}
