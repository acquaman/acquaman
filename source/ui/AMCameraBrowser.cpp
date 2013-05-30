#include "AMCameraBrowser.h"

#include <QWidget>
#include "AMShapeDataSetView.h"
#include "AMShapeDataSet.h"

#include <QDebug>


AMCameraBrowser::AMCameraBrowser(QObject *parent) :
    QObject(parent)
{
    shapeDataSet_ = new AMShapeDataSet();
}




QString AMCameraBrowser::currentURL()
{
    return currentURL_;
}

AMShapeDataSet *AMCameraBrowser::shapeDataSet()
{
    return shapeDataSet_;
}

void AMCameraBrowser::setCurrentURL(QString currentURL)
{
    currentURL_ = currentURL;
}


