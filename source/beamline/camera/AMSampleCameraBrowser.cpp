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


#include "AMSampleCameraBrowser.h"

#include <QWidget>
#include "ui/beamline/camera/AMSampleCameraView.h"
#include "beamline/camera/AMSampleCamera.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePlate.h"

#include "beamline/AMBeamline.h"

#include <QDebug>

AMSampleCameraURL::AMSampleCameraURL(const QString &urlString, QObject *parent) :
	AMDbObject(parent)
{
	urlString_ = urlString;
}

QString AMSampleCameraURL::urlString() const{
	return urlString_;
}

void AMSampleCameraURL::setURLString(const QString &urlString){
	urlString_ = urlString;
}

AMSampleCameraBrowser::AMSampleCameraBrowser(QObject *parent) :
	QObject(parent)
{
	shapeDataSet_ = AMSampleCamera::set();
	currentSamplePlate_ = 0; //NULL
	onSamplePlateChanged(AMBeamline::bl()->samplePlate());

	connect(AMBeamline::bl(), SIGNAL(samplePlateChanged(AMSamplePlate*)), this, SLOT(onSamplePlateChanged(AMSamplePlate*)));

	QList<int> allURLIds = AMDatabase::database("user")->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<AMSampleCameraURL>());
	AMDbObject *newURLObject;
	AMSampleCameraURL *newURL;
	for(int x = 0; x < allURLIds.count(); x++){
		newURLObject = AMDbObjectSupport::s()->createAndLoadObjectAt(AMDatabase::database("user"), AMDbObjectSupport::s()->tableNameForClass<AMSampleCameraURL>(), allURLIds.at(x));
		newURL = qobject_cast<AMSampleCameraURL*>(newURLObject);
		if(newURL)
			urls_.append(newURL);
	}
}

QString AMSampleCameraBrowser::currentURL()
{
	return currentURL_;
}

AMSampleCamera *AMSampleCameraBrowser::shapeDataSet()
{
	return shapeDataSet_;
}


void AMSampleCameraBrowser::setCurrentURL(QString currentURL)
{
	currentURL_ = currentURL;
}

void AMSampleCameraBrowser::addURL(const QString &urlString){
	AMSampleCameraURL *newURL = new AMSampleCameraURL(urlString);
	newURL->storeToDb(AMDatabase::database("user"));
	urls_.append(newURL);
}

void AMSampleCameraBrowser::removeURL(const QString &urlString){
	AMSampleCameraURL *removeURL = 0;
	for(int x = 0; x < urls_.count(); x++)
		if(urls_.at(x)->urlString() == urlString)
			removeURL = urls_.at(x);

	if(removeURL){
		urls_.removeAll(removeURL);
		AMDatabase::database("user")->deleteRow(removeURL->id(), AMDbObjectSupport::s()->tableNameForClass<AMSampleCameraURL>());
	}
}

QList<AMSampleCameraURL*> AMSampleCameraBrowser::urls(){
	return urls_;
}

/// Manage drawing of the new sample plate
void AMSampleCameraBrowser::onSamplePlateChanged(AMSamplePlate *samplePlate){
	// disconnect all shapes from the old sample.
	if(currentSamplePlate_){
		disconnect(shapeDataSet_, SIGNAL(shapeDataChanged()), this, SLOT(onCameraShapesChanged()));
	}
	// set the new sample plate
	currentSamplePlate_ = samplePlate;
	// Add all the shapes from the database
	if(currentSamplePlate_)
	{
		foreach (AMSample* sample, currentSamplePlate_->allSamples())
		{
			shapeDataSet()->addSample(sample);
		}
		// connect new shapes to the sample plate
		connect(shapeDataSet_, SIGNAL(shapeDataChanged()), this, SLOT(onCameraShapesChanged()));
	}
}

void AMSampleCameraBrowser::onCameraShapesChanged(){
	if(currentSamplePlate_)
		currentSamplePlate_->onSampleCameraShapesChanged(shapeDataSet_->shapeList());
}
