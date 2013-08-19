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

#include <QTimer>
#include <QTextDocument>

#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <QCompleter>
#include <QStringListModel>

#include "GraphicsTextItem.h"
#include <QMediaPlayer>
#include <QPainterPath>

#include "AMCameraConfigurationWizard.h"
#include "AMBeamConfigurationWizard.h"
#include "AMSamplePlateWizard.h"

#include <limits>

#include "AMShapeDataListView.h"



#define SAMPLEPOINTS 6

#define DELAY 1000



AMShapeDataSetView::AMShapeDataSetView(AMShapeDataSet *shapeModel, QWidget *parent, bool useOpenGlViewport) :
    QWidget(parent)
{
    qDebug()<<"Registering type";
    shapeModel_ = shapeModel;
    shapeView_ = AMShapeDataView::shapeView();// start with no shape data, as none has been drawn yet
    shapeScene_ = new AMShapeDataSetGraphicsView(parent, useOpenGlViewport);
    cameraConfiguration_ = new AMCameraConfigurationView(shapeModel_->cameraConfiguration());
    beamConfiguration_ = new AMBeamConfigurationView(shapeModel_->beamConfiguration());

    updateTracker_ = -1;
    samplePlateMovement_ = 0;

    borderColour_ = QColor(Qt::red);

    activeBorderColour_ = QColor(Qt::blue);

    pressTimer_ = new QTimer();
    pressTimer_->setInterval(DELAY);
    pressTimer_->setSingleShot(true);

    currentView_ = NAME;
    mode_ = DRAW;


    cameraWizard_ = new AMCameraConfigurationWizard();
    AMShapeDataSetGraphicsView* view = new AMShapeDataSetGraphicsView(parent, useOpenGlViewport);
    view->setScene(shapeScene_->scene());
    cameraWizard_->setView(view);

    beamWizard_ = new AMBeamConfigurationWizard();
    beamWizard_->setView(view);

    samplePlateWizard_ = new AMSamplePlateWizard();
    samplePlateWizard_->setView(view);

    showBeamOutline_ = true;

//    shapeDataListView_ = new AMShapeDataListView();
//    setListViewModel();
//    shapeDataListView_->show();

    index_ = 0;
    groupRectangleActive_= false;

    QPen pen(borderColour_);

    QBrush brush(QColor(Qt::transparent));

    crosshairXLine_ = shapeScene_->scene()->addLine(0.5,0,0.5,1,pen);
    crosshairYLine_ = shapeScene_->scene()->addLine(0,0.5,0,1,pen);
    crosshairXLine_->setVisible(true);
    crosshairYLine_->setVisible(true);

    QPolygonF polygon(QRectF(5, 5, 0, 0));

    shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));

    GraphicsTextItem* newItem = new GraphicsTextItem();
    shapeScene_->scene()->addItem(newItem);
    textItems_<<newItem;


    doubleClickInProgress_ = false;

    QPainterPath* path = new QPainterPath();
    path->addPolygon(polygon);
    currentShape_ = shapeScene_->scene()->addPath(*path,pen,brush);

    setGUI();

    QStringList wordList;
    wordList_ = new QStringListModel(wordList);
    autoCompleter_ = new QCompleter();
    autoCompleter_->setModel(wordList_);
    autoCompleter_->setCaseSensitivity(Qt::CaseInsensitive);
    autoCompleteBox_->setCompleter(autoCompleter_);


    makeConnections();



    /// This fixes the problem with with different views flashing or going white
    /// when they have QGraphicsTextItems or GraphicsTextItems in them.
    /// No real reason why this works, it just does.
    /// see AMGraphicsViewWizard::setView (AMGraphicsViewWizard.cpp) for a full description
    QGraphicsTextItem* textFixItem = new QGraphicsTextItem("Fix");
    textFixItem->setZValue(INT_MAX);
    textFixItem->setPos(-1*textFixItem->boundingRect().width(), -1*textFixItem->boundingRect().height());
    shapeScene_->scene()->addItem(textFixItem);

    reviewCrosshairLinePositions();



}

AMShapeDataSetView::~AMShapeDataSetView()
{
    delete beamWizard_;
    delete cameraWizard_;
    delete samplePlateWizard_;
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
        if(shapes_.contains(i) && shapeModel_->isValid(i))
        {
            shapes_[i]->setPolygon(shapeModel_->shape(i));
            shapes_[i]->setToolTip(shapeModel_->name(i));
            shapes_[i]->setVisible(shapeModel_->visible(i));
            if(!wordList_->stringList().contains(shapeModel_->otherData(i)))
            {
                wordList_->insertRow(wordList_->rowCount());
                wordList_->setData(wordList_->index(wordList_->rowCount()-1),shapeModel_->otherData(i));
            }

            if(shapeModel_->isBackwards(i))
            {
                shapes_[i]->setBrush(QBrush(QColor(Qt::transparent)));
            }
            else
            {
                shapes_[i]->setBrush(QBrush(QColor(Qt::transparent)));
            }


            if(textItems_.count() < i+1) textItems_<< new GraphicsTextItem();
            if(shapeModel_->isValid(i))
            {
                QPointF point(0,textItems_[i]->boundingRect().height());
                QPointF first = shapes_[i]->polygon().first();
                textItems_[i]->setPos(first-point);
                textItems_[i]->setDefaultTextColor(Qt::red);
                if(i == current_)textItems_[i]->setDefaultTextColor(Qt::blue);
                if(currentView_ == NAME)
                    textItems_[i]->setPlainText(shapeModel_->name(i));
                else if(currentView_ == DATA)
                    textItems_[i]->setPlainText(shapeModel_->otherData(i));
                else if(currentView_ == ID)
                    textItems_[i]->setPlainText(QString::number(shapeModel_->idNumber(i)));

                if(currentView_ == HIDE)
                    textItems_[i]->hide();
                else /*if(!textItems_[i]->isVisible())*/
                    textItems_[i]->setVisible(shapes_[i]->isVisible());
            }
            else
            {
                textItems_[i]->setPlainText("");
            }

        }
        else
            qDebug()<<"Missing shape"<<i;
    }

    if(groupRectangleActive_)
    {
        groupRectangle_->setPolygon(shapeModel_->groupRectangle());
    }

   // print the intersection shapes
   intersection();


   // show the polygon currently being drawn
   QPolygonF currentPolygon = shapeModel_->currentPolygon();
   QPainterPath* path = new QPainterPath();
   path->addPolygon(currentPolygon);
   currentShape_->setPath(*path);

    shapeModel_->updateView();

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
    if(mode_ == DRAW)
    {
        setMultiDrawMode();
        markAction_->setToolTip("Click to place a vertex, double click for last vertex.");
        return;
    }
    mode_ = DRAW;
    markAction_->setToolTip("Press twice for polygon marking.");
//    pressTimer_->start();
    emit modeChange();
}

void AMShapeDataSetView::setMoveMode()
{
    mode_ = MOVE;
    emit modeChange();
}

void AMShapeDataSetView::setEditMode()
{
    mode_ = EDIT;
    emit modeChange();
}

void AMShapeDataSetView::setShiftMode()
{
    mode_ = SHIFT;
    emit modeChange();

}

void AMShapeDataSetView::setOperationMode()
{
    mode_ = OPERATION;
    emit modeChange();
}

void AMShapeDataSetView::setGroupMode()
{
    mode_ = GROUP;
    emit modeChange();

}

void AMShapeDataSetView::setConfigurationMode()
{
    mode_ = CONFIGURE;
    emit modeChange();

}

void AMShapeDataSetView::setMultiDrawMode()
{
//    if(drawButton_->isDown() || )
    mode_ = MULTIDRAW;
    emit modeChange();
    emit enterMultiDraw();
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
    QVector3D coordinates [SAMPLEPOINTS];
    QPointF points [SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        points[i] = QPointF(pointLineEdit_[2*i]->text().toDouble(),pointLineEdit_[2*i+1]->text().toDouble());
        coordinates [i] = QVector3D(coordinateLineEdit_[3*i]->text().toDouble(),coordinateLineEdit_[3*i+1]->text().toDouble(),coordinateLineEdit_[3*i+2]->text().toDouble());
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

void AMShapeDataSetView::stopTimer()
{
    pressTimer_->stop();
}

void AMShapeDataSetView::changeDrawButtonText()
{
    if(mode_ == MULTIDRAW)
    {
        markAction_->setText("Mark Polygon");
    }
    else
    {
        markAction_->setText("Mark");
    }
}

void AMShapeDataSetView::updateItemName(int index)
{
    if(shapeModel_->isValid(index))
    {
        shapeView_->blockSignals(true);
        if(currentView_ == NAME)
        {
            shapeModel_->setName(textItems_[index]->document()->toPlainText(), index);
            if(index == current_)
            {
                shapeView_->setName(textItems_[index]->document()->toPlainText());
            }
        }
        else if(currentView_ == DATA)
        {
            shapeModel_->setOtherData(textItems_[index]->document()->toPlainText(), index);
        }
        else if(currentView_ == ID)
        {
            shapeModel_->setIdNumber(textItems_[index]->document()->toPlainText().toDouble(), index);
        }
        shapeView_->blockSignals(false);
    }

}

void AMShapeDataSetView::updateCurrentTextItemName()
{
    if(shapeModel_->isValid(current_))
    {
        if(currentView_ == NAME)
            textItems_[current_]->setPlainText(currentName());
    }
}

void AMShapeDataSetView::setViewName()
{
    currentView_ = NAME;
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setViewOtherData()
{
    currentView_ = DATA;
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setViewIdNumber()
{
    currentView_ = ID;
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setViewHidden()
{
    currentView_ = HIDE;
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::autoCompleteEnterPressed()
{
    shapeModel_->setCurrentInfo(autoCompleteBox_->text());
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::beamShape(int shapeNumber)
{
    QList<QPointF*>* points;
    if(beamWizard_ != 0)
        points = beamWizard_->pointList();
    else return;
    QPointF topLeft = mapPointToVideo(*(points->at(2*shapeNumber)));
    QPointF bottomRight = mapPointToVideo(*(points->at(2*shapeNumber+1)));

    setDrawOnShapeEnabled(true);
    shapeModel_->setDrawOnSamplePlate();


    shapeModel_->setBeamMarker(topLeft,shapeNumber);
    shapeModel_->updateBeamMarker(bottomRight, shapeNumber);
    currentChanged();

    reviewCrosshairLinePositions();

    if(updateTracker_ == shapeNumber)
    {
        beamWizard_->updateShape(shapes_[current_]);
    }
    else
    {
        AMShapeDataSetGraphicsView* newView = new AMShapeDataSetGraphicsView();
        newView->setScene(shapeScene_->scene());
        beamWizard_->updateScene(newView);
    }

    updateTracker_ = shapeNumber;



}

void AMShapeDataSetView::beamCalibrate()
{

    shapeModel_->beamCalibrate();
    reviewCrosshairLinePositions();

}

void AMShapeDataSetView::moveBeamSamplePlate(QVector3D coordinate)
{
//    shapeModel_->moveSamplePlateTo(coordinate);// change to move motor
    qDebug()<<"AMShapeDataSetView::moveBeamSamplePlate - moving to coordinate"<<coordinate;
    shapeModel_->moveMotorTo(coordinate);
    reviewCrosshairLinePositions();
    AMShapeDataSetGraphicsView* view = new AMShapeDataSetGraphicsView();
    view->setScene(shapeScene_->scene());
    if(beamWizard_->isVisible())
        beamWizard_->updateScene(view);
    if(cameraWizard_->isVisible())
        cameraWizard_->updateScene(view);
    if(samplePlateWizard_->isVisible())
        samplePlateWizard_->updateScene(view);
}

void AMShapeDataSetView::showBeamMarker(int index)
{
    shapeModel_->addBeamMarker(index);
    reviewCrosshairLinePositions();
    beamWizard_->updateScene(shapeScene_);
}

void AMShapeDataSetView::transmitMotorMovementEnabled()
{
    qDebug()<<"AMShapeDataSetView::transmitMotorMovementEnabled - returning request";
    emit motorMovementEnabled(shapeModel_->motorMovementenabled());
}

void AMShapeDataSetView::updateShapeName(QString newName, QString oldName)
{
    qDebug()<<"AMShapeDataSetView::updateShapeName - Updating shape name";
    if(currentView_ == NAME)
    {
        if(currentIndex() >= 0 && textItems_.count() > currentIndex() && textItems_.at(currentIndex())->document()->toPlainText() == oldName)
            textItems_[currentIndex()]->setPlainText(newName);
        else
            reviewCrosshairLinePositions();
    }
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
    shapeModel_->updateAllShapes();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::showCameraBeamWindow()
{
    configurationWindow_->show();
    configurationWindow_->adjustSize();
}

void AMShapeDataSetView::showShapeView()
{
    shapeView_->show();
}

void AMShapeDataSetView::setDrawOnShape()
{
    shapeModel_->setDrawOnShape();
}

void AMShapeDataSetView::setDrawOnShapeEnabled(bool enable)
{
    shapeModel_->setDrawOnShapeEnabled(enable);
    drawOnShapePushButton_->setDisabled(!enable);
}

void AMShapeDataSetView::reviewCameraConfiguration()
{
    bool review = false;
    QList<QPointF*>* pointList = cameraWizard_->pointList();
    foreach(QPointF* point, *pointList)
    {
        if(*point != QPointF(0,0))
            review = true;
    }

    QList<QVector3D*>* coordinateList = cameraWizard_->coordinateList();
//    QPointF topLeft = shapeScene_->mapSceneToVideo(shapeScene_->videoItem()->sceneBoundingRect().topLeft());
//    QPointF bottomRight = shapeScene_->mapSceneToVideo(shapeScene_->videoItem()->sceneBoundingRect().bottomRight());
//    QPointF factor = bottomRight - topLeft;
//    QPointF offset = topLeft;
    QPointF positions[SAMPLEPOINTS];
    QVector3D coordinates[SAMPLEPOINTS];
    if(review)
    {
        for(int i = 0; i < SAMPLEPOINTS; i++)
        {
            positions[i] = mapPointToVideo(*pointList->at(i));
//            positions[i].setX(positions[i].x()*(factor.x()));
//            positions[i].setY(positions[i].y()*(factor.y()));
//            positions[i] += offset;
            coordinates[i] = *coordinateList->at(i);
        }
        shapeModel_->findCamera(positions,coordinates);
        cameraConfiguration_->updateAll();
        shapeModel_->updateAllShapes();
        reviewCrosshairLinePositions();
    }
}

void AMShapeDataSetView::setTilt(QString tilt)
{
    double newTilt = tilt.toDouble();
    shapeModel_->setTilt(newTilt);
}



void AMShapeDataSetView::toggleDistortion()
{
    shapeModel_->toggleDistortion();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setMedia(QMediaContent url)
{
    shapeScene_->mediaPlayer()->setMedia(url);
    qDebug() << "Status? " << shapeScene_->mediaPlayer()->mediaStatus();
}

#include <QApplication>
void AMShapeDataSetView::play()
{
    qDebug()<<"supported"<<QMediaPlayer::supportedMimeTypes();
    shapeScene_->mediaPlayer()->play();
}

QMediaPlayer* AMShapeDataSetView::mediaPlayer()
{
    return shapeScene_->mediaPlayer();
}

QPointF AMShapeDataSetView::mapPointToVideo(QPointF position)
{
    QPointF topLeft = shapeScene_->mapSceneToVideo(shapeScene_->videoItem()->sceneBoundingRect().topLeft());
    QPointF bottomRight = shapeScene_->mapSceneToVideo(shapeScene_->videoItem()->sceneBoundingRect().bottomRight());
    QPointF factor = bottomRight - topLeft;
    QPointF offset = topLeft;
    QPointF newPosition;
    newPosition.setX(position.x()*factor.x());
    newPosition.setY(position.y()*factor.y());
    newPosition += offset;
    return newPosition;
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

//        if(showCrosshairCheckBox_->isChecked() != crosshairVisible)
//        {
//            showCrosshairCheckBox_->blockSignals(true);
//            showCrosshairCheckBox_->setChecked(crosshairVisible);
//            showCrosshairCheckBox_->blockSignals(false);
//        }
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

void AMShapeDataSetView::hideCameraParameters(bool hide)
{
    cameraConfiguration_->hideCameraParameters(hide);
    configurationWindow_->adjustSize();
}

void AMShapeDataSetView::startCameraWizard()
{
    delete cameraWizard_;
    qDebug()<<"AMShapeDataSetView::startCameraWizard - creating new wizard";
    cameraWizard_ = new AMCameraConfigurationWizard();
    connect(cameraWizard_, SIGNAL(done()), this, SLOT(reviewCameraConfiguration()));
    connect(cameraWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
    connect(cameraWizard_, SIGNAL(moveTo(QVector3D)), this, SLOT(moveBeamSamplePlate(QVector3D)));
    connect(this, SIGNAL(motorMovementEnabled(bool)), cameraWizard_, SLOT(setMotorMovementEnabled(bool)));
//    connect(this, SIGNAL(moveSucceeded()), cameraWizard_, SIGNAL(moveSucceeded()));
    connect(this,SIGNAL(moveSucceeded()), cameraWizard_, SLOT(testMoveSlot()));
    qDebug()<<"AMShapeDataSetView::startCameraWizard - setting view";
    AMShapeDataSetGraphicsView* view = new AMShapeDataSetGraphicsView(0);
    view->setScene(shapeScene_->scene());
    view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
    cameraWizard_->setView(view);
    qDebug()<<"AMShapeDataSetView::startCameraWizard - showing";
    cameraWizard_->show();
}

void AMShapeDataSetView::startBeamWizard()
{
    delete beamWizard_;
    beamWizard_ = new AMBeamConfigurationWizard();
    connect(beamWizard_, SIGNAL(showShape(int)), this, SLOT(beamShape(int)));
    connect(beamWizard_, SIGNAL(done()), this, SLOT(beamCalibrate()));
    connect(beamWizard_, SIGNAL(moveTo(QVector3D)), this, SLOT(moveBeamSamplePlate(QVector3D)));/// move to \todo implement for others
    connect(beamWizard_, SIGNAL(showBeamMarker(int)), this, SLOT(showBeamMarker(int)));
    connect(beamWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
    connect(this, SIGNAL(motorMovementEnabled(bool)), beamWizard_, SLOT(setMotorMovementEnabled(bool)));
    connect(this, SIGNAL(moveSucceeded()), beamWizard_, SIGNAL(moveSucceeded()));
    AMShapeDataSetGraphicsView* view = new AMShapeDataSetGraphicsView(0);
    view->setScene(shapeScene_->scene());
    view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
    beamWizard_->setView(view);
    beamWizard_->show();
}

void AMShapeDataSetView::startSampleWizard()
{
    delete samplePlateWizard_;
    samplePlateWizard_ = new AMSamplePlateWizard();
    AMShapeDataSetGraphicsView* view = new AMShapeDataSetGraphicsView();
    connect(samplePlateWizard_, SIGNAL(movePlate(int)), this, SLOT(moveSamplePlate(int)));
    connect(samplePlateWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
    connect(samplePlateWizard_, SIGNAL(moveTo(QVector3D)), this, SLOT(moveBeamSamplePlate(QVector3D)));
    connect(this, SIGNAL(motorMovementEnabled(bool)), samplePlateWizard_, SLOT(setMotorMovementEnabled(bool)));
    connect(this, SIGNAL(moveSucceeded()), samplePlateWizard_, SIGNAL(moveSucceeded()));
    view->setScene(shapeScene_->scene());
    view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
    samplePlateWizard_->setView(view);
    samplePlateWizard_->show();
}

void AMShapeDataSetView::setSamplePlate()
{
    shapeModel_->setSamplePlate();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setCameraConfigurationShape()
{
    shapeModel_->setCameraConfigurationShape();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::moveSamplePlate(int movement)
{
    int relativeMovement = movement - samplePlateMovement_;
    samplePlateMovement_ = movement;
    shapeModel_->moveSamplePlate(relativeMovement);
    reviewCrosshairLinePositions();
    int index = shapeModel_->samplePlateIndex();
    AMShapeDataSetGraphicsView* view = shapeScene_;
    if(shapeModel_->isValid(index))
        samplePlateWizard_->updateScene(view);
}

void AMShapeDataSetView::showBeamOutline(bool show)
{
    showBeamOutline_ = show;
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::moveTestSlot()
{
    qDebug()<<"AMShapeDataSetView::moveTestSlot - move succeeded signal recieved";
    emit moveSucceeded();
}

//void AMShapeDataSetView::setShapeVisible(bool visible)
//{
//    if(shapeModel_->isValid(current_))
//    {
//        shapes_[current_]->setVisible(visible);
//        reviewCrosshairLinePositions();
//    }
//}

void AMShapeDataSetView::updateCurrentShape()
{
    emit updateShapes(current_);
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::createIntersectionShapes(QVector<QPolygonF> shapes)
{
    intersections_.clear();
    for(int i =0; !shapes.isEmpty(); i++)
    {
        QPen pen(QColor(Qt::yellow));
        QBrush brush(QColor(Qt::yellow));
        if(!showBeamOutline_)
        {
         pen.setColor(Qt::transparent);
         brush.setColor(Qt::transparent);
        }
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


//void AMShapeDataSetView::setListViewModel()
//{
////    shapeDataListView_->setResizeMode(QListView::Adjust);
////    shapeDataListView_->setModel(shapeModel_);
//}



void AMShapeDataSetView::resizeEvent(QResizeEvent *event)
{

    shapeScene_->resizeEvent(event);
	reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mousePressHandler(QPointF position)
{
    connect(this, SIGNAL(mouseMove(QPointF)), this, SLOT(mouseMoveHandler(QPointF)));

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
    else if(mode_ == MULTIDRAW)
    {
        emit mouseMultiDrawPressed((position));
    }
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mouseRightClickHandler(QPointF position)
{
    connect(this, SIGNAL(mouseMove(QPointF)), this, SLOT(mouseMoveHandler(QPointF)));

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
    pressTimer_->stop();
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
        disconnect(this, SLOT(mouseMoveHandler(QPointF)));

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
    disconnect(this, SLOT(mouseMoveHandler(QPointF)));
    reviewCrosshairLinePositions();
}



void AMShapeDataSetView::mouseDoubleClickHandler(QPointF position)
{
		doubleClickInProgress_ = true;
        if(mode_ == MULTIDRAW)
        {
            emit mouseMultiDrawDoubleClicked(position);
            reviewCrosshairLinePositions();
            emit currentChanged();
        }
        else if(!crosshairLocked())
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

//    reviewCrosshairLinePositions();


}






/// append a new rectangle to the current list, add it to the scene
void AMShapeDataSetView::addNewShape()
{
    index_++;
    QPen pen(borderColour_);
    QBrush brush(QColor(Qt::transparent));
    QPolygonF polygon(QRectF(5,5,20,20));
    shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));
    GraphicsTextItem* newItem = new GraphicsTextItem();
    shapeScene_->scene()->addItem(newItem);
    textItems_.insert(index_,newItem) ;
    textItems_[index_]->setZValue(1000);
    textItems_[index_]->setTextInteractionFlags(Qt::TextEditable);
    textItems_[index_]->setShapeIndex(index_);
//    textItems_[index_]->set
//    textItems_[index_]->setAcceptedMouseButtons(Qt::LeftButton);
    connect(textItems_[index_], SIGNAL(textChanged(int)), this, SLOT(updateItemName(int)));
}

/// Remove a rectangle from the scene
void AMShapeDataSetView::deleteShape()
{
    QGraphicsPolygonItem* polygon = shapes_[index_];
    shapeScene_->scene()->removeItem(shapes_[index_]);
    shapes_.remove(index_);
    QGraphicsTextItem* text = textItems_[index_];
    shapeScene_->scene()->removeItem(textItems_[index_]);
    textItems_.removeAt(index_);
    index_--;
    delete polygon;
    delete text;
}

/// change the currently selected item, outline it in blue?
void AMShapeDataSetView::currentSelectionChanged()
{
    if(shapeModel_->isValid(current_))
        shapes_[current_]->setPen(borderColour_);
    current_ = shapeModel_->currentIndex();

    shapeView_->setShapeData(shapeModel_->currentShape());
    motorREdit_->setText(QString::number(motorRotation()));
    motorXEdit_->setText(QString::number(motorX()));
    motorYEdit_->setText(QString::number(motorY()));
    motorZEdit_->setText(QString::number(motorZ()));

    autoCompleteBox_->setText(shapeModel_->currentInfo());

    if(shapeModel_->isValid(current_))
    {
        shapes_[current_]->setPen(activeBorderColour_);

    }

    reviewCrosshairLinePositions();

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


void AMShapeDataSetView::setGUI()
{

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



    tfl->addWidget(toolBar_ = new QToolBar("Tool Bar"));
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




    QFrame* shapeFrame = new QFrame();
    QHBoxLayout* shapeHorizontalLayout =  new QHBoxLayout();
    shapeHorizontalLayout->setContentsMargins(12,4,12,4);
//    shapeHorizontalLayout->addWidget(showShapeView_ = new QPushButton("Show shape"));
//    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(drawOnShapeCheckBox_ = new QCheckBox("Draw on shape"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(drawOnShapePushButton_ = new QPushButton("Select Shape"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(distortionButton_ = new QPushButton("Distortion"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(labelToolBar_ = new QToolBar("Labels"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(autoCompleteBox_ = new QLineEdit());
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(cameraWizardButton_ = new QPushButton("Camera Wizard"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(beamWizardButton_ = new QPushButton("Beam Wizard"));
    shapeHorizontalLayout->addSpacing(20);
    shapeHorizontalLayout->addWidget(samplePlateWizardButton_ = new QPushButton("Sample Wizard"));
    shapeHorizontalLayout->addStretch();
    shapeFrame->setLayout(shapeHorizontalLayout);




    drawOnShapeCheckBox_->setChecked(false);
    drawOnShapePushButton_->setDisabled(true);


    QVBoxLayout *vbl = new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    vbl->addWidget(crosshairFrame);
    vbl->addWidget(shapeScene_);
    vbl->addWidget(shapeFrame);
    vbl->addWidget(toolFrame);
    setLayout(vbl);


    configurationWindow_ = new QFrame();
    QVBoxLayout *cvl = new QVBoxLayout();
    cvl->setContentsMargins(0,0,0,0);
    cvl->addWidget(cameraConfiguration_);
    cvl->addWidget(beamConfiguration_);

    QFrame* samplePlateFrame = new QFrame();
    QHBoxLayout* samplePlateLayout = new QHBoxLayout();
    samplePlateLayout->setContentsMargins(12,4,12,4);
    samplePlateLayout->addWidget(samplePlateButton_ = new QPushButton("Set Sample Plate"));
    samplePlateLayout->addSpacing(20);
    samplePlateLayout->addWidget(cameraConfigurationShapeButton_ = new QPushButton("Set Outer Plate"));
    samplePlateLayout->addStretch(20);
    samplePlateLayout->addWidget(showBeamOutlineCheckBox_ = new QCheckBox("Show beam area"));
    samplePlateLayout->addStretch();
    samplePlateFrame->setLayout(samplePlateLayout);

    cvl->addWidget(samplePlateFrame);

    showBeamOutlineCheckBox_->setChecked(true);

    configurationWindow_->setLayout(cvl);

    configurationWindow_->setWindowTitle("Configuration");


    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        pointPushButton_[i] = new QPushButton("Set Point " + QString::number(i));
        pointLineEdit_[i] = new QLineEdit("point"+QString::number(i));
        pointLineEdit_[i+SAMPLEPOINTS] = new QLineEdit("point"+QString::number(SAMPLEPOINTS+i));
        coordinateLineEdit_[i] = new QLineEdit("coordinate"+QString::number(i));
        coordinateLineEdit_[i+SAMPLEPOINTS] = new QLineEdit("coordinate"+QString::number(SAMPLEPOINTS+i));
        coordinateLineEdit_[i+2*SAMPLEPOINTS] = new QLineEdit("coordinate"+QString::number(2*SAMPLEPOINTS+i));
    }




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

    /// camera configuration window (finds camera)
    cameraConfigurationWindow_ = new QFrame();

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


    QActionGroup* actionGroup = new QActionGroup(this);
    markAction_ = new QAction("Mark",actionGroup);
    moveAction_ = new QAction("Move", actionGroup);
    editAction_ = new QAction("Edit", actionGroup);
    shiftAction_ = new QAction("Shift", actionGroup);
    operationAction_ = new QAction("Operation", actionGroup);
    groupAction_ = new QAction("Group", actionGroup);
    markAction_->setCheckable(true);
    moveAction_->setCheckable(true);
    editAction_->setCheckable(true);
    shiftAction_->setCheckable(true);
    operationAction_->setCheckable(true);
    groupAction_->setCheckable(true);
    markAction_->setChecked(true);

    toolBar_->addAction(markAction_);
    toolBar_->addAction(moveAction_);
    toolBar_->addAction(editAction_);
    toolBar_->addAction(shiftAction_);
    toolBar_->addAction(operationAction_);
    toolBar_->addAction(groupAction_);

    markAction_->setToolTip("Press twice for polygon marking.");

    QActionGroup* labelActionGroup = new QActionGroup(this);
    viewName_ = new QAction("Name", labelActionGroup);
    viewOtherData_ = new QAction("Data", labelActionGroup);
    viewIdNumber_ = new QAction("Id", labelActionGroup);
    viewHidden_ = new QAction("Hide", labelActionGroup);
    viewName_->setCheckable(true);
    viewOtherData_->setCheckable(true);
    viewIdNumber_->setCheckable(true);
    viewHidden_->setCheckable(true);
    viewName_->setChecked(true);

    labelToolBar_->addAction(viewName_);
    labelToolBar_->addAction(viewOtherData_);
    labelToolBar_->addAction(viewIdNumber_);
    labelToolBar_->addAction(viewHidden_);


    bool defaultUseCameraMatrix = true;
    cameraMatrixCheckBox_->setChecked(defaultUseCameraMatrix);
    setUseCameraMatrix(defaultUseCameraMatrix);
    hideCameraParameters(defaultUseCameraMatrix);

}

void AMShapeDataSetView::makeConnections()
{
    // mouse signals to be handled
    connect(shapeScene_, SIGNAL(mousePressed(QPointF)), this, SLOT(mousePressHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseRightClicked(QPointF)), this, SLOT(mouseRightClickHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseLeftReleased(QPointF)), this, SLOT(mouseLeftReleaseHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseRightReleased(QPointF)), this, SLOT(mouseRightReleaseHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(mouseDoubleClickHandler(QPointF)));
    connect(shapeScene_, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(mouseMove(QPointF)));


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
    connect(this, SIGNAL(mouseMultiDrawPressed(QPointF)), shapeModel_, SLOT(drawShape(QPointF)));
    connect(this, SIGNAL(mouseMultiDrawDoubleClicked(QPointF)), shapeModel_, SLOT(finishShape()));

    connect(this, SIGNAL(enterMultiDraw()), shapeModel_, SLOT(startMultiDraw()));

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
    connect(setMotorCoordinate_, SIGNAL(clicked()), this, SLOT(setMotorCoordinatePressed()));

    connect(shapeModel_, SIGNAL(motorMoved()), this, SLOT(motorMoved()));

    /// shape view
    connect(this, SIGNAL(applyDistortion()), this, SLOT(toggleDistortion()));


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
    connect(cameraMatrixCheckBox_, SIGNAL(clicked(bool)), this, SLOT(hideCameraParameters(bool)));

    connect(configurationWindowButton_, SIGNAL(clicked()), this, SLOT(showCameraBeamWindow()));

    connect(samplePlateButton_, SIGNAL(clicked()), this, SLOT(setSamplePlate()));

    connect(cameraConfigurationShapeButton_, SIGNAL(clicked()), this, SLOT(setCameraConfigurationShape()));

    connect(showBeamOutlineCheckBox_, SIGNAL(toggled(bool)), this, SLOT(showBeamOutline(bool)));

//    connect(showShapeView_, SIGNAL(clicked()), this, SLOT(showShapeView()));
    connect(drawOnShapePushButton_, SIGNAL(clicked()), this, SLOT(setDrawOnShape()));
    connect(drawOnShapeCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setDrawOnShapeEnabled(bool)));


    connect(distortionButton_, SIGNAL(clicked()), this, SIGNAL(applyDistortion()));
    connect(cameraWizardButton_, SIGNAL(clicked()), this, SLOT(startCameraWizard()));
    connect(beamWizardButton_, SIGNAL(clicked()), this, SLOT(startBeamWizard()));
    connect(samplePlateWizardButton_, SIGNAL(clicked()), this, SLOT(startSampleWizard()));


    /// allows non-rectangle drawing

    connect(pressTimer_, SIGNAL(timeout()), this, SLOT(setMultiDrawMode()));
    connect(this, SIGNAL(modeChange()), this, SLOT(changeDrawButtonText()));

    connect(shapeView_, SIGNAL(newName()), this, SLOT(updateCurrentTextItemName()));


    connect(autoCompleteBox_, SIGNAL(returnPressed()), this, SLOT(autoCompleteEnterPressed()));

    /// shape list view
//    connect(shapeDataListView_, SIGNAL(currentIndexChanged()), this, SLOT(currentSelectionChanged()));

//    connect(shapeView_, SIGNAL(shapeVisible(bool)), this, SLOT(setShapeVisible(bool)));

    connect(markAction_, SIGNAL(triggered()), this, SLOT(setDrawMode()));
    connect(moveAction_, SIGNAL(triggered()), this, SLOT(setMoveMode()));
    connect(editAction_, SIGNAL(triggered()), this, SLOT(setEditMode()));
    connect(shiftAction_, SIGNAL(triggered()), this, SLOT(setShiftMode()));
    connect(operationAction_, SIGNAL(triggered()), this, SLOT(setOperationMode()));
    connect(groupAction_, SIGNAL(triggered()), this, SLOT(setGroupMode()));

    connect(viewName_, SIGNAL(triggered()), this, SLOT(setViewName()));
    connect(viewOtherData_, SIGNAL(triggered()), this, SLOT(setViewOtherData()));
    connect(viewIdNumber_, SIGNAL(triggered()), this, SLOT(setViewIdNumber()));
    connect(viewHidden_, SIGNAL(triggered()), this, SLOT(setViewHidden()));

    connect(shapeModel_, SIGNAL(moveSucceeded()), this, SLOT(moveTestSlot()));
    connect(shapeModel_, SIGNAL(shapeNameChanged(QString, QString)), this, SLOT(updateShapeName(QString, QString)));

}
