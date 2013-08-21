#include "AMSampleCameraBrowser.h"

#include <QWidget>
#include "ui/beamline/camera/AMSampleCameraView.h"
#include "beamline/camera/AMSampleCamera.h"
#include "dataman/AMSampleContainer.h"
#include "dataman/AMSample.h"

#include "beamline/AMBeamline.h"

#include <QDebug>


AMSampleCameraBrowser::AMSampleCameraBrowser(QObject *parent) :
	QObject(parent)
{
	qRegisterMetaType<AMQVector3DVector>();

    shapeDataSet_ = AMSampleCamera::set();
	//sampleContainer_ = new AMSampleContainer();
	sampleContainer_ = AMBeamline::bl()->sampleContainer();

	connect(shapeDataSet_, SIGNAL(shapesChanged()), sampleContainer_, SLOT(updateSamples()));
	connect(this, SIGNAL(indexChanged(int)), shapeDataSet_, SLOT(setCurrentIndex(int)));
	connect(shapeDataSet_, SIGNAL(currentIndexChanged(int)), this, SLOT(shapeIndexChanged(int)));
}




QString AMSampleCameraBrowser::currentURL()
{
	return currentURL_;
}

AMSampleCamera *AMSampleCameraBrowser::shapeDataSet()
{
	return shapeDataSet_;
}

AMSampleContainer *AMSampleCameraBrowser::sampleContainer()
{
	return sampleContainer_;
}

void AMSampleCameraBrowser::setCurrentURL(QString currentURL)
{
	currentURL_ = currentURL;
}

void AMSampleCameraBrowser::sampleIndexChanged(int index)
{
	int shapeIndex = 0;
	AMShapeData* shapeData = sampleContainer_->sample(index)->sampleShapePositionData();
	shapeIndex = shapeDataSet_->indexOfShape(shapeData);
	emit indexChanged(shapeIndex);
}

void AMSampleCameraBrowser::shapeIndexChanged(int index)
{
	int sampleIndex = -1;
	if(index >= 0 && shapeDataSet()->isValid(index))
	{
		const AMShapeData* shapeData = shapeDataSet_->shapeList().at(index);
		sampleIndex = sampleContainer_->indexOfSample(shapeData);
	}
	emit changeSampleIndex(sampleIndex);
}


