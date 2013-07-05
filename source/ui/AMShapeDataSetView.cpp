#include "AMShapeDataSetView.h"
#include <QGraphicsLineItem>
#include <QResizeEvent>
#include <QGraphicsItem>

#include <QMap>
#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include "ui/AMShapeDataSet.h"
#include "ui/AMCameraConfiguration.h"

#include <QSlider>
#include <QCheckBox>
#include "AMColorPickerButton2.h"

#include <QBoxLayout>
#include <QLabel>

#include "AMShapeDataSetGraphicsView.h"

#include "AMShapeDataView.h"
#include <QPushButton>
#include <QLineEdit>

#include "AMCameraConfigurationView.h"
#include "AMBeamConfigurationView.h"
#include <QColor>


#define SAMPLEPOINTS 6



AMShapeDataSetView::AMShapeDataSetView(AMShapeDataSet *shapeModel, QWidget *parent, bool useOpenGlViewport) :
    QWidget(parent)
{

    shapeModel_ = shapeModel;
    shapeView_ = new AMShapeDataView();// start with no shape data, as none has been drawn yet
    shapeScene_ = new AMShapeDataSetGraphicsView(parent, useOpenGlViewport);
    cameraConfiguration_ = new AMCameraConfigurationView(shapeModel_->cameraConfiguration());
    beamConfiguration_ = new AMBeamConfigurationView(shapeModel_->beamConfiguration());

    borderColour_ = QColor(Qt::red);

    activeBorderColour_ = QColor(Qt::blue);


    ///GUI Setup
    QFrame* crosshairFrame = new QFrame();
    QHBoxLayout* chl = new QHBoxLayout();
    chl->setContentsMargins(12,4,12,4);
    chl->addWidget(showCrosshairCheckBox_ = new QCheckBox("Crosshair:"));
    chl->addSpacing(20);
    chl->addWidget(new QLabel("Color:"));
    chl->addWidget(crosshairColorPicker_ = new AMColorPickerButton2(Qt::red));
    chl->addWidget(new QLabel("Line:"));
    chl->addWidget(crosshairThicknessSlider_ = new QSlider(Qt::Horizontal));
    crosshairThicknessSlider_->setMaximumWidth(80);
    crosshairThicknessSlider_->setRange(1,6);
    crosshairThicknessSlider_->setValue(1);
    chl->addSpacing(20);
    chl->addWidget(lockCrosshairCheckBox_ = new QCheckBox("Lock position"));
    chl->addSpacing(20);
    chl->addWidget(enableMotorMovement_ = new QCheckBox("Enable Motor Movement"));
    enableMotorMovement_->setChecked(false);
    chl->addSpacing(20);
    chl->addWidget(enableMotorTracking_ = new QCheckBox("Enable Motor Tracking"));
    enableMotorTracking_->setChecked(false);
    chl->addWidget(configureCameraButton_ = new QPushButton("Calibrate Camera"));
    chl->addSpacing(20);
    chl->addWidget(configurationWindowButton_ = new QPushButton("Configure Settings"));
    chl->addStretch();
    crosshairFrame->setLayout(chl);
    showCrosshairCheckBox_->setChecked(true);

    QFrame* toolFrame  = new QFrame();
    QHBoxLayout* tfl = new QHBoxLayout();
    tfl->setContentsMargins(12,4,12,4);
    tfl->addWidget(drawButton_ = new QPushButton("Draw"));
    tfl->addSpacing(20);
    tfl->addWidget(moveButton_ = new QPushButton("Move"));
    tfl->addSpacing(20);
    tfl->addWidget(editButton_ = new QPushButton("Edit"));
    tfl->addSpacing(20);
    tfl->addWidget(shiftButton_ = new QPushButton("Shift"));
    tfl->addSpacing(20);
    tfl->addWidget(operationButton_ = new QPushButton("Operation"));
    tfl->addSpacing(20);
    tfl->addWidget(groupButton_ = new QPushButton("Group"));
    tfl->addSpacing(20);
    tfl->addWidget(motorXEdit_ = new QLineEdit());
    tfl->addSpacing(10);
    tfl->addWidget(motorYEdit_ = new QLineEdit());
    tfl->addSpacing(10);
    tfl->addWidget(motorZEdit_ = new QLineEdit());
    tfl->addSpacing(10);
    tfl->addWidget(motorREdit_ = new QLineEdit());
    tfl->addSpacing(20);
    tfl->addWidget(setMotorCoordinate_ = new QPushButton("Set"));
    tfl->addStretch(20);
    toolFrame->setLayout(tfl);



    QVBoxLayout *vbl = new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    vbl->addWidget(crosshairFrame);
    vbl->addWidget(shapeScene_);
    vbl->addWidget(shapeView_);
    vbl->addWidget(toolFrame);
    setLayout(vbl);


    configurationWindow_ = new QFrame();
    QVBoxLayout *cvl = new QVBoxLayout();
    cvl->setContentsMargins(0,0,0,0);
    cvl->addWidget(cameraConfiguration_);
    cvl->addWidget(beamConfiguration_);
    configurationWindow_->setLayout(cvl);

    configurationWindow_->setWindowTitle("Configuration");

   // configurationWindow->show();


    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        pointPushButton_[i] = new QPushButton("Set Point " + QString::number(i));
        pointLineEdit_[i] = new QLineEdit("point"+QString::number(i));
        pointLineEdit_[i+SAMPLEPOINTS] = new QLineEdit("point"+QString::number(SAMPLEPOINTS+i));
        coordinateLineEdit_[i] = new QLineEdit("coordinate"+QString::number(i));
        coordinateLineEdit_[i+SAMPLEPOINTS] = new QLineEdit("coordinate"+QString::number(SAMPLEPOINTS+i));
        coordinateLineEdit_[i+2*SAMPLEPOINTS] = new QLineEdit("coordinate"+QString::number(2*SAMPLEPOINTS+i));
    }

    /// camera configuration window (finds camera)
    cameraConfigurationWindow_ = new QFrame();


    QFrame* ccFrame [SAMPLEPOINTS];
    QHBoxLayout* cchl [SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        ccFrame[i] = new QFrame();
        cchl[i] = new QHBoxLayout();
        cchl[i]->setContentsMargins(12,4,12,4);
        cchl[i]->addWidget(pointLineEdit_[2*i]);
        cchl[i]->addSpacing(20);
        cchl[i]->addWidget(pointLineEdit_[2*i+1]);
        cchl[i]->addSpacing(20);
        cchl[i]->addWidget(pointPushButton_[i]);
        cchl[i]->addStretch();
        ccFrame[i]->setLayout(cchl[i]);
    }

    QFrame* coordinateLineFrame [SAMPLEPOINTS];
    QHBoxLayout* clhl [SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        coordinateLineFrame[i] = new QFrame();
        clhl[i] = new QHBoxLayout();
        clhl[i]->setContentsMargins(12,4,12,4);
        for(int j = 0; j < 3; j++)
        {
            clhl[i]->addWidget(coordinateLineEdit_[3*i+j]);
            clhl[i]->addSpacing(20);
        }
        clhl[i]->addStretch();
        coordinateLineFrame[i]->setLayout(clhl[i]);
    }


    QFrame* configurationOptionsFrame = new QFrame();
    QHBoxLayout* cohl = new QHBoxLayout();
    cohl->setContentsMargins(12,4,12,4);
    cohl->addWidget(new QLabel("Use motor coordinates:"));
    cohl->addWidget(motorCoordinateCheckBox_ = new QCheckBox());
    cohl->addSpacing(20);
    cohl->addWidget(deleteCalibrationPoints_ = new QPushButton("Delete calibration points"));
    cohl->addWidget(new QLabel("Use camera Matrix"));
    cohl->addWidget(cameraMatrixCheckBox_ = new QCheckBox());
    cohl->addStretch();
    configurationOptionsFrame->setLayout(cohl);


    QVBoxLayout* ccvl = new QVBoxLayout();
    ccvl->setContentsMargins(0,0,0,0);
    ccvl->addWidget(configurationOptionsFrame);
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        ccvl->addWidget(ccFrame[i]);
        ccvl->addWidget(coordinateLineFrame[i]);
    }
    ccvl->addWidget(startCameraConfiguration_ = new QPushButton("Calibrate Camera"));
    ccvl->addStretch();

    cameraConfigurationWindow_->setLayout(ccvl);
    cameraConfigurationWindow_->setWindowTitle("Calibrate Camera");


    index_ = 0;
    groupRectangleActive_= false;


    QPen pen(borderColour_);

    QBrush brush(QColor(Qt::transparent));

    crosshairXLine_ = shapeScene_->scene()->addLine(0.5,0,0.5,1,pen);
    crosshairYLine_ = shapeScene_->scene()->addLine(0,0.5,0,1,pen);

    QPolygonF polygon(QRectF(5, 5, 0, 0));

    shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));

	reviewCrosshairLinePositions();

	doubleClickInProgress_ = false;

//    connect(shapeScene_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));

	// Leave this up to user-programmers to decide if they want to move the crosshair with a double-click:
	// connect(this, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(setCrosshairPosition(QPointF)));

    //mouse is not held down before signal connected

    // mouse signals to be handled
    connect(shapeScene_, SIGNAL(mousePressed(QPointF)), this, SLOT(mousePressHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseRightClicked(QPointF)), this, SLOT(mouseRightClickHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseLeftReleased(QPointF)), this, SLOT(mouseLeftReleaseHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseRightReleased(QPointF)), this, SLOT(mouseRightReleaseHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(mouseDoubleClickHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseMoved(QPointF)), this, SLOT(mouseMoveHandler(QPointF)));

    connect(this, SIGNAL(currentChanged()), this, SLOT(currentSelectionChanged()));

    //mouse press -
    connect(this, SIGNAL(mousePressed(QPointF)), shapeModel_, SLOT(startRectangle(QPointF)));
    connect(this, SIGNAL(mouseRightClicked(QPointF)), shapeModel_, SLOT(deleteRectangle(QPointF)));
    connect(this, SIGNAL(mouseMovePressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseMoveRightPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseEditPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseEditRightPressed(QPointF)), shapeModel_, SLOT(setZoomPoint(QPointF)));
    connect(this, SIGNAL(mouseEditRightPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseShiftPressed(QPointF)), shapeModel_, SLOT(setShapeVectors(QPointF)));
    connect(this, SIGNAL(mouseShiftRightPressed(QPointF)), shapeModel_, SLOT(setZoomPoint(QPointF)));
    connect(this, SIGNAL(mouseOperationPressed(QPointF,QPointF)), shapeModel_, SLOT(shiftToPoint(QPointF,QPointF)));
    connect(this, SIGNAL(mouseOperationSelect(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseGroupPressed(QPointF)), shapeModel_, SLOT(startGroupRectangle(QPointF)));
    connect(this, SIGNAL(setCoordinate(double,double,double)), shapeModel_, SLOT(setCoordinates(double,double,double)));

    connect(this, SIGNAL(oneSelect()), shapeModel_, SLOT(oneSelect()));
    connect(this, SIGNAL(twoSelect()), shapeModel_, SLOT(twoSelect()));


    /// crosshair bar
    connect(crosshairColorPicker_, SIGNAL(colorChanged(QColor)), this, SLOT(setCrosshairColor(QColor)));
    connect(showCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairVisible(bool)));
    connect(lockCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairLocked(bool)));
    connect(crosshairThicknessSlider_, SIGNAL(valueChanged(int)), this, SLOT(setCrosshairLineThickness(int)));
    connect(enableMotorMovement_, SIGNAL(clicked(bool)), this, SLOT(enableMotorMovement(bool)));
    connect(enableMotorTracking_, SIGNAL(clicked(bool)), this, SLOT(enableMotorTracking(bool)));
    connect(configureCameraButton_, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));


    /// operations bar
    connect(drawButton_, SIGNAL(clicked()), this, SLOT(setDrawMode()));
    connect(moveButton_, SIGNAL(clicked()), this, SLOT(setMoveMode()));
    connect(editButton_, SIGNAL(clicked()), this, SLOT(setEditMode()));
    connect(shiftButton_, SIGNAL(clicked()), this, SLOT(setShiftMode()));
    connect(operationButton_, SIGNAL(clicked()), this, SLOT(setOperationMode()));
    connect(groupButton_, SIGNAL(clicked()), this, SLOT(setGroupMode()));
    connect(setMotorCoordinate_, SIGNAL(clicked()), this, SLOT(setMotorCoordinatePressed()));

    connect(shapeModel_, SIGNAL(motorMoved()), this, SLOT(motorMoved()));

    /// shape view
    connect(shapeView_, SIGNAL(setCoordinate()), this, SLOT(reviewCrosshairLinePositions()));
    connect(shapeView_, SIGNAL(applyDistortion()), this, SLOT(toggleDistortion()));


    connect(cameraConfiguration_, SIGNAL(update(AMCameraConfiguration*)), this, SLOT(setCameraModel(AMCameraConfiguration*)));


    connect(shapeView_, SIGNAL(updateShapes()), this, SLOT(updateCurrentShape()));
    connect(this, SIGNAL(updateShapes(int)), shapeModel_, SLOT(updateShape(int)));

    cameraConfiguration_->updateAll();

    /// beam configuration
    connect(beamConfiguration_, SIGNAL(oneSelect()), this, SIGNAL(oneSelect()));
    connect(beamConfiguration_, SIGNAL(twoSelect()), this, SIGNAL(twoSelect()));
    connect(beamConfiguration_, SIGNAL(intersection()),this, SLOT(intersection()));
    connect(shapeModel_ ,SIGNAL(beamChanged(QObject*)), beamConfiguration_, SLOT(beamChanged(QObject*)));

    /// camera configuration
    connect(pointPushButton_[0], SIGNAL(clicked()), this, SLOT(selectPointOne()));
    connect(pointPushButton_[1], SIGNAL(clicked()), this, SLOT(selectPointTwo()));
    connect(pointPushButton_[2], SIGNAL(clicked()), this, SLOT(selectPointThree()));
    connect(pointPushButton_[3], SIGNAL(clicked()), this, SLOT(selectPointFour()));
    connect(pointPushButton_[4], SIGNAL(clicked()), this, SLOT(selectPointFive()));
    connect(pointPushButton_[5], SIGNAL(clicked()), this, SLOT(selectPointSix()));

    connect(startCameraConfiguration_, SIGNAL(clicked()), this, SLOT(runCameraConfiguration()));

    connect(motorCoordinateCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setUseMotorCoordinate(bool)));
    connect(deleteCalibrationPoints_, SIGNAL(clicked()), this, SLOT(deleteCalibrationPoints()));
    connect(cameraMatrixCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setUseCameraMatrix(bool)));
    connect(cameraMatrixCheckBox_, SIGNAL(clicked(bool)), cameraConfiguration_, SLOT(disableAll(bool)));

    connect(configurationWindowButton_, SIGNAL(clicked()), this, SLOT(showCameraBeamWindow()));



}


void AMShapeDataSetView::reviewCrosshairLinePositions()
{

    QSizeF viewSize = shapeScene_->size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
    QSizeF videoSize = shapeScene_->videoItem()->nativeSize();

    // scale the video size to the view size, obeying the transformation mode
	QSizeF scaledSize = videoSize;
    scaledSize.scale(viewSize, shapeScene_->videoItem()->aspectRatioMode());

    shapeModel_->setViewSize(viewSize);
    shapeModel_->setScaledSize(scaledSize);
	// now, scaledSize will either be:
		// same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
        // One dimension the same, other dimension smalmouseShiftPressedler than viewSize, if Qt::KeepAspectRatio
		// or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
                                       (viewSize.height()-scaledSize.height())/2),
                                        scaledSize);

    // activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

    qreal xSceneCoord = activeRect.left() + shapeModel_->crosshairX()*activeRect.width();
    qreal ySceneCoord = activeRect.top() + shapeModel_->crosshairY()*activeRect.height();



    crosshairXLine_->setLine(xSceneCoord, activeRect.top(), xSceneCoord, activeRect.bottom());
	crosshairYLine_->setLine(activeRect.left(), ySceneCoord, activeRect.right(), ySceneCoord);

    if(index_ < shapeModel_->shapeListLength())
    {
        while(index_<shapeModel_->shapeListLength())
        {
            addNewShape();
        }
    }
    else if(index_ > shapeModel_->shapeListLength())
    {
        while(index_ > shapeModel_->shapeListLength()) deleteShape();
        if(shapeModel_->isValid(current_))
            shapes_[current_]->setPen(QColor(Qt::red));
        current_ = -1;
        shapeModel_->setCurrentIndex(current_);
        emit currentChanged();
    }

    for(int i = 0; i < index_+ 1; i++)
    {
        if(shapes_.contains(i))
        {
            shapes_[i]->setPolygon(shapeModel_->shape(i));
            if(shapeModel_->isBackwards(i))
            {
                shapes_[i]->setBrush(QBrush(QColor(Qt::green)));
            }
            else
            {
                shapes_[i]->setBrush(QBrush(QColor(Qt::transparent)));
            }

        }
        else
            qDebug()<<"Missing shape"<<i;
    }

    if(groupRectangleActive_)
    {
        qDebug()<<"Changing the group rectangle";
        groupRectangle_->setPolygon(shapeModel_->groupRectangle());
    }

   // print the intersection shapes
            intersection();




}

void AMShapeDataSetView::setCrosshairPosition(const QPointF &pos)
{
    shapeModel_->setCrosshair(pos);
}


QPen AMShapeDataSetView::crosshairPen() const
{
	return crosshairXLine_->pen();
}


QPointF AMShapeDataSetView::crosshairPosition() const
{
    return shapeModel_->crosshair();
}

bool AMShapeDataSetView::crosshairVisible() const
{
    return crosshairXLine_->isVisible();
}

bool AMShapeDataSetView::crosshairLocked()
{
    return shapeModel_->crosshairLocked();
}

void AMShapeDataSetView::setDrawMode()
{
    mode_ = DRAW;
}

void AMShapeDataSetView::setMoveMode()
{
    mode_ = MOVE;
}

void AMShapeDataSetView::setEditMode()
{
    mode_ = EDIT;
}

void AMShapeDataSetView::setShiftMode()
{
    mode_ = SHIFT;
}

void AMShapeDataSetView::setOperationMode()
{
    mode_ = OPERATION;

//    configurationWindow_->show();

}

void AMShapeDataSetView::setGroupMode()
{
    mode_ = GROUP;
//    qDebug()<<"AMShapeDataSetView::setGroupMode - calling findcamera";
//    QPointF points[6];
//    points[0]=QPointF(0.5,0.5);
//    points[1]=QPointF(0.6,1.1);
//    points[2]=QPointF(1,0.5);
//    points[3]=QPointF(0.5,0.3);
//    points[4]=QPointF(0.8,0.5);
//    points[5]=QPointF(0.6,0.6);
//    QVector3D coordinate[6];
//    coordinate[0] = QVector3D(12,51,11.8);
//    coordinate[1] = QVector3D(0.2,-1.2,0);
//    coordinate[2] = QVector3D(1,0,0);
//    coordinate[3] = QVector3D(0,0.6,-1);
//    coordinate[4] = QVector3D(0.9,0,-1);
//    coordinate[5] = QVector3D(0.3,-0.3,-1);
////    coordinate[0] = QVector3D(11,-12,28);
////    coordinate[1] = QVector3D(0,-1.2,0.2);
////    coordinate[2] = QVector3D(0,0,1);
////    coordinate[3] = QVector3D(1,0.6,0);
////    coordinate[4] = QVector3D(1,0,0.9);
////    coordinate[5] = QVector3D(1,-0.3,0.3);
//    shapeModel_->findCamera(points,coordinate);
//    reviewCrosshairLinePositions();

}

void AMShapeDataSetView::setConfigurationMode()
{
    mode_ = CONFIGURE;

}

void AMShapeDataSetView::setMotorCoordinatePressed()
{
    double x = (motorXEdit_->text().toDouble());
    double y = (motorYEdit_->text().toDouble());
    double z = (motorZEdit_->text().toDouble());
    double r = (motorREdit_->text().toDouble());
    setMotorCoordinate(x,y,z,r);
}

void AMShapeDataSetView::enableMotorMovement(bool isEnabled)
{
    if(isEnabled != enableMotorMovement_->isChecked())
    {
        qDebug()<<"enableMotorMovement has incorrect value";
        enableMotorMovement_->setChecked(false);
        isEnabled = false;
    }
    shapeModel_->enableMotorMovement(isEnabled);

}

void AMShapeDataSetView::enableMotorTracking(bool isEnabled)
{
    shapeModel_->enableMotorTracking(isEnabled);
}

void AMShapeDataSetView::showConfigurationWindow()
{
    cameraConfigurationWindow_->show();
}


void AMShapeDataSetView::setPoint(QPointF position, int pointToSelect)
{
    pointLineEdit_[pointToSelect*2]->setText(QString::number(position.x()));
    pointLineEdit_[pointToSelect*2 + 1]->setText(QString::number(position.y()));
    if(useMotorCoordinate_)
    {
        coordinateLineEdit_[pointToSelect*3]->setText(QString::number(shapeModel_->motorX()));
        coordinateLineEdit_[pointToSelect*3 + 1]->setText(QString::number(shapeModel_->motorY()));
        coordinateLineEdit_[pointToSelect*3 + 2]->setText(QString::number(shapeModel_->motorZ()));
    }
}

void AMShapeDataSetView::selectPoint(int point)
{
    qDebug()<<"AMShapeDataSetView::selectPoint - point is"<<point;
    setConfigurationMode();
    pointToSelect_ = point;
}

void AMShapeDataSetView::selectPointOne()
{
    setConfigurationMode();
    pointToSelect_ = 0;
}



void AMShapeDataSetView::selectPointTwo()
{
    setConfigurationMode();
    pointToSelect_ = 1;
}

void AMShapeDataSetView::selectPointThree()
{
    setConfigurationMode();
    pointToSelect_ = 2;
}

void AMShapeDataSetView::selectPointFour()
{
    setConfigurationMode();
    pointToSelect_ = 3;
}

void AMShapeDataSetView::selectPointFive()
{
    setConfigurationMode();
    pointToSelect_ = 4;
}

void AMShapeDataSetView::selectPointSix()
{
    setConfigurationMode();
    pointToSelect_ = 5;
}

void AMShapeDataSetView::runCameraConfiguration()
{
    qDebug()<<"Running camera Configuration";
    QVector3D coordinates [SAMPLEPOINTS];
    QPointF points [SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        points[i] = QPointF(pointLineEdit_[2*i]->text().toDouble(),pointLineEdit_[2*i+1]->text().toDouble());
        coordinates [i] = QVector3D(coordinateLineEdit_[3*i]->text().toDouble(),coordinateLineEdit_[3*i+1]->text().toDouble(),coordinateLineEdit_[3*i+2]->text().toDouble());
    }
    for(int i = 1; i < SAMPLEPOINTS; i++)
    {
        coordinates[i] -= coordinates[0];
    }
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        qDebug()<<"point:"<<points[i]<<"coordinate:"<<coordinates[i];
    }
    shapeModel_->findCamera(points,coordinates);
    cameraConfiguration_->updateAll();
    reviewCrosshairLinePositions();



}

void AMShapeDataSetView::deleteCalibrationPoints()
{
    shapeModel_->deleteCalibrationPoints();
    reviewCrosshairLinePositions();
}




void AMShapeDataSetView::setMotorCoordinate(double x, double y, double z, double r)
{
    shapeModel_->setMotorCoordinate(x,y,z,r);
    emit currentChanged();
    reviewCrosshairLinePositions();
}

double AMShapeDataSetView::motorRotation()
{
    return (shapeModel_->motorRotation());
}

double AMShapeDataSetView::motorX()
{
    return shapeModel_->motorX();
}

double AMShapeDataSetView::motorY()
{
    return shapeModel_->motorY();
}

double AMShapeDataSetView::motorZ()
{
    return shapeModel_->motorZ();
}

QString AMShapeDataSetView::currentName()
{
    return shapeModel_->currentName();
}

void AMShapeDataSetView::setCurrentName(QString name)
{
   shapeModel_->setCurrentName(name);
}

QString AMShapeDataSetView::currentInfo()
{
    return shapeModel_->currentInfo();
}

void AMShapeDataSetView::setCurrentInfo(QString info)
{
    shapeModel_->setCurrentInfo(info);
}

int AMShapeDataSetView::currentIndex()
{
    return shapeModel_->currentIndex();
}

double AMShapeDataSetView::xCoordinate()
{
    double coordinate = shapeModel_->currentCoordinate().x();
    return coordinate;
}

double AMShapeDataSetView::yCoordinate()
{
    return shapeModel_->currentCoordinate().y();
}

double AMShapeDataSetView::zCoordinate()
{

    return shapeModel_->currentCoordinate().z();
}

double AMShapeDataSetView::rotation()
{
    return shapeModel_->rotation();
}

double AMShapeDataSetView::tilt()
{
    return shapeModel_->tilt();
}

void AMShapeDataSetView::setX(QString x)
{

   QVector3D currentCoordinate = shapeModel_->currentCoordinate();
   double y = currentCoordinate.y();
   double z = currentCoordinate.z();
   shapeModel_->setCoordinates(x.toDouble(),y,z);
}

void AMShapeDataSetView::setY(QString y)
{
    QVector3D currentCoordinate = shapeModel_->currentCoordinate();
    double x = currentCoordinate.x();
    double z = currentCoordinate.z();
    shapeModel_->setCoordinates(x,y.toDouble(),z);
}

void AMShapeDataSetView::setZ(QString z)
{
    QVector3D currentCoordinate = shapeModel_->currentCoordinate();
    double y = currentCoordinate.y();
    double x = currentCoordinate.x();
    shapeModel_->setCoordinates(x,y,z.toDouble());
}

void AMShapeDataSetView::setRotation(QString rotation)
{
    shapeModel_->setRotation(rotation.toDouble());
}

void AMShapeDataSetView::motorMoved()
{
    motorXEdit_->setText(QString::number(motorX()));
    motorYEdit_->setText(QString::number(motorY()));
    motorZEdit_->setText(QString::number(motorZ()));
    motorREdit_->setText(QString::number(motorRotation()));
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setUseMotorCoordinate(bool use)
{
    useMotorCoordinate_ = use;
}

void AMShapeDataSetView::setUseCameraMatrix(bool use)
{
    shapeModel_->setUseCameraMatrix(use);
}

void AMShapeDataSetView::showCameraBeamWindow()
{
    configurationWindow_->show();
}

void AMShapeDataSetView::setTilt(QString tilt)
{
    double newTilt = tilt.toDouble();
    shapeModel_->setTilt(newTilt);
}

void AMShapeDataSetView::moveCurrentToCoordinate()
{
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::toggleDistortion()
{
    shapeModel_->toggleDistortion();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setMedia(QMediaContent url)
{
    shapeScene_->mediaPlayer()->setMedia(url);
}

void AMShapeDataSetView::play()
{
    shapeScene_->mediaPlayer()->play();
}

QMediaPlayer* AMShapeDataSetView::mediaPlayer()
{
    return shapeScene_->mediaPlayer();
}

void AMShapeDataSetView::setCrosshairColor(const QColor &color)
{
    QPen pen = crosshairPen();
    pen.setColor(color);
   setCrosshairPen(pen);

    if(crosshairColorPicker_->color() != color)
    {
        crosshairColorPicker_->blockSignals(true);
        crosshairColorPicker_->setColor(color);
        crosshairColorPicker_->blockSignals(false);
    }
}

void AMShapeDataSetView::setCrosshairLineThickness(int thickness)
{
        QPen pen = crosshairPen();
        pen.setWidth(thickness);
        setCrosshairPen(pen);

        if(crosshairThicknessSlider_->value() != thickness)
        {
            crosshairThicknessSlider_->blockSignals(true);
            crosshairThicknessSlider_->setValue(thickness);
            crosshairThicknessSlider_->blockSignals(false);
        }

}

void AMShapeDataSetView::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMShapeDataSetView::setCrosshairVisible(bool crosshairVisible)
{
	crosshairXLine_->setVisible(crosshairVisible);
    crosshairYLine_->setVisible(crosshairVisible);

        if(showCrosshairCheckBox_->isChecked() != crosshairVisible)
        {
            showCrosshairCheckBox_->blockSignals(true);
            showCrosshairCheckBox_->setChecked(crosshairVisible);
            showCrosshairCheckBox_->blockSignals(false);
        }
}

void AMShapeDataSetView::setCrosshairLocked(bool doLock)
{
        shapeModel_->setCrosshairLocked(doLock);

        if(lockCrosshairCheckBox_->isChecked() != doLock)
        {
            lockCrosshairCheckBox_->blockSignals(true);
            lockCrosshairCheckBox_->setChecked(doLock);
            lockCrosshairCheckBox_->blockSignals(false);
        }
}

void AMShapeDataSetView::setCameraModel(AMCameraConfiguration *model)
{
    shapeModel_->setCameraModel(model);
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::intersection()
{
    if(shapeModel_->findIntersections())
    {
        if(!intersections_.isEmpty()) clearIntersections();
        createIntersectionShapes(shapeModel_->intersections());
    }
}

void AMShapeDataSetView::updateCurrentShape()
{
    emit updateShapes(current_);
}

void AMShapeDataSetView::createIntersectionShapes(QVector<QPolygonF> shapes)
{
    intersections_.clear();
    for(int i =0; !shapes.isEmpty(); i++)
    {
        QPen pen(QColor(Qt::yellow));
        QBrush brush(QColor(Qt::yellow));
        QPolygonF polygon(QRectF(5,5,20,20));
        intersections_<<shapeScene_->scene()->addPolygon(polygon,pen,brush);
        intersections_[i]->setPolygon(shapes.first());
        shapes.remove(0);
    }
}

void AMShapeDataSetView::clearIntersections()
{
    while(!intersections_.isEmpty())
    {
        shapeScene_->scene()->removeItem(intersections_.first());
        intersections_.remove(0);
    }
}

void AMShapeDataSetView::resizeEvent(QResizeEvent *event)
{

    shapeScene_->resizeEvent(event);

	reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mousePressHandler(QPointF position)
{
    if(mode_ == DRAW)
    {
        emit mousePressed((position));
        connect(this,SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishRectangle(QPointF)));

    }
    else if(mode_ == MOVE)
    {
        emit mouseMovePressed((position));
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMovedMoveMode(QPointF)), shapeModel_,SLOT(moveCurrentShape(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }

    else if(mode_ == EDIT)
    {
        emit mouseEditPressed((position));
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishRectangle(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }


    else if(mode_ == SHIFT)
    {
        emit mouseShiftPressed((position));
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(moveAllShapes(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }

    else if (mode_ == OPERATION)
    {
        emit mouseOperationSelect((position));
        emit mouseOperationPressed((position),shapeModel_->crosshair());
        currentSelectionChanged();
    }
    else if (mode_ == GROUP)
    {
        emit mouseGroupPressed((position));
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishGroupRectangle(QPointF)));
        createGroupRectangle();

    }
    else if (mode_ == CONFIGURE)
    {
        setPoint(position, pointToSelect_);
    }

    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mouseRightClickHandler(QPointF position)
{
        if (mode_ == MOVE)
        {
            emit mouseMoveRightPressed(position);
            currentSelectionChanged();
            connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(rotateRectangle(QPointF)));
            connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
        }

        else if (mode_ == EDIT)
        {
            emit mouseEditRightPressed(position);
            currentSelectionChanged();
            connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(zoomShape(QPointF)));
            connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
        }

        else if (mode_ == SHIFT)
        {
            emit mouseShiftRightPressed((position));
            connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(zoomAllShapes(QPointF)));
            connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
        }

        else if ( mode_ == GROUP)
        {
            shapeModel_->placeGrid((position));
            reviewCrosshairLinePositions();
            currentSelectionChanged();

        }
        else emit mouseRightClicked((position));

        reviewCrosshairLinePositions();

}

void AMShapeDataSetView::mouseLeftReleaseHandler(QPointF position)
{
        if(doubleClickInProgress_)
        {
            emit mouseDoubleClicked((position));
			doubleClickInProgress_ = false;
		}
        else
        {
            emit mouseReleased((position));
		}

        disconnect(shapeModel_, SLOT(finishRectangle(QPointF)));
        disconnect(shapeModel_, SLOT(moveCurrentShape(QPointF)));
        disconnect(this, SIGNAL(currentChanged()));
        disconnect(shapeModel_, SLOT(moveAllShapes(QPointF)));
        disconnect(shapeModel_, SLOT(finishGroupRectangle(QPointF)));

        if(groupRectangleActive_)
        {
            destroyGroupRectangle();
        }

        reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mouseRightReleaseHandler(QPointF position)
{
    emit mouseReleased((position));
    disconnect(shapeModel_, SLOT(zoomAllShapes(QPointF)));
    disconnect(this, SIGNAL(currentChanged()));
    disconnect(shapeModel_, SLOT(rotateRectangle(QPointF)));
    disconnect(shapeModel_, SLOT(zoomShape(QPointF)));
    reviewCrosshairLinePositions();
}



void AMShapeDataSetView::mouseDoubleClickHandler(QPointF position)
{
		doubleClickInProgress_ = true;
        if(!crosshairLocked())
        {
            setCrosshairPosition(position);
        }
}

void AMShapeDataSetView::mouseMoveHandler(QPointF position)
{

    emit mouseMoved((position));
    if(mode_ == MOVE)
    {
        emit mouseMovedMoveMode((position));
    }
    else if(mode_ == DRAW)
    {
        currentSelectionChanged();
    }

     reviewCrosshairLinePositions();
}






/// append a new rectangle to the current list, add it to the scene
void AMShapeDataSetView::addNewShape()
{
    index_++;
    QPen pen(borderColour_);
    QBrush brush(QColor(Qt::transparent));
    QPolygonF polygon(QRectF(5,5,20,20));
    shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));
}

/// Remove a rectangle from the scene
void AMShapeDataSetView::deleteShape()
{
    QGraphicsPolygonItem* polygon = shapes_[index_];
    shapeScene_->scene()->removeItem(shapes_[index_]);
    shapes_.remove(index_);
    index_--;
    delete polygon;
}

/// change the currently selected item, outline it in blue?
void AMShapeDataSetView::currentSelectionChanged()
{
//    emit currentChanged();
    shapeView_->setShapeData(shapeModel_->currentShape());
    motorREdit_->setText(QString::number(motorRotation()));
    motorXEdit_->setText(QString::number(motorX()));
    motorYEdit_->setText(QString::number(motorY()));
    motorZEdit_->setText(QString::number(motorZ()));


    if(shapeModel_->isValid(current_))
        shapes_[current_]->setPen(borderColour_);
    current_ = shapeModel_->currentIndex();
    if(shapeModel_->isValid(current_))
    {
        shapes_[current_]->setPen(activeBorderColour_);
    }
}

void AMShapeDataSetView::createGroupRectangle()
{
    QBrush penBrush(QColor(Qt::magenta));
    QPen pen(penBrush,1,Qt::DotLine);
    QBrush brush(Qt::transparent);
    groupRectangle_ = shapeScene_->scene()->addPolygon(QPolygonF(QRectF(5,5,20,20)),pen,brush);
    groupRectangleActive_ = true;

}

void AMShapeDataSetView::destroyGroupRectangle()
{
    shapeScene_->scene()->removeItem(groupRectangle_);
    groupRectangleActive_ = false;
}
