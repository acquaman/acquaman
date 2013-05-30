#include "AMCameraConfigurationView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "ui/AMCameraConfiguration.h"
#include <QDebug>
#include <QVector3D>
#include "dataman/database/AMDbObjectSupport.h"
#include <QSlider>
#include "ui/AMBeamConfigurationView.h"

AMCameraConfigurationView::AMCameraConfigurationView(AMCameraConfiguration *cameraConfiguration, QWidget *parent) :
    QWidget(parent)
{
    if(cameraConfiguration == 0)
        cameraModel_ = new AMCameraConfiguration();
    else
        cameraModel_ = cameraConfiguration;
    AMDatabase *dbSGM = AMDatabase::database("user");
    QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(), "name", "defaultConfiguration");
    if(matchIDs.count() == 0)
    {
        cameraModel_->setName("defaultConfiguration");
        cameraModel_->setCameraDistortion(-0.09);
        cameraModel_->setCameraFocalLength(0.41);
        cameraModel_->setCameraFOV(20);
        cameraModel_->setCameraCenter(QVector3D(0,0,0));
        cameraModel_->setCameraPosition(QVector3D(0,0,1));
        cameraModel_->setCameraRotation(0);
        bool success = cameraModel_->storeToDb(dbSGM);
        if(!success)qDebug()<<"Failed to store item in database";
        else qDebug()<<"successfully stored item in database";
    }
    else
    {
        qDebug()<<"Fetching the default configuration from the database";
        cameraModel_->loadFromDb(dbSGM,matchIDs.first());
//        cameraModel_->setCameraRotation(0);
//        cameraModel_->storeToDb(dbSGM);


    }

    setWindowTitle("Configuration");

    //GUI SETUP
    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->setSpacing(0);
    vbl->setContentsMargins(0,0,0,0);

    QFrame* positionFrame = new QFrame();
    QHBoxLayout* phl = new QHBoxLayout();
    phl->setContentsMargins(12,4,12,4);
    phl->addWidget(new QLabel("Camera Position"));
    phl->addSpacing(20);
    phl->addWidget(new QLabel("x:"));
    phl->addWidget(positionX_ = new QLineEdit());
    phl->addSpacing(20);
    phl->addWidget(new QLabel("y:"));
    phl->addWidget(positionY_ = new QLineEdit());
    phl->addSpacing(20);
    phl->addWidget(new QLabel("z:"));
    phl->addWidget(positionZ_ = new QLineEdit());
    phl->addStretch();
    positionFrame->setLayout(phl);

    QFrame* centerFrame = new QFrame();
    QHBoxLayout* chl = new QHBoxLayout();
    chl->setContentsMargins(12,4,12,4);
    chl->addWidget(new QLabel("Camera Center  "));
    chl->addSpacing(20);
    chl->addWidget(new QLabel("x:"));
    chl->addWidget(centerX_ = new QLineEdit());
    chl->addSpacing(20);
    chl->addWidget(new QLabel("y:"));
    chl->addWidget(centerY_ = new QLineEdit());
    chl->addSpacing(20);
    chl->addWidget(new QLabel("z:"));
    chl->addWidget(centerZ_ = new QLineEdit());
    chl->addStretch();
    centerFrame->setLayout(chl);

    QFrame* fovFrame = new QFrame();
    QHBoxLayout* fhl = new QHBoxLayout();
    fhl->setContentsMargins(12,4,12,4);
    fhl->addWidget(new QLabel("Camera Field of View"));
    fhl->addWidget(cameraFOV_ = new QLineEdit());
    fhl->addSpacing(20);
    fhl->addWidget(new QLabel("Camera Focal Length"));
    fhl->addWidget(cameraFocalLength_ = new QLineEdit());
    fhl->addSpacing(20);
    fhl->addWidget(new QLabel("Lens Distortion"));
    fhl->addWidget(cameraDistortion_ = new QLineEdit());
    fhl->addSpacing(20);
    fhl->addWidget(setButton_ = new QPushButton("Set"));
    fhl->addStretch();
    fovFrame->setLayout(fhl);

    QFrame* rotationFrame = new QFrame();
    QHBoxLayout* rhl = new QHBoxLayout();
    rhl->setContentsMargins(12,4,12,4);
    rhl->addWidget(new QLabel("Camera Rotation"));
    rhl->addWidget(cameraRotation_ = new QLineEdit());
    rhl->addWidget(cameraRotationSlider_ = new QSlider(Qt::Horizontal));
    rhl->addStretch();
    rotationFrame->setLayout(rhl);

    cameraRotationSlider_->setMaximumWidth(80);
    cameraRotationSlider_->setRange(0,80);
    cameraRotationSlider_->setValue(40);

    QFrame* selectionFrame = new QFrame();
    QHBoxLayout* shl = new QHBoxLayout();
    shl->setContentsMargins(12,4,12,4);
    shl->addWidget(configurationSelection_ = new QComboBox());
    shl->addSpacing(20);
    shl->addWidget(configurationName_ = new QLineEdit());
    shl->addSpacing(20);
    shl->addWidget(saveConfiguration_ = new QPushButton("Save Configuration"));
    shl->addSpacing(20);
    shl->addWidget(overwriteButton_ = new QPushButton("Overwrite configuration"));
    shl->addStretch();
    selectionFrame->setLayout(shl);

    beamConfiguration_ = new AMBeamConfigurationView();



    vbl->addWidget(positionFrame);
    vbl->addWidget(centerFrame);
    vbl->addWidget(fovFrame);
    vbl->addWidget(rotationFrame);
    vbl->addWidget(selectionFrame);
    vbl->addWidget(beamConfiguration_);
    setLayout(vbl);

    populateComboBox(1);



    /// connections

    connect(positionX_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionX(QString)));
    connect(positionY_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionY(QString)));
    connect(positionZ_, SIGNAL(textChanged(QString)), this, SLOT(updatePositionZ(QString)));
    connect(centerX_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterX(QString)));
    connect(centerY_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterY(QString)));
    connect(centerZ_, SIGNAL(textChanged(QString)), this, SLOT(updateCenterZ(QString)));
    connect(cameraFOV_, SIGNAL(textChanged(QString)), this, SLOT(updateFOV(QString)));
    connect(cameraFocalLength_, SIGNAL(textChanged(QString)), this, SLOT(updateFocalLength(QString)));
    connect(cameraDistortion_, SIGNAL(textChanged(QString)), this, SLOT(updateDistortion(QString)));
    connect(setButton_, SIGNAL(clicked()), this, SLOT(updateAll()));


    connect(cameraRotation_, SIGNAL(textChanged(QString)), this, SLOT(updateRotation(QString)));
    connect(cameraRotationSlider_, SIGNAL(valueChanged(int)), this, SLOT(updateRotationSlider(int)));

    connect(saveConfiguration_, SIGNAL(clicked()), this, SLOT(saveConfiguration()));
    connect(configurationName_, SIGNAL(textChanged(QString)), this, SLOT(updateName(QString)));
    connect(configurationSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSelection(int)));
     connect(overwriteButton_, SIGNAL(clicked()), this, SLOT(overwriteConfiguration()));

     connect(beamConfiguration_, SIGNAL(oneSelect()), this, SIGNAL(oneSelect()));
     connect(beamConfiguration_, SIGNAL(twoSelect()), this, SIGNAL(twoSelect()));
     connect(this, SIGNAL(beamChanged(QObject*)), beamConfiguration_, SLOT(beamChanged(QObject*)));
     connect(beamConfiguration_, SIGNAL(intersection()), this, SIGNAL(intersection()));

}


double AMCameraConfigurationView::positionX()
{
    return cameraModel_->cameraPositionX();
}

double AMCameraConfigurationView::positionY()
{
    return cameraModel_->cameraPositionY();
}

double AMCameraConfigurationView::positionZ()
{
    return cameraModel_->cameraPositionZ();
}

double AMCameraConfigurationView::centerX()
{
    return cameraModel_->cameraCenterX();
}

double AMCameraConfigurationView::centerY()
{
    return cameraModel_->cameraCenterY();
}

double AMCameraConfigurationView::centerZ()
{
    return cameraModel_->cameraCenterZ();
}

double AMCameraConfigurationView::cameraFOV()
{
    return cameraModel_->cameraFOV();
}

double AMCameraConfigurationView::cameraFocalLength()
{
    return cameraModel_->cameraFocalLength();

}

double AMCameraConfigurationView::cameraDistortion()
{
    return cameraModel_->cameraDistortion();
}

double AMCameraConfigurationView::cameraRotation()
{
    return cameraModel_->cameraRotation();
}

void AMCameraConfigurationView::setPositionX(double x)
{
    cameraModel_->setCameraPositionX(x);
}

void AMCameraConfigurationView::setPositionY(double y)
{
    cameraModel_->setCameraPositionY(y);
}

void AMCameraConfigurationView::setPositionZ(double z)
{
    cameraModel_->setCameraPositionZ(z);
}

void AMCameraConfigurationView::setCenterX(double x)
{
    cameraModel_->setCameraCenterX(x);
}

void AMCameraConfigurationView::setCenterY(double y)
{
    cameraModel_->setCameraCenterY(y);
}

void AMCameraConfigurationView::setCenterZ(double z)
{
    cameraModel_->setCameraCenterZ(z);
}

void AMCameraConfigurationView::setCameraFOV(double fov)
{
    cameraModel_->setCameraFOV(fov);
}

void AMCameraConfigurationView::setCameraFocalLength(double focalLength)
{
    cameraModel_->setCameraFocalLength(focalLength);
}

void AMCameraConfigurationView::setCameraDistortion(double distortion)
{
    cameraModel_->setCameraDistortion(distortion);
}

void AMCameraConfigurationView::setCameraRotation(double rotation)
{
    cameraModel_->setCameraRotation(rotation);
}

void AMCameraConfigurationView::updatePositionX(QString x)
{
    bool* conversionOK = new bool(false);
    double newValue = x.toDouble(conversionOK);
    if(*conversionOK) setPositionX(newValue);
}

void AMCameraConfigurationView::updatePositionY(QString y)
{
    bool* conversionOK = new bool(false);
    double newValue = y.toDouble(conversionOK);
    if(*conversionOK) setPositionY(newValue);
}

void AMCameraConfigurationView::updatePositionZ(QString z)
{
    bool* conversionOK = new bool(false);
    double newValue = z.toDouble(conversionOK);
    if(*conversionOK) setPositionZ(newValue);
}

void AMCameraConfigurationView::updateCenterX(QString x)
{
    bool* conversionOK = new bool(false);
    double newValue = x.toDouble(conversionOK);
    if(*conversionOK) setCenterX(newValue);
}

void AMCameraConfigurationView::updateCenterY(QString y)
{
    bool* conversionOK = new bool(false);
    double newValue = y.toDouble(conversionOK);
    if(*conversionOK) setCenterY(newValue);
}

void AMCameraConfigurationView::updateCenterZ(QString z)
{
    bool* conversionOK = new bool(false);
    double newValue = z.toDouble(conversionOK);
    if(*conversionOK) setCenterZ(newValue);
}

void AMCameraConfigurationView::updateFOV(QString fov)
{
    bool* conversionOK = new bool(false);
    double newValue = fov.toDouble(conversionOK);
    if(*conversionOK) setCameraFOV(newValue);
}

void AMCameraConfigurationView::updateFocalLength(QString focalLength)
{
    bool* conversionOK = new bool(false);
    double newValue = focalLength.toDouble(conversionOK);
    if(*conversionOK) setCameraFocalLength(newValue);
}

void AMCameraConfigurationView::updateDistortion(QString distortion)
{
    bool* conversionOK = new bool(false);
    double newValue = distortion.toDouble(conversionOK);
    if(*conversionOK) setCameraDistortion(newValue);
}

void AMCameraConfigurationView::updateRotation(QString rotation)
{
    bool* conversionOK = new bool(false);
    double newValue = rotation.toDouble(conversionOK);
    if(*conversionOK) setCameraRotation(newValue);
}

void AMCameraConfigurationView::updateRotationSlider(int rotation)
{
    double value = (double)rotation;
    double max = (double)cameraRotationSlider_->maximum();
    double fraction = value/max;
    setCameraRotation(fraction*6.28 - 3.14);
    cameraRotation_->setText(QString::number(cameraRotation()));
    emit(update(cameraModel_));

}

void AMCameraConfigurationView::updateName(QString name)
{
    cameraModel_->setName(name);
}

void AMCameraConfigurationView::updateSelection(int id)
{
    AMDatabase *dataBase = AMDatabase::database("user");
    cameraModel_->loadFromDb(dataBase,id+1);
    updateAll();
}

/// Saves the current configuration as a new database item
/// must have a unique name
void AMCameraConfigurationView::saveConfiguration()
{
    AMDatabase *dataBase = AMDatabase::database("user");
    QList<int> matchIDs = dataBase->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(), "name", cameraModel_->name());
    if(0 == matchIDs.count())
    {
        cameraModel_->dissociateFromDb();
        cameraModel_->storeToDb(dataBase);
        populateComboBox(cameraModel_->id());
    }
    else
    {
        qDebug()<<"Camera model of that name already exists";
    }

}

/// overwrites the values of the current configuration
void AMCameraConfigurationView::overwriteConfiguration()
{
    AMDatabase *dataBase = AMDatabase::database("user");
    cameraModel_->storeToDb(dataBase);
    populateComboBox(cameraModel_->id());
}

/// looks up currently saved configurations, and shows them in the combo box
void AMCameraConfigurationView::populateComboBox(int dbIndex)
{
    configurationSelection_->clear();
    // populate the selection box
     AMDatabase *database = AMDatabase::database("user");
    QList<QVariant> nameList = database->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(),"name");

    QList<QVariant>::iterator i;
    for(i = nameList.begin(); i != nameList.end(); i++)
    {
        configurationSelection_->addItem((*i).toString());
    }
    configurationSelection_->setCurrentIndex(dbIndex-1);
}

/// updates all the fields in the window
void AMCameraConfigurationView::updateAll()
{
    QVector3D position = cameraModel_->cameraPosition();
    QVector3D center = cameraModel_->cameraCenter();
    QString name = cameraModel_->name();
    emit update(cameraModel_);
    positionX_->setText(QString::number(position.x()));
    positionY_->setText(QString::number(position.y()));
    positionZ_->setText(QString::number(position.z()));
    centerX_->setText(QString::number(center.x()));
    centerY_->setText(QString::number(center.y()));
    centerZ_->setText(QString::number(center.z()));
    cameraFOV_->setText(QString::number(cameraFOV()));
    cameraFocalLength_->setText(QString::number(cameraFocalLength()));
    cameraDistortion_->setText(QString::number(cameraDistortion()));
    configurationName_->setText(name);
    cameraRotation_->setText(QString::number(cameraRotation()));

}



