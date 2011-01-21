/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMDetector.h"


AMSingleControlDetector::AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent) :
		AMDetectorInfo(name, name, parent)
{
	control_ = control;
}

bool AMSingleControlDetector::setControls(AMDetectorInfo *detectorSettings){
	return false;
}

AMSpectralOutputDetector::AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMSpectralOutputDetectorInfo(name, name, numSpectrumBins, xElementName, yElementNames, parent)
{
	control_ = control;
}

bool AMSpectralOutputDetector::setControls(AMSpectralOutputDetectorInfo *detectorSettings){
	return false;
}

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent) :
		PGTDetectorInfo(name, name, parent)
{
	dataWaveform_ = dataWaveform;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	integrationTime_ = integrationTime;
	integrationMode_ = integrationMode;
}

PGTDetector::~PGTDetector()
{
	dataWaveform_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
	integrationTime_ = NULL;
	integrationMode_ = NULL;
}

bool PGTDetector::settingsMatchFbk(PGTDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeCtrl()->value()) > integrationTimeCtrl()->tolerance() )
		return rVal;
	else if( fabs( settings->integrationModeList().indexOf(settings->integrationMode()) - integrationModeCtrl()->value()) > integrationModeCtrl()->tolerance() )
		return rVal;
	else if( fabs(settings->hvSetpoint() - hvFbkCtrl()->value()) > hvFbkCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

bool PGTDetector::setControls(PGTDetectorInfo *pgtSettings){
	hvSetpoint_->move( pgtSettings->hvSetpoint() );
	integrationTime_->move( pgtSettings->integrationTime() );
	integrationMode_->move( integrationMode_->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent) :
		MCPDetectorInfo(name, name, parent)
{
	reading_ = reading;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
}

MCPDetector::~MCPDetector(){
	reading_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
}

bool MCPDetector::settingsMatchFbk(MCPDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->hvSetpoint() - hvFbkCtrl()->value()) > hvFbkCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

bool MCPDetector::setControls(MCPDetectorInfo *mcpSettings){
	hvSetpoint_->move( mcpSettings->hvSetpoint() );
	return true;
}
