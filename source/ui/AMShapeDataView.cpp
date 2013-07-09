#include "AMShapeDataView.h"

#include "AMShapeData.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <QDebug>

AMShapeDataView::AMShapeDataView(AMShapeData *shapeModel, QWidget *parent) :
    QWidget(parent)
{
    if(shapeModel == 0)
        shapeModel_ = new AMShapeData();
    else
        shapeModel_ = shapeModel;

    /// Set up GUI

    QHBoxLayout* ihl = new QHBoxLayout();
    ihl->setContentsMargins(12,4,12,4);
    ihl->addWidget(nameEdit_ = new QLineEdit());
    ihl->addSpacing(20);
    ihl->addWidget(tiltEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(xEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(yEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(zEdit_ = new QLineEdit());
    ihl->addSpacing(10);
    ihl->addWidget(rotationEdit_ = new QLineEdit());
    ihl->addSpacing(20);
    ihl->addWidget(setCoordinate_ = new QPushButton("Set"));
    ihl->addSpacing(20);
    ihl->addWidget(distortionButton_ = new QPushButton("Distortion"));
    ihl->addStretch();
    setLayout(ihl);

    /// Make Connections

    connect(nameEdit_, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));
    connect(tiltEdit_, SIGNAL(textChanged(QString)), this, SLOT(tiltChanged(QString)));
    connect(rotationEdit_, SIGNAL(textChanged(QString)), this, SLOT(rotationChanged(QString)));

    connect(xEdit_, SIGNAL(textChanged(QString)), this, SLOT(xChanged(QString)));
    connect(yEdit_, SIGNAL(textChanged(QString)), this, SLOT(yChanged(QString)));
    connect(zEdit_, SIGNAL(textChanged(QString)), this, SLOT(zChanged(QString)));

    connect(setCoordinate_,SIGNAL(clicked()), this, SIGNAL(setCoordinate()));
    connect(distortionButton_, SIGNAL(clicked()), this, SIGNAL(applyDistortion()));


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

void AMShapeDataView::nameChanged(QString name)
{
    if(isValid())
        shapeModel_->setName(name);
}

void AMShapeDataView::tiltChanged(QString tilt)
{
    if(isValid())
    {
        shapeModel_->setTilt(tilt.toDouble());
        emit updateShapes();
    }
}

void AMShapeDataView::xChanged(QString xString)
{
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
        emit updateShapes();
    }
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
    }

}

bool AMShapeDataView::isValid()
{
    if(shapeModel_ == 0)
        return false;
    else if(shapeModel_->count() <= 0)
    {
        qDebug()<<"AMShapeDataView::isValid: "<<shapeModel_->count();
        return false;
    }
    return true;
}


