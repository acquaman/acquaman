#include "AMShapeDataView.h"

#include "AMShapeData.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>

AMShapeDataView::AMShapeDataView(QWidget *parent) :
    QWidget(parent)
{

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

    connect(nameEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(nameChanged(QString)));
    connect(tiltEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(tiltChanged(QString)));
    connect(xEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(xChanged(QString)));
    connect(yEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(yChanged(QString)));
    connect(zEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(zChanged(QString)));
    connect(rotationEdit_, SIGNAL(textChanged(QString)), this, SIGNAL(rotationChanged(QString)));
    connect(setCoordinate_, SIGNAL(clicked()), this, SIGNAL(setCoordinate()));
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
