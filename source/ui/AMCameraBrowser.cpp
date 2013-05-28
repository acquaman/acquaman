#include "AMCameraBrowser.h"

#include <QWidget>
#include "AMShapeDataSetView.h"
#include "AMShapeDataSet.h"

#include <QDebug>


AMCameraBrowser::AMCameraBrowser(QObject *parent) :
    QObject(parent)
{
    crosshairLocked_ = false;
    shapeDataSetView_ = new AMShapeDataSetView();
}


void AMCameraBrowser::setCrosshairColor(const QColor &color)
{
    QPen pen = shapeDataSetView_->crosshairPen();
    pen.setColor(color);
    shapeDataSetView_->setCrosshairPen(pen);

//	if(crosshairColorPicker_->color() != color) {
//		crosshairColorPicker_->blockSignals(true);
//		crosshairColorPicker_->setColor(color);
//		crosshairColorPicker_->blockSignals(false);
//	}
}


void AMCameraBrowser::setCrosshairVisible(bool isVisible)
{
    shapeDataSetView_->setCrosshairVisible(isVisible);

//	if(showCrosshairCheckBox_->isChecked() != isVisible) {
//		showCrosshairCheckBox_->blockSignals(true);
//		showCrosshairCheckBox_->setChecked(isVisible);
//		showCrosshairCheckBox_->blockSignals(false);
//	}
}

void AMCameraBrowser::setCrosshairLocked(bool doLock)
{
    crosshairLocked_ = doLock;

//	if(lockCrosshairCheckBox_->isChecked() != doLock) {
//		lockCrosshairCheckBox_->blockSignals(true);
//		lockCrosshairCheckBox_->setChecked(doLock);
//		lockCrosshairCheckBox_->blockSignals(false);
//	}
}

QColor AMCameraBrowser::crosshairColor() const
{
    return shapeDataSetView_->crosshairPen().color();
}

bool AMCameraBrowser::crosshairVisible() const
{
    return shapeDataSetView_->crosshairVisible();
}

QPointF AMCameraBrowser::crosshairPosition() const
{
    return shapeDataSetView_->crosshairPosition();
}

void AMCameraBrowser::setCrosshairPosition(const QPointF &pos) const
{
    shapeDataSetView_->setCrosshairPosition(pos);
}

void AMCameraBrowser::setShapeDataView(AMShapeDataSetView *view)
{
    shapeDataSetView_ = view;
    if(shapeDataSetView_ == 0)
    {
        qDebug()<<"The view set was null";
    }
}

void AMCameraBrowser::setCrosshairLineThickness(int thickness)
{
    QPen pen = shapeDataSetView_->crosshairPen();
    pen.setWidth(thickness);
    shapeDataSetView_->setCrosshairPen(pen);

//	if(crosshairThicknessSlider_->value() != thickness) {
//		crosshairThicknessSlider_->blockSignals(true);
//		crosshairThicknessSlider_->setValue(thickness);
//		crosshairThicknessSlider_->blockSignals(false);
//	}
}


int AMCameraBrowser::crosshairLineThickness() const
{
    return shapeDataSetView_->crosshairPen().width();
}

void AMCameraBrowser::onVideoWidgetDoubleClicked(const QPointF &clickPoint)
{
    if(!crosshairLocked_)
        shapeDataSetView_->setCrosshairPosition(clickPoint);
}
