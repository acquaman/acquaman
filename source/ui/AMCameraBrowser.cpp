#include "AMCameraBrowser.h"

#include <QWidget>
#include "AMShapeDataSetView.h"
#include "AMShapeDataSet.h"
#include "AMSampleContainer.h"

#include <QDebug>


AMCameraBrowser::AMCameraBrowser(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<AMQVector3DVector>();

    shapeDataSet_ = AMShapeDataSet::set();
    sampleContainer_ = new AMSampleContainer();

    connect(shapeDataSet_, SIGNAL(shapesChanged()), sampleContainer_, SLOT(updateSamples()));
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


