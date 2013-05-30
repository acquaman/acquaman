#include "AMCameraBrowser.h"

#include <QWidget>
#include "AMShapeDataSetView.h"
#include "AMShapeDataSet.h"

#include <QDebug>


AMCameraBrowser::AMCameraBrowser(QObject *parent) :
    QObject(parent)
{
    shapeDataSet_ = new AMShapeDataSet();
    crosshairLocked_ = false;
    shapeDataSetView_ = new AMShapeDataSetView(shapeDataSet_);
}

///// \todo move to shapeDataset
//void AMCameraBrowser::setCrosshairColor(const QColor &color)
//{
//    QPen pen = shapeDataSetView_->crosshairPen();
//    pen.setColor(color);
//    shapeDataSetView_->setCrosshairPen(pen);

////	if(crosshairColorPicker_->color() != color) {
////		crosshairColorPicker_->blockSignals(true);
////		crosshairColorPicker_->setColor(color);
////		crosshairColorPicker_->blockSignals(false);
////	}
//}

///// \todo move to shapeDataset
//void AMCameraBrowser::setCrosshairVisible(bool isVisible)
//{
//    shapeDataSetView_->setCrosshairVisible(isVisible);

////	if(showCrosshairCheckBox_->isChecked() != isVisible) {
////		showCrosshairCheckBox_->blockSignals(true);
////		showCrosshairCheckBox_->setChecked(isVisible);
////		showCrosshairCheckBox_->blockSignals(false);
////	}
//}

///// \todo move to shapeDataset
//void AMCameraBrowser::setCrosshairLocked(bool doLock)
//{
//    crosshairLocked_ = doLock;

////	if(lockCrosshairCheckBox_->isChecked() != doLock) {
////		lockCrosshairCheckBox_->blockSignals(true);
////		lockCrosshairCheckBox_->setChecked(doLock);
////		lockCrosshairCheckBox_->blockSignals(false);
////	}
//}

/// \todo move to shapeDataset
QColor AMCameraBrowser::crosshairColor() const
{
    return shapeDataSetView_->crosshairPen().color();
}

/// \todo move to shapeDataset
bool AMCameraBrowser::crosshairVisible() const
{
    return shapeDataSetView_->crosshairVisible();
}

/// \todo move to shapeDataset
QPointF AMCameraBrowser::crosshairPosition() const
{
    return shapeDataSetView_->crosshairPosition();
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

/// \todo move to shapeDataset
void AMCameraBrowser::setCrosshairPosition(const QPointF &pos) const
{
    shapeDataSetView_->setCrosshairPosition(pos);
}

/// \todo will be unnecessary, once all the crosshair stuff is gone
void AMCameraBrowser::setShapeDataView(AMShapeDataSetView *view)
{
    shapeDataSetView_ = view;
    if(shapeDataSetView_ == 0)
    {
        qDebug()<<"The view set was null";
    }
}


///// \todo move to shapeDataset
//void AMCameraBrowser::setCrosshairLineThickness(int thickness)
//{
//    QPen pen = shapeDataSetView_->crosshairPen();
//    pen.setWidth(thickness);
//    shapeDataSetView_->setCrosshairPen(pen);

////	if(crosshairThicknessSlider_->value() != thickness) {
////		crosshairThicknessSlider_->blockSignals(true);
////		crosshairThicknessSlider_->setValue(thickness);
////		crosshairThicknessSlider_->blockSignals(false);
////	}
//}


/// \todo move to shapeDataset
int AMCameraBrowser::crosshairLineThickness() const
{
    return shapeDataSetView_->crosshairPen().width();
}

/// \todo move to shapeDataset
void AMCameraBrowser::onVideoWidgetDoubleClicked(const QPointF &clickPoint)
{
    if(!crosshairLocked_)
        shapeDataSetView_->setCrosshairPosition(clickPoint);
}
