#include "AMShapeDataView.h"

#include "AMShapeData.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <cmath>

#include <QDebug>

AMShapeDataView::AMShapeDataView(AMShapeData *shapeModel, QWidget *parent) :
    QWidget(parent)
{
    if(shapeModel == 0)
        shapeModel_ = new AMShapeData();
    else
        shapeModel_ = shapeModel;

    /// Set up GUI

    QFrame* coordinateFrame= new QFrame();
    QHBoxLayout* ihl = new QHBoxLayout();
    ihl->setContentsMargins(12,4,12,4);
    ihl->addWidget(nameEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(xEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(yEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(zEdit_ = new QLineEdit());
    ihl->addStretch();
    coordinateFrame->setLayout(ihl);

    QFrame* rotationFrame = new QFrame();
    QHBoxLayout *rotationLayout = new QHBoxLayout();
    rotationLayout->setContentsMargins(12,4,12,4);
    rotationLayout->addWidget(tiltEdit_ = new QLineEdit());
    rotationLayout->addSpacing(20);
    rotationLayout->addWidget(yRotationEdit_ = new QLineEdit());
    rotationLayout->addSpacing(20);
    rotationLayout->addWidget(rotationEdit_ = new QLineEdit());
    rotationLayout->addStretch();
    rotationFrame->setLayout(rotationLayout);

    QFrame* sliderFrame = new QFrame();
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->setContentsMargins(12,4,12,4);
    sliderLayout->addWidget(xAxisSlider_ = new QSlider(Qt::Horizontal));
    sliderLayout->addSpacing(20);
    sliderLayout->addWidget(yAxisSlider_ = new QSlider(Qt::Horizontal));
    sliderLayout->addSpacing(20);
    sliderLayout->addWidget(zAxisSlider_ = new QSlider(Qt::Horizontal));
    sliderLayout->addStretch();
    sliderFrame->setLayout(sliderLayout);
    xAxisSlider_->setRange(-250,250);
    xAxisSlider_->setValue(0);
    yAxisSlider_->setRange(-250,250);
    yAxisSlider_->setValue(0);
    zAxisSlider_->setRange(-250,250);
    zAxisSlider_->setValue(0);




    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->addWidget(coordinateFrame);
    infoLayout->addWidget(rotationFrame);
    infoLayout->addWidget(sliderFrame);
    infoLayout->addStretch();

    setLayout(infoLayout);
    setWindowTitle("Shape View");
    /// Make Connections

    connect(nameEdit_, SIGNAL(textEdited(QString)), this, SLOT(nameChanged(QString)));
    connect(tiltEdit_, SIGNAL(textEdited(QString)), this, SLOT(tiltChanged(QString)));
    connect(rotationEdit_, SIGNAL(textEdited(QString)), this, SLOT(rotationChanged(QString)));
    connect(yRotationEdit_, SIGNAL(textEdited(QString)), this, SLOT(yAxisRotationChanged(QString)));

    connect(xEdit_, SIGNAL(textEdited(QString)), this, SLOT(xChanged(QString)));
    connect(yEdit_, SIGNAL(textEdited(QString)), this, SLOT(yChanged(QString)));
    connect(zEdit_, SIGNAL(textEdited(QString)), this, SLOT(zChanged(QString)));

    connect(xAxisSlider_, SIGNAL(valueChanged(int)), this, SLOT(xAxisRotation(int)));
    connect(yAxisSlider_, SIGNAL(valueChanged(int)), this, SLOT(yAxisRotation(int)));
    connect(zAxisSlider_, SIGNAL(valueChanged(int)), this, SLOT(zAxisRotation(int)));

}

void AMShapeDataView::setName(QString name)
{
    nameEdit_->setText(name);
}

void AMShapeDataView::setTilt(QString tilt)
{
    tiltEdit_->setText(tilt);
}

void AMShapeDataView::setX(QString x)
{
    xEdit_->setText(x);
}

void AMShapeDataView::setY(QString y)
{
    yEdit_->setText(y);
}

void AMShapeDataView::setZ(QString z)
{
    zEdit_->setText(z);
}

void AMShapeDataView::setRotation(QString rotation)
{
    rotationEdit_->setText(rotation);
}

void AMShapeDataView::setShapeData(AMShapeData *shapeData)
{
    shapeModel_ = shapeData;
    update();
}

void AMShapeDataView::setYAxisRotation(QString rotation)
{
    yRotationEdit_->setText(rotation);
}

void AMShapeDataView::nameChanged(QString name)
{
    if(isValid())
    {
        shapeModel_->setName(name);
        emit newName();
    }
}

void AMShapeDataView::tiltChanged(QString tilt)
{
    if(isValid())
    {
        shapeModel_->setTilt(tilt.toDouble());
        bool block = true;
        block = xAxisSlider_->blockSignals(block);
        int value = tilt.toDouble()/M_PI*xAxisSlider_->maximum();
        xAxisSlider_->setValue(value);
        xAxisSlider_->blockSignals(block);
        emit updateShapes();
    }
}

void AMShapeDataView::xChanged(QString xString)
{

    qDebug()<<"AMShapeDataView::xChanged";
    if(isValid())
    {
        double x = xString.toDouble();
        QVector3D coordinate = shapeModel_->centerCoordinate();
        coordinate.setX(x);
        shapeModel_->shiftTo(coordinate);
        emit updateShapes();
    }
}

void AMShapeDataView::yChanged(QString yString)
{
    qDebug()<<"AMShapeDataView::yChanged";
    if(isValid())
    {
        double y = yString.toDouble();
        QVector3D coordinate = shapeModel_->centerCoordinate();
        coordinate.setY(y);
        shapeModel_->shiftTo(coordinate);
        emit updateShapes();
    }
}

void AMShapeDataView::zChanged(QString zString)
{
    qDebug()<<"AMShapeDataView::zChanged";
    if(isValid())
    {
        double z = zString.toDouble();
        QVector3D coordinate = shapeModel_->centerCoordinate();
        coordinate.setZ(z);
        shapeModel_->shiftTo(coordinate);
        emit updateShapes();
    }
}


void AMShapeDataView::rotationChanged(QString rotation)
{
    if(isValid())
    {
        shapeModel_->setRotation(rotation.toDouble());
        bool block = true;
        block = zAxisSlider_->blockSignals(block);
        int value = rotation.toDouble()/M_PI*zAxisSlider_->maximum();
        zAxisSlider_->setValue(value);
        zAxisSlider_->blockSignals(block);
        emit updateShapes();
    }
}

void AMShapeDataView::yAxisRotationChanged(QString rotation)
{
    if(isValid())
    {
        shapeModel_->setYAxisRotation(rotation.toDouble());
        bool block = true;
        block = yAxisSlider_->blockSignals(block);
        int value = rotation.toDouble()/M_PI*yAxisSlider_->maximum();
        yAxisSlider_->setValue(value);
        yAxisSlider_->blockSignals(block);
        emit updateShapes();


    }
}

void AMShapeDataView::xAxisRotation(int value)
{
    qDebug()<<"Update x axis rotation";
    double max = xAxisSlider_->maximum();
    double tilt = value/max*M_PI;
    shapeModel_->setTilt(tilt);
    bool block = tiltEdit_->blockSignals(true);
    tiltEdit_->setText(QString::number(tilt));
    tiltEdit_->blockSignals(block);
    emit updateShapes();

}

void AMShapeDataView::yAxisRotation(int value)
{
    double max = yAxisSlider_->maximum();
    double rotation = value/max*M_PI;
    shapeModel_->setYAxisRotation(rotation);
    bool block = yRotationEdit_->blockSignals(true);
    yRotationEdit_->setText(QString::number(rotation));
    yRotationEdit_->blockSignals(block);
    emit updateShapes();
}

void AMShapeDataView::zAxisRotation(int value)
{
    double max = zAxisSlider_->maximum();
    double rotation = value/max*M_PI;
    shapeModel_->setRotation(rotation);
    bool block = rotationEdit_->blockSignals(true);
    rotationEdit_->setText(QString::number(rotation));
    rotationEdit_->blockSignals(block);
    emit updateShapes();
}


void AMShapeDataView::update()
{
    if(isValid())
    {
        nameEdit_->setText(shapeModel_->name());
        tiltEdit_->setText(QString::number(shapeModel_->tilt()));
        QVector3D coordinate = shapeModel_->centerCoordinate();
        xEdit_->setText(QString::number(coordinate.x()));
        yEdit_->setText(QString::number(coordinate.y()));
        zEdit_->setText(QString::number(coordinate.z()));
        rotationEdit_->setText(QString::number(shapeModel_->rotation()));
        yRotationEdit_->setText(QString::number(shapeModel_->yAxisRotation()));
        emit updateShapes();
    }
    else
    {
        nameEdit_->setText("None selected");
    }

}

bool AMShapeDataView::isValid()
{
    if(shapeModel_ == 0)
        return false;
    else if(shapeModel_->count() <= 0)
    {
        return false;
    }
    return true;
}




