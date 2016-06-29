#include "CLSBeamline.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/CLS/CLSBeamlineStatus.h"
#include "util/AMErrorMonitor.h"

/// ==================== constructors / destructor  =======================
CLSBeamline::CLSBeamline(const QString &controlName) :
	AMBeamline(controlName)
{
	beamlineStatus_ = 0;

	beamOnAction_ = 0;
	beamOffAction_ = 0;

	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(updateBeamStatus()));
	updateBeamStatus();
}

CLSBeamline::~CLSBeamline()

{

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

void CLSBeamline::updateBeamStatus()
{
	bool beamOn = beamlineStatus_->isOn() && CLSStorageRing::sr1()->beamAvailable();
	emit beamAvaliabilityChanged(beamOn);
}

void CLSBeamline::onBeamOnActionFinished(){
	disconnect( beamOnAction_, 0, this, 0 );

	beamOnAction_->deleteLater();
	beamOnAction_ = 0; //NULL
}

void CLSBeamline::onBeamOnActionFailed(){
	AMErrorMon::error(this, ERR_CLS_BEAM_ON_FAILED, QString("Failed to execute the beam on actions with message: %1.").arg(beamOnAction_->failureMessage()), true);
	onBeamOnActionFinished();
}

void CLSBeamline::onBeamOffActionFinished(){
	disconnect( beamOffAction_, 0, this, 0 );

	beamOffAction_->deleteLater();
	beamOffAction_ = 0; //NULL
}

/// ==================== protected methods =======================
void CLSBeamline::setBeamlineStatus(CLSBeamlineStatus *beamlineStatus)
{
	if (beamlineStatus_ != beamlineStatus) {
		if (beamlineStatus_)
			disconnect(beamlineStatus, 0, this, 0);

		beamlineStatus_ = beamlineStatus;
		if (beamlineStatus_) {
			connect(beamlineStatus_, SIGNAL(beamStatusChanged(bool)), this, SLOT(updateBeamStatus()));
		}
	}
}

AMAction3* CLSBeamline::createBeamOnActions() const
{
	AMErrorMon::alert(this, ERR_CLS_BEAM_ACTION_UNIMPLEMENTED, QString("There is no implementation for createBeamOnActions()"));
	return 0;
}

AMAction3* CLSBeamline::createBeamOffActions() const
{
	AMErrorMon::alert(this, ERR_CLS_BEAM_ACTION_UNIMPLEMENTED, QString("There is no implementation for createBeamOffActions()"));
	return 0;
}
