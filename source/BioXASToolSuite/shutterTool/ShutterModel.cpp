/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "ShutterModel.h"
#include <QDebug>

ShutterModel::ShutterModel(QObject *parent) : QObject(parent)
{
    // the pvs we will be connecting to.
    aodShutterControl_ = 0;
    safetyShutter_ = 0;
    photonShutter_ = 0;

    // the application status: will shutters automatically open when AOD enables shutter control?
    automaticShuttersOpen_ = false;

    // the actions responsible for physically opening and closing the shutters.
    openAction_ = 0;
    closeAction_ = 0;

    createComponents();
    makeConnections();
}



bool ShutterModel::aodShutterControlConnected()
{
    return aodShutterControl_->isConnected();
}



bool ShutterModel::shuttersConnected()
{
    if (safetyShutter_->isConnected() && photonShutter_->isConnected())
        return true;
    else
        return false;
}



bool ShutterModel::isOpen()
{
    return (safetyShutter_->isOpen() && photonShutter_->isOpen());
}



bool ShutterModel::isClosed()
{
    return (safetyShutter_->isClosed() && photonShutter_->isClosed());
}



bool ShutterModel::isBetween()
{
    if (!isOpen() && !isClosed())
        return true;
    else
        return false;
}



void ShutterModel::openShutters()
{
    openAction_ = createOpenShuttersAction();
    connect( openAction_, SIGNAL(succeeded()), this, SLOT(onOpenActionFinished()) );
    connect( openAction_, SIGNAL(failed()), this, SLOT(onOpenActionFinished()) );

    qDebug() << "Model::openShutters() : beginning shutter open action.";
    openAction_->start();
}



void ShutterModel::closeShutters()
{
    closeAction_ = createCloseShuttersAction();

    connect( closeAction_, SIGNAL(succeeded()), this, SLOT(onCloseActionFinished()) );
    connect( closeAction_, SIGNAL(failed()), this, SLOT(onCloseActionFinished()) );

    qDebug() << "Model::closeShutters() : beginning shutter close action.";
    closeAction_->start();
}



void ShutterModel::setAutomaticShuttersOpen(bool enabled)
{
    automaticShuttersOpen_ = enabled;

    if (enabled)
        qDebug() << "Model :: automatic shutters open feature is enabled.";
    else
        qDebug() << "Model :: automatic shutters open feature is disabled.";
}



void ShutterModel::onAODShutterControlConnected(bool isConnected)
{
    if (isConnected) {
        qDebug() << "Model :: Connected to AOD shutters enabled/disabled pv.";
    } else {
        qDebug() << "Model :: Disconnected from AOD shutters enabled/disabled pv.";
    }

    connectedCheck();
}



void ShutterModel::onAODShutterControlValueChanged(double enabledState)
{
    // if beamline control of shutters is enabled and the automatic open feature is enabled, open the beamline shutters.
	if (enabledState == ShutterModel::Enabled) {
        qDebug() << "Model :: Beamline control of shutters is enabled (likely after injection).";
        emit postInjectionControlsEnabled();

        if (automaticShuttersOpen_)
            openShutters();

	} else if (enabledState == ShutterModel::Disabled) {
        qDebug() << "Model :: Beamline control of shutters is disabled (likely for injection?).";
        emit preInjectionControlsDisabled();

    } else {
        qDebug() << "Model :: Beamline control of shutters is in an unknown state. (!!)";
    }
}



void ShutterModel::onSafetyShutterConnected(bool isConnected)
{
    if (isConnected) {
        qDebug() << "Model :: Connected to safety shutter.";

    } else {
        qDebug() << "Model :: Disconnected from safety shutter.";
    }

    connectedCheck();
}



void ShutterModel::onSafetyShutterStateChanged(int newState)
{
	if (newState == ShutterModel::Open)
        qDebug() << "Model :: Safety shutter is open.";
	else if (newState == ShutterModel::Closed)
        qDebug() << "Model :: Safety shutter is closed.";
    else
        qDebug() << "Model :: Safety shutter is in an unknown state. (!!)";

    shutterStateCheck();
}



void ShutterModel::onPhotonShutterConnected(bool isConnected)
{
    if (isConnected) {
        qDebug() << "Model :: Connected to photon shutter.";

    } else {
        qDebug() << "Model :: Disconnected from photon shutter.";
    }

    connectedCheck();
}



void ShutterModel::onPhotonShutterStateChanged(int newState)
{
	if (newState == ShutterModel::Open)
        qDebug() << "Model :: Photon shutter is open.";
	else if (newState == ShutterModel::Closed)
        qDebug() << "Model :: Photon shutter is closed.";
    else
        qDebug() << "Model :: Photon shutter is in an unknown state. (!!)";

    shutterStateCheck();
}



void ShutterModel::onOpenActionFinished()
{
    qDebug() << "Open action finished with state : " << openAction_->statusText();

    disconnect( openAction_, SIGNAL(succeeded()), this, SLOT(onOpenActionFinished()) );
    disconnect( openAction_, SIGNAL(failed()), this, SLOT(onOpenActionFinished()) );

    openAction_->deleteLater();
    openAction_ = 0;
}



void ShutterModel::onCloseActionFinished()
{
    qDebug() << "Close action finished with state : " << closeAction_->statusText();

    disconnect( closeAction_, SIGNAL(succeeded()), this, SLOT(onCloseActionFinished()) );
    disconnect( closeAction_, SIGNAL(failed()), this, SLOT(onCloseActionFinished()) );

    closeAction_->deleteLater();
    closeAction_ = 0;
}



void ShutterModel::onErrorReported(const QString &errorInfo)
{
    qDebug() << "Model :: Error from pvs reported:";
    qDebug() << errorInfo;
}



void ShutterModel::createComponents()
{
    aodShutterControl_ = new AMReadOnlyPVControl("SRStatus:shutters", "SRStatus:shutters", this);
    safetyShutter_ = new CLSBiStateControl("safety shutter", "safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
    photonShutter_ = new CLSBiStateControl("photon shutter", "photon shutter", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
}



void ShutterModel::makeConnections()
{
    connect( aodShutterControl_, SIGNAL(connected(bool)), this, SLOT(onAODShutterControlConnected(bool)) );
    connect( aodShutterControl_, SIGNAL(valueChanged(double)), this, SLOT(onAODShutterControlValueChanged(double)) );
    connect( aodShutterControl_, SIGNAL(error(QString)), this, SLOT(onErrorReported(QString)) );

    connect( safetyShutter_, SIGNAL(connected(bool)), this, SLOT(onSafetyShutterConnected(bool)) );
    connect( safetyShutter_, SIGNAL(stateChanged(int)), this, SLOT(onSafetyShutterStateChanged(int)) );
    connect( safetyShutter_, SIGNAL(error(QString)), this, SLOT(onErrorReported(QString)) );

    connect( photonShutter_, SIGNAL(connected(bool)), this, SLOT(onPhotonShutterConnected(bool)) );
    connect( photonShutter_, SIGNAL(stateChanged(int)), this, SLOT(onPhotonShutterStateChanged(int)) );
    connect( photonShutter_, SIGNAL(error(QString)), this, SLOT(onErrorReported(QString)) );
}



void ShutterModel::connectedCheck()
{
    // check that we are connected to the aod pv for telling us beamline shutter controls are en/disabled.
    if (aodShutterControl_->isConnected())
        emit aodShutterControlConnected(true);
    else
        emit aodShutterControlConnected(false);

    // check that we are connected to the appropriate shutter control pvs.
    if (safetyShutter_->isConnected() && photonShutter_->isConnected())
        emit shuttersConnected(true);
    else
        emit shuttersConnected(false);
}



void ShutterModel::shutterStateCheck()
{
    if (isOpen())
        emit shuttersOpen();

    else if (isClosed())
        emit shuttersClosed();

    else
        emit shuttersBetween();
}



AMAction3* ShutterModel::createOpenShuttersAction()
{
    AMSequentialListAction3 *openShuttersAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("Opens safety and photon shutters.", "Opens safety and photon shutters."));

    AMControlInfo setpoint = safetyShutter_->toInfo();
	setpoint.setValue(ShutterModel::Open);
    AMControlMoveActionInfo3 *openShutterInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *openShutterAction = new AMControlMoveAction3(openShutterInfo, safetyShutter_);
    openShuttersAction->addSubAction(openShutterAction);

    setpoint = photonShutter_->toInfo();
	setpoint.setValue(ShutterModel::Open);
    openShutterInfo = new AMControlMoveActionInfo3(setpoint);
    openShutterAction = new AMControlMoveAction3(openShutterInfo, photonShutter_);
    openShuttersAction->addSubAction(openShutterAction);

    return openShuttersAction;
}



AMAction3* ShutterModel::createCloseShuttersAction()
{
    AMSequentialListAction3 *closeShuttersAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("Closes photon and safety shutters.", "Closes photon and safety shutters."));

    AMControlInfo setpoint = photonShutter_->toInfo();
	setpoint.setValue(ShutterModel::Closed);
    AMControlMoveActionInfo3 *closeShutterInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *closeShutterAction = new AMControlMoveAction3(closeShutterInfo, photonShutter_);
    closeShuttersAction->addSubAction(closeShutterAction);

    setpoint = safetyShutter_->toInfo();
	setpoint.setValue(ShutterModel::Closed);
    closeShutterInfo = new AMControlMoveActionInfo3(setpoint);
    closeShutterAction = new AMControlMoveAction3(closeShutterInfo, safetyShutter_);
    closeShuttersAction->addSubAction(closeShutterAction);

    return closeShuttersAction;
}
