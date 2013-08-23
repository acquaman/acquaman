/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMBeamline.h"

#include "util/AMErrorMonitor.h"

#include "dataman/AMSampleContainer.h"
#include "dataman/AMSamplePlate.h"

AMBeamline* AMBeamline::instance_ = 0;


AMBeamline::AMBeamline(const QString& controlName)
	: AMControl(controlName, "")
{
	exposedControls_ = new AMControlSet(this);
	exposedDetectors_ = new AMDetectorSet(this);
	sampleContainer_ = new AMSampleContainer(this);
	samplePlate_ = 0; //NULL
	//connect(sampleContainer_, SIGNAL(sampleAdded(AMSample*)), this, SLOT(onSampleContainerSampleAdded(AMSample*)));
	samplePlateBrowser_ = 0;
}

AMBeamline::~AMBeamline()
{
}



void AMBeamline::releaseBl() {

	if(instance_) {
		delete instance_;
		instance_ = 0;
		}

}

AMBeamline * AMBeamline::bl()
{
	 if(!instance_) {
		 AMErrorMon::alert(0, AMBEAMLINE_BEAMLINE_NOT_CREATED_YET, "No beamline created yet.  You need to call YourBeamline::bl() before calling AMBeamline::bl().");
	 }

	return instance_;
}

AMSampleContainer* AMBeamline::sampleContainer(){
	return sampleContainer_;
}

AMSamplePlate* AMBeamline::samplePlate(){
	return samplePlate_;
}

AMSamplePlateBrowser* AMBeamline::samplePlateBrowser(){
	if(!samplePlateBrowser_)
		samplePlateBrowser_ = new AMSamplePlateBrowser(AMDatabase::database("user"), this);
	return samplePlateBrowser_;
}

void AMBeamline::setSamplePlate(AMSamplePlate *samplePlate){
	emit samplePlateAboutToChange(samplePlate_);
	samplePlate_ = samplePlate;
	emit samplePlateChanged(samplePlate_);

	if(samplePlate_)
		samplePlate_->storeToDb(AMDatabase::database("user"));
}

void AMBeamline::onSampleContainerSampleAdded(AMSample *sample){
	if(samplePlate_){
		qDebug() << "Heard sample named " << sample->name() << " added from sample container";
		samplePlate_->addSample(sample);
	}
}

bool AMBeamline::detectorAvailable(const AMDetectorInfo &detectorInfo){
	if(exposedDetectors()->detectorNamed(detectorInfo.name()) && exposedDetectors()->detectorNamed(detectorInfo.name())->isConnected())
		return true;
	return false;
}

AMDetectorGroup* AMBeamline::exposedDetectorGroupByName(const QString &name) const{
	for(int x = 0; x < exposedDetectorGroups_.count(); x++)
		if(exposedDetectorGroups_.at(x)->name() == name)
			return exposedDetectorGroups_.at(x);

	return 0; //NULL
}

bool AMBeamline::addExposedDetectorGroup(AMDetectorGroup *detectorGroup){
	for(int x = 0; x < exposedDetectorGroups_.count(); x++)
		if(exposedDetectorGroups_.at(x)->name() == detectorGroup->name())
			return false;

	exposedDetectorGroups_.append(detectorGroup);
	return true;
}
