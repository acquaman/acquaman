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
		if(newURL){
			urls_.append(newURL);
			qDebug() << "Loading up url " << newURL->urlString();
		}
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

QList<AMSampleCameraURL*> AMSampleCameraBrowser::urls(){
	return urls_;
}

void AMSampleCameraBrowser::onSamplePlateChanged(AMSamplePlate *samplePlate){
	if(currentSamplePlate_)
		disconnect(shapeDataSet_, SIGNAL(shapesChanged()), currentSamplePlate_, SLOT(onSampleCameraShapesChanged()));
	currentSamplePlate_ = samplePlate;
	if(currentSamplePlate_)
		connect(shapeDataSet_, SIGNAL(shapesChanged()), currentSamplePlate_, SLOT(onSampleCameraShapesChanged()));
}
