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


#include "AMScanConfiguration.h"

AMScanConfiguration::AMScanConfiguration(QObject *parent) :
	AMDbObject(parent)
{
	userScanName_ = "";
	userExportName_ = "$name.txt";
	autoExportEnabled_ = true;
	expectedDuration_ = 0;
}

AMScanConfiguration::AMScanConfiguration(const QList<AMScanConfiguration*> &configurations, QObject *parent) :
	AMDbObject(parent)
{
	userScanName_ = "";
	userExportName_ = "$name.txt";
	autoExportEnabled_ = true;
	expectedDuration_ = 0;

	foreach (AMScanConfiguration *configuration, configurations)
		merge(configuration);
}

AMScanConfiguration::AMScanConfiguration(const AMScanConfiguration &original)
	: AMDbObject(original)
{
	userScanName_ = original.userScanName();
	userExportName_ = original.userExportName();
	autoExportEnabled_ = original.autoExportEnabled();
	expectedDuration_ = original.expectedDuration();
	axisControlInfos_ = original.axisControlInfos();
	detectorConfigurations_ = original.detectorConfigurations();
}

const QMetaObject* AMScanConfiguration::getMetaObject(){
	return metaObject();
}

QString AMScanConfiguration::description() const {
	return "Generic Scan";
}

QString AMScanConfiguration::detailedDescription() const{
	return "Generic Scan Details";
}

QString AMScanConfiguration::autoScanName() const{
	return "Generic Scan";
}

QString AMScanConfiguration::userScanName() const{
	return userScanName_;
}

QString AMScanConfiguration::userExportName() const{
	return userExportName_;
}

QString AMScanConfiguration::technique() const{
	return "[None]";
}

bool AMScanConfiguration::autoExportEnabled() const{
	return autoExportEnabled_;
}

bool AMScanConfiguration::canEnumConvert(const QString &enumName) const{
	Q_UNUSED(enumName)
	return false;
}

QString AMScanConfiguration::enumConvert(const QString &enumName, int enumValue) const{
	Q_UNUSED(enumName)
	Q_UNUSED(enumValue)
	return "[??]";
}

bool AMScanConfiguration::hasDetectorInfo(const AMDetectorInfo &newDetectorInfo) const
{
	bool detectorFound = false;

	for (int detectorIndex = 0, detectorCount = detectorConfigurations_.count(); detectorIndex < detectorCount && !detectorFound; detectorIndex++) {
		AMDetectorInfo detectorInfo = detectorConfigurations_.at(detectorIndex);
		if (detectorInfo.name() == newDetectorInfo.name())
			detectorFound = true;
	}

	return detectorFound;
}

bool AMScanConfiguration::hasAxisControlInfo(const AMControlInfo &newControlInfo) const
{
	bool controlFound = false;

	for (int controlIndex = 0, controlCount = axisControlInfos_.count(); controlIndex < controlCount && !controlFound; controlIndex++) {
		AMControlInfo controlInfo = axisControlInfos_.at(controlIndex);
		if (controlInfo.name() == newControlInfo.name())
			controlFound = true;
	}

	return controlFound;
}

void AMScanConfiguration::setUserScanName(const QString &userScanName){
	if(userScanName_ != userScanName){
		userScanName_ = userScanName;
		emit userScanNameChanged(userScanName_);
		emit configurationChanged();
		setModified(true);
	}
}

void AMScanConfiguration::setUserExportNmae(const QString &userExportName){
	if(userExportName_ != userExportName){
		userExportName_ = userExportName;
		emit userExportNameChanged(userExportName_);
		emit configurationChanged();
		setModified(true);
	}
}

void AMScanConfiguration::setAutoExportEnabled(bool autoExportEnabled){
	if(autoExportEnabled_ != autoExportEnabled){
		autoExportEnabled_ = autoExportEnabled;
		emit autoExportEnabledChanged(autoExportEnabled_);
		emit configurationChanged();
		setModified(true);
	}
}

void AMScanConfiguration::setExpectedDuration(double duration)
{
	if (expectedDuration_ != duration){

		expectedDuration_ = duration;
		emit expectedDurationChanged(expectedDuration_);
	}
}

void AMScanConfiguration::setDetectorConfigurations(const AMDetectorInfoSet &detectorConfigurations){
	if(detectorConfigurations_ != detectorConfigurations){
		detectorConfigurations_ = detectorConfigurations;
		setModified(true);
		emit detectorConfigurationsChanged();
	}
}

void AMScanConfiguration::setAxisControlInfos(const AMControlInfoList &axisControlInfos)
{
	if (axisControlInfos_ != axisControlInfos){

		axisControlInfos_ = axisControlInfos;
		setModified(true);
		emit axisControlInfosChanged();
	}
}

void AMScanConfiguration::merge(AMScanConfiguration *configuration)
{
	if (configuration) {

		// Add the axis control infos of the new configuration to this one, provided this one
		// doesn't have them already.

		QList<AMControlInfo> newControlInfos = configuration->axisControlInfos().toList();
		foreach (AMControlInfo controlInfo, newControlInfos)
			if (!hasAxisControlInfo(controlInfo))
				axisControlInfos_.append(controlInfo);

		// Add the detector infos of the new configuration to this one, provided this one doesn't
		// have them already.

		QList<AMDetectorInfo> newDetectorInfos = configuration->detectorConfigurations().toList();
		foreach (AMDetectorInfo detectorInfo, newDetectorInfos)
			if (!hasDetectorInfo(detectorInfo))
				detectorConfigurations_.addDetectorInfo(detectorInfo);
	}
}
