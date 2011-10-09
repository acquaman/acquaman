/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
	userExportName_ = "$name.dat";
	autoExportEnabled_ = true;
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

void AMScanConfiguration::setUserScanName(const QString &userScanName){
	if(userScanName_ != userScanName){
		userScanName_ = userScanName;
		emit userScanNameChanged(userScanName_);
	}
}

void AMScanConfiguration::setUserExportNmae(const QString &userExportName){
	if(userExportName_ != userExportName){
		userExportName_ = userExportName;
		emit userExportNameChanged(userExportName_);
	}
}

void AMScanConfiguration::setAutoExportEnabled(bool autoExportEnabled){
	if(autoExportEnabled_ != autoExportEnabled){
		autoExportEnabled_ = autoExportEnabled;
		emit autoExportEnabledChanged(autoExportEnabled_);
	}
}
