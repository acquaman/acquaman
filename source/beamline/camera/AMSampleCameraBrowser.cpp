#include "AMSampleCameraBrowser.h"

#include <QWidget>
#include "ui/beamline/camera/AMSampleCameraView.h"
#include "beamline/camera/AMSampleCamera.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePlate.h"

#include "beamline/AMBeamline.h"

#include <QDebug>


AMSampleCameraBrowser::AMSampleCameraBrowser(QObject *parent) :
	QObject(parent)
{
	shapeDataSet_ = AMSampleCamera::set();
	currentSamplePlate_ = 0; //NULL
	onSamplePlateChanged(AMBeamline::bl()->samplePlate());

	connect(AMBeamline::bl(), SIGNAL(samplePlateChanged(AMSamplePlate*)), this, SLOT(onSamplePlateChanged(AMSamplePlate*)));
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

void AMSampleCameraBrowser::onSamplePlateChanged(AMSamplePlate *samplePlate){
	if(currentSamplePlate_)
		disconnect(shapeDataSet_, SIGNAL(shapesChanged()), currentSamplePlate_, SLOT(onSampleCameraShapesChanged()));
	currentSamplePlate_ = samplePlate;
	if(currentSamplePlate_)
		connect(shapeDataSet_, SIGNAL(shapesChanged()), currentSamplePlate_, SLOT(onSampleCameraShapesChanged()));
}
