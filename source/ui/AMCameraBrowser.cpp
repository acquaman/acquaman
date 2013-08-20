#include "AMCameraBrowser.h"

#include <QWidget>
#include "AMShapeDataSetView.h"
#include "AMShapeDataSet.h"
#include "AMSampleContainer.h"

#include "beamline/AMBeamline.h"

#include <QDebug>


AMCameraBrowser::AMCameraBrowser(QObject *parent) :
	QObject(parent)
{
	qRegisterMetaType<AMQVector3DVector>();

	shapeDataSet_ = AMShapeDataSet::set();
	//sampleContainer_ = new AMSampleContainer();
	sampleContainer_ = AMBeamline::bl()->sampleContainer();

	connect(shapeDataSet_, SIGNAL(shapesChanged()), sampleContainer_, SLOT(updateSamples()));
	connect(this, SIGNAL(indexChanged(int)), shapeDataSet_, SLOT(setCurrentIndex(int)));
	connect(shapeDataSet_, SIGNAL(currentIndexChanged(int)), this, SLOT(shapeIndexChanged(int)));
}




QString AMCameraBrowser::currentURL()
{
	return currentURL_;
}

AMShapeDataSet *AMCameraBrowser::shapeDataSet()
{
	return shapeDataSet_;
}

AMSampleContainer *AMCameraBrowser::sampleContainer()
{
	return sampleContainer_;
}

void AMCameraBrowser::setCurrentURL(QString currentURL)
{
	currentURL_ = currentURL;
}

void AMCameraBrowser::sampleIndexChanged(int index)
{
	int shapeIndex = 0;
	AMShapeData* shapeData = sampleContainer_->sample(index)->sampleShapePositionData();
	shapeIndex = shapeDataSet_->indexOfShape(shapeData);
	emit indexChanged(shapeIndex);
}

void AMCameraBrowser::shapeIndexChanged(int index)
{
	int sampleIndex = -1;
	if(index >= 0 && shapeDataSet()->isValid(index))
	{
		const AMShapeData* shapeData = shapeDataSet_->shapeList().at(index);
		sampleIndex = sampleContainer_->indexOfSample(shapeData);
	}
	emit changeSampleIndex(sampleIndex);
}


