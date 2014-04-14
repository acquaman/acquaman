 #include "AMSampleCameraView.h"
#include <QGraphicsLineItem>
#include <QResizeEvent>
#include <QGraphicsItem>

#include <QMap>
#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include "beamline/camera/AMSampleCamera.h"
#include "beamline/camera/AMCameraConfiguration.h"

#include <QSlider>
#include <QCheckBox>
#include "ui/AMColorPickerButton2.h"

#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include "ui/beamline/camera/AMSampleCameraGraphicsView.h"

#include "ui/beamline/camera/AMShapeDataView.h"
#include <QPushButton>
#include <QLineEdit>

#include "ui/beamline/camera/AMCameraConfigurationView.h"
#include "ui/beamline/camera/AMBeamConfigurationView.h"
#include <QColor>

#include <QTimer>
#include <QTextDocument>

#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <QCompleter>
#include <QStringListModel>

#include "ui/AMGraphicsTextItem.h"
#include <QMediaPlayer>
#include <QPainterPath>

#include "ui/beamline/camera/AMCameraConfigurationWizard.h"
#include "ui/beamline/camera/AMBeamConfigurationWizard.h"
#include "ui/beamline/camera/AMSamplePlateWizard.h"
#include "ui/beamline/camera/AMRotationWizard.h"

#include <limits>

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSample.h"

#include "beamline/camera/AMRotationalOffset.h"

#include "beamline/AMBeamline.h"
#include "beamline/camera/AMAngle.h"

#define SAMPLEPOINTS 6

#define DELAY 1000



AMSampleCameraView::AMSampleCameraView(AMSampleCamera *shapeModel, ViewType viewType, QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	shapeModel_ = shapeModel;
	shapeScene_ = new AMSampleCameraGraphicsView(parent, useOpenGlViewport);
	shapeScene_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	shapeScene_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	shapeScene_->videoItem()->setAspectRatioMode(Qt::KeepAspectRatio);
	cameraConfiguration_ = new AMCameraConfigurationView(shapeModel_->cameraConfiguration());
	beamConfiguration_ = new AMBeamConfigurationView(shapeModel_->beamConfiguration());

	updateTracker_ = -1;
	samplePlateMovement_ = 0;



	//    borderColour_ = QColor(Qt::red);

	//    activeBorderColour_ = QColor(Qt::blue);

	pressTimer_ = new QTimer();
	pressTimer_->setInterval(DELAY);
	pressTimer_->setSingleShot(true);

	currentView_ = NAME;
	mode_ = DRAW;


	// doing this broke things horribly, not sure why
//	/// start them off null, create them and delete them when used
//	cameraWizard_ = NULL;
//	beamWizard_ = NULL;
//	samplePlateWizard_ = NULL;
//	rotationWizard_ = NULL;
	cameraWizard_ = new AMCameraConfigurationWizard();
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView(parent, useOpenGlViewport);
	view->setScene(shapeScene_->scene());
	cameraWizard_->setView(view);

	beamWizard_ = new AMBeamConfigurationWizard();
	beamWizard_->setView(view);

	samplePlateWizard_ = new AMSamplePlateWizard();
	samplePlateWizard_->setView(view);

	rotationWizard_ = new AMRotationWizard();
	rotationWizard_->setView(view);


	showBeamOutline_ = true;

	index_ = 0;
	groupRectangleActive_= false;
	samplePlateSelected_ = false;

	QPen pen(colour(BORDER));

	QBrush brush(colour(FILL));

	crosshairXLine_ = shapeScene_->scene()->addLine(0.5,0,0.5,1,pen);
	crosshairYLine_ = shapeScene_->scene()->addLine(0,0.5,0,1,pen);
//	crosshairXLine_->setVisible(true);
//	crosshairYLine_->setVisible(true);

	crosshairXLine_->setVisible(false);
	crosshairYLine_->setVisible(false);

	QPolygonF polygon(QRectF(5, 5, 0, 0));

	shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));

	AMGraphicsTextItem* newItem = new AMGraphicsTextItem();
	shapeScene_->scene()->addItem(newItem);
	textItems_<<newItem;

	videoBorderItem_ = new QGraphicsRectItem(QRectF(5,5,0,0));
	shapeScene_->scene()->addItem(videoBorderItem_);


	doubleClickInProgress_ = false;

	QPainterPath* path = new QPainterPath();
	path->addPolygon(polygon);
	currentShape_ = shapeScene_->scene()->addPath(*path,pen,brush);
	samplePlate_ = 0;


	setGUI(viewType);


	//    QStringList wordList;
	//    wordList_ = new QStringListModel(wordList);
	//    autoCompleter_ = new QCompleter();
	//    autoCompleter_->setModel(wordList_);
	//    autoCompleter_->setCaseSensitivity(Qt::CaseInsensitive);
	//    autoCompleteBox_->setCompleter(autoCompleter_);


	makeConnections(viewType);



	/// This fixes the problem with with different views flashing or going white
	/// when they have QGraphicsTextItems or GraphicsTextItems in them.
	/// No real reason why this works, it just does.
	/// see AMGraphicsViewWizard::setView (AMGraphicsViewWizard.cpp) for a full description
	QGraphicsTextItem* textFixItem = new QGraphicsTextItem("Fix");
	textFixItem->setZValue(INT_MAX);
	textFixItem->setPos(-1*textFixItem->boundingRect().width(), -1*textFixItem->boundingRect().height());
	shapeScene_->scene()->addItem(textFixItem);

	refreshSceneView();

	connect(shapeModel_, SIGNAL(enableMotorMovementChanged(bool)), this, SLOT(onEnableMotorMovementChanged(bool)));
	connect(shapeModel_, SIGNAL(enableMotorTrackingChanged(bool)), this, SLOT(onEnableMotorTrackingChanged(bool)));

	showGrid_ = false;
}

AMSampleCameraView::~AMSampleCameraView()
{
	delete beamWizard_;
	delete cameraWizard_;
	delete samplePlateWizard_;
	delete rotationWizard_;
}


void AMSampleCameraView::refreshSceneView()
{

	QSizeF viewSize = shapeScene_->sceneRect().size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
	QSizeF videoSize = shapeScene_->videoItem()->nativeSize();
	double videoTop = shapeScene_->videoItem()->boundingRect().topLeft().y();

	// scale the video size to the view size, obeying the transformation mode
	QSizeF scaledSize = videoSize;
        scaledSize.scale(viewSize, shapeScene_->videoItem()->aspectRatioMode());

	shapeModel_->setViewSize(viewSize);
	shapeModel_->setScaledSize(scaledSize, videoTop);
	// now, scaledSize will either be:
	// same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
	// One dimension the same, other dimension smaller than viewSize, if Qt::KeepAspectRatio
	// or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,videoTop),
				   //                                       (viewSize.height()-scaledSize.height())/2),
                                    scaledSize);


	// activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal xSceneCoord = activeRect.left() + shapeModel_->crosshairX()*activeRect.width();
	qreal ySceneCoord = activeRect.top() + shapeModel_->crosshairY()*activeRect.height();


	videoBorderItem_->setRect(activeRect);




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

		if(isValid(current_))
			shapes_[current_]->setPen(colour(BORDER));
		current_ = -1;
		shapeModel_->setCurrentIndex(current_);
		emit currentChanged();
	}
	for(int i = 0; i < index_+ 1; i++)
	{
		if(shapes_.contains(i) && isValid(i))
		{
			shapes_[i]->setPolygon(shapeModel_->shape(i));
			shapes_[i]->setToolTip(shapeModel_->name(i));
			shapes_[i]->setVisible(shapeModel_->visible(i));

			if(shapeModel_->isBackwards(i))
			{
				shapes_[i]->setBrush(QBrush(colour(BACKWARDSFILL)));
			}
			else
			{
				shapes_[i]->setBrush(QBrush(colour(FILL)));
			}


			if(textItems_.count() < i+1) textItems_<< new AMGraphicsTextItem();
			if(isValid(i))
			{
				QPointF point(0,textItems_[i]->boundingRect().height());
				QPointF first = shapes_[i]->polygon().first();
				textItems_[i]->setPos(first-point);
				textItems_[i]->setDefaultTextColor(colour(BORDER));
				if(i == current_)textItems_[i]->setDefaultTextColor(Qt::blue);
				if(currentView_ == NAME)
				{
					textItems_[i]->setPlainText(shapeModel_->name(i));
					textItems_[i]->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextEditable);
				}
				else if(currentView_ == DATA)
				{
					qDebug()<<shapeModel_->otherDataOne(i);
					textItems_[i]->setPlainText(shapeModel_->otherDataOne(i));
					textItems_[i]->setTextInteractionFlags(Qt::NoTextInteraction);
				}
				else if(currentView_ == ID)
				{
					textItems_[i]->setPlainText(shapeModel_->otherDataTwo(i));
					textItems_[i]->setTextInteractionFlags(Qt::NoTextInteraction);
				}

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
	// draw the sample plate
	drawSamplePlate();
	// print the intersection shapes
	intersection();


	// show the polygon currently being drawn
	QPolygonF currentPolygon = shapeModel_->currentPolygon();
	QPainterPath* path = new QPainterPath();
	path->addPolygon(currentPolygon);
	currentShape_->setPath(*path);

	shapeModel_->updateView();

}

void AMSampleCameraView::setCrosshairPosition(const QPointF &pos)
{
	shapeModel_->setCrosshair(pos);
}


QPen AMSampleCameraView::crosshairPen() const
{
	return crosshairXLine_->pen();
}


QPointF AMSampleCameraView::crosshairPosition() const
{
	return shapeModel_->crosshair();
}

bool AMSampleCameraView::crosshairVisible() const
{
	return crosshairXLine_->isVisible();
}

bool AMSampleCameraView::crosshairLocked() const
{
	return shapeModel_->crosshairLocked();
}

void AMSampleCameraView::setDrawMode()
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

void AMSampleCameraView::setMoveMode()
{
	mode_ = MOVE;
	emit modeChange();
}

void AMSampleCameraView::setEditMode()
{
	mode_ = EDIT;
	emit modeChange();
}

void AMSampleCameraView::setShiftMode()
{
	mode_ = SHIFT;
	emit modeChange();

}

void AMSampleCameraView::setOperationMode()
{
	mode_ = OPERATION;
	emit modeChange();
}

void AMSampleCameraView::setGroupMode()
{
	mode_ = GROUP;
	emit modeChange();

}

void AMSampleCameraView::setConfigurationMode()
{
	mode_ = CONFIGURE;
	emit modeChange();

}

void AMSampleCameraView::setMultiDrawMode()
{
	//    if(drawButton_->isDown() || )
	mode_ = MULTIDRAW;
	emit modeChange();
	emit enterMultiDraw();
}

void AMSampleCameraView::setMotorCoordinatePressed()
{
	double x = (motorXEdit_->text().toDouble());
	double y = (motorYEdit_->text().toDouble());
	double z = (motorZEdit_->text().toDouble());
	double r = (motorREdit_->text().toDouble());
	setMotorCoordinate(x,y,z,r);
}

void AMSampleCameraView::enableMotorMovement(bool isEnabled)
{
	if(isEnabled != enableMotorMovement_->isChecked())
	{
		qDebug()<<"enableMotorMovement has incorrect value";
		enableMotorMovement_->setChecked(false);
		isEnabled = false;
	}
	shapeModel_->enableMotorMovement(isEnabled);

}

void AMSampleCameraView::enableMotorTracking(bool isEnabled)
{
	shapeModel_->enableMotorTracking(isEnabled);
}

void AMSampleCameraView::onEnableMotorMovementChanged(bool isEnabled){
	if(enableMotorMovement_->isChecked() != isEnabled)
		enableMotorMovement_->setChecked(isEnabled);
}

void AMSampleCameraView::onEnableMotorTrackingChanged(bool isEnabled){
	if(enableMotorTracking_->isChecked() != isEnabled)
		enableMotorTracking_->setChecked(isEnabled);
}

void AMSampleCameraView::showConfigurationWindow()
{
	cameraConfigurationWindow_->show();
}


void AMSampleCameraView::setPoint(QPointF position, int pointToSelect)
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

void AMSampleCameraView::selectPoint(int point)
{
	setConfigurationMode();
	pointToSelect_ = point;
}

void AMSampleCameraView::selectPointOne()
{
	setConfigurationMode();
	pointToSelect_ = 0;
}



void AMSampleCameraView::selectPointTwo()
{
	setConfigurationMode();
	pointToSelect_ = 1;
}

void AMSampleCameraView::selectPointThree()
{
	setConfigurationMode();
	pointToSelect_ = 2;
}

void AMSampleCameraView::selectPointFour()
{
	setConfigurationMode();
	pointToSelect_ = 3;
}

void AMSampleCameraView::selectPointFive()
{
	setConfigurationMode();
	pointToSelect_ = 4;
}

void AMSampleCameraView::selectPointSix()
{
	setConfigurationMode();
	pointToSelect_ = 5;
}

void AMSampleCameraView::runCameraConfiguration()
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
	refreshSceneView();



}

void AMSampleCameraView::deleteCalibrationPoints()
{
	shapeModel_->deleteCalibrationPoints();
	refreshSceneView();
}

void AMSampleCameraView::stopTimer()
{
	pressTimer_->stop();
}

void AMSampleCameraView::changeDrawButtonText()
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

void AMSampleCameraView::updateItemName(int index)
{
	if(isValid(index))
	{
		//        shapeView_->blockSignals(true);
		if(currentView_ == NAME)
		{
			shapeModel_->setName(textItems_[index]->toPlainText(), index);
		}
		else if(currentView_ == DATA)
		{
			shapeModel_->setOtherDataOne(textItems_[index]->toPlainText(), index);
		}
		else if(currentView_ == ID)
		{
			shapeModel_->setOtherDataTwo(textItems_[index]->toPlainText(), index);
		}
		//        shapeView_->blockSignals(false);
	}

}

void AMSampleCameraView::updateItemReturnPressed(int index){
	emit shapePropertyUpdated(shapeModel_->shapeList().at(index));
}

void AMSampleCameraView::updateCurrentTextItemName()
{
	if(isValid(current_))
	{
		if(currentView_ == NAME)
			textItems_[current_]->setPlainText(currentName());
	}
}

void AMSampleCameraView::setViewName()
{
	currentView_ = NAME;
	refreshSceneView();
}

void AMSampleCameraView::setViewOtherData()
{
	currentView_ = DATA;
	refreshSceneView();
}

void AMSampleCameraView::setViewIdNumber()
{
	currentView_ = ID;
	refreshSceneView();
}

void AMSampleCameraView::setViewHidden()
{
	currentView_ = HIDE;
	refreshSceneView();
}

void AMSampleCameraView::autoCompleteEnterPressed()
{
	//    shapeModel_->setCurrentInfo(autoCompleteBox_->text());
	refreshSceneView();
}

void AMSampleCameraView::beamShape(int shapeNumber)
{
	const QList<QPointF*>* points;
	if(beamWizard_ != 0)
		points = beamWizard_->getPointList();
	else return;
	QPointF topLeft = mapPointToVideo(*(points->at(2*shapeNumber)));
	QPointF bottomRight = mapPointToVideo(*(points->at(2*shapeNumber+1)));

	setDrawOnShapeEnabled(true);
	shapeModel_->setDrawOnSamplePlate();


	shapeModel_->setBeamMarker(topLeft,shapeNumber);
	shapeModel_->updateBeamMarker(bottomRight, shapeNumber);
	currentChanged();

	refreshSceneView();

	if(updateTracker_ == shapeNumber)
	{
		beamWizard_->updateShape(shapes_[current_]);
	}
	else
	{
		AMSampleCameraGraphicsView* newView = new AMSampleCameraGraphicsView();
		newView->setScene(shapeScene_->scene());
		beamWizard_->updateScene(newView);
	}

	updateTracker_ = shapeNumber;



}

void AMSampleCameraView::beamCalibrate()
{
	shapeModel_->beamCalibrate();
	showSamplePlate_->setChecked(true);
	refreshSceneView();
//	delete beamWizard_;
//	beamWizard_ = NULL;

}

void AMSampleCameraView::samplePlateCreate()
{
	qDebug()<<"Sample plate being created.";

	const QList<QPointF*>* pointList = samplePlateWizard_->getPointList();
	int numberOfPoints = samplePlateWizard_->numberOfPoints();
	/// create sample plate takes a QVector of QVector3D and a QVector of QPointF
	/// it also expects the order to be toplefta,topleftb,etc
	// need to convert pointList into the correct format
	if((pointList->count() - numberOfPoints - 1)%numberOfPoints  != 0)
	{
		qDebug()<<"AMSampleCameraView::samplePlateCreate - Invalid number of points. ";
		return;
	}
	int pointsPerSample = (pointList->count() - numberOfPoints -1)/numberOfPoints;
	// have a list divided into numberOfPoints sections, want to change it into one list that
	// interleaves the three sections ex:
	// "pointList" is 0abcd0efgh0ijkl0
	// "combinedPoints" is aeibfjcgkdhl
	QList<QPointF> *list = new QList<QPointF>[numberOfPoints];
	QVector<QPointF> combinedPoints;
	int index = 0;
	for(int i = 0; i < numberOfPoints; i++)
	{
		for(int j = 0; j < pointsPerSample; j++)
		{
			/// get each section into its own list
			index = (numberOfPoints+1)*i+1+j;
			if(index >= pointList->size() || index < 0)
			{
				qDebug()<<"AMSampleCameraView::samplePlateCreate - Invalid sample index"<<index;
				return;
			}
			list[i].append(*pointList->at(index));
		}
	}

	/// interleave the created lists
	while(!samplePointListEmpty(list,numberOfPoints))
	{
		for(int i = 0; i < numberOfPoints; i++)
		{
			if(!list[i].isEmpty())
			{
				combinedPoints<<(list[i].takeFirst());
			}
			else
			{
				qDebug()<<"AMSampleCameraView::samplePlateCreate - List"<<i<<"unexpectedly empty";
			}
		}
	}

	const QList<QVector3D*>* coordinateList = samplePlateWizard_->getCoordinateList();
	QVector<QVector3D> sampleCoordinateList;
	foreach(QVector3D* coordinate, *coordinateList)
	{
		sampleCoordinateList<<*coordinate;
	}

	const QList<double> *rotationsList = samplePlateWizard_->getRotationList();
	QVector<double> rotations;
	foreach(double angle, *rotationsList)
	{
		rotations<<angle;
	}
	shapeModel_->createSamplePlate(sampleCoordinateList,combinedPoints, rotations, numberOfPoints);

	//		delete samplePlateWizard_;
	//		samplePlateWizard_ = NULL;

}

void AMSampleCameraView::rotationConfiguration()
{
	qDebug()<<"Calling configure rotation";
	QList<QPointF*> pointList = *rotationWizard_->getPointList();
	QList<QVector3D*> coordinateList = *rotationWizard_->getCoordinateList();
	QList<double> rotationList = *rotationWizard_->getRotationList();
	int numberOfPoints = rotationWizard_->numberOfPoints();
	QVector<QVector3D> coordinates;
	QVector<QPointF> points;
	QVector<AMAngle *> rotations;
	foreach (QVector3D *coordinate, coordinateList)
	{
		coordinates<<*coordinate;
	}

	foreach (QPointF *point, pointList)
	{
		points<<*point;
	}

	foreach (double rotation, rotationList)
	{
		rotations<<new AMAngle(rotation, AMAngle::DEG);/// in degrees, from motor coord
	}

	if(pointList.count() != numberOfPoints || rotationList.count() != numberOfPoints)
	{
		qDebug()<<"AMSampleCameraView::rotationConfiguration - Incorrect number of points";
	}

	shapeModel_->configureRotation( coordinates, points, rotations, rotationWizard_->numberOfPoints());
	shapeModel_->saveRotationalOffset();

	foreach(AMAngle *angle, rotations)
	{
		delete angle;
	}

//	delete rotationWizard_;
//	rotationWizard_ = NULL;
}

bool AMSampleCameraView::samplePointListEmpty(QList<QPointF>*list, int numberOfPoints) const
{
    bool empty= true;
    for(int i = 0; i < numberOfPoints; i++)
    {
        empty &= list[i].isEmpty();
    }
    return empty;
}

/// moves the sample plate to the requested point.
/// also updates the scene of each wizard
void AMSampleCameraView::moveBeamSamplePlate(QVector3D coordinate)
{
	double rotation = sampleCamera()->motorRotation();
	moveBeamSamplePlate(coordinate,rotation);
}

void AMSampleCameraView::moveBeamSamplePlate(QVector3D coordinate, double rotation)
{
	if(!shapeModel_->motorMovementEnabled())
	{
		shapeModel_->setMotorCoordinate(coordinate.x(),coordinate.y(),coordinate.z(),rotation);
	}
	shapeModel_->moveMotorTo(coordinate, rotation);
	refreshSceneView();
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView();
	view->setScene(shapeScene_->scene());
	if(beamWizard_->isVisible())
		beamWizard_->updateScene(view);
	if(cameraWizard_->isVisible())
		cameraWizard_->updateScene(view);
	if(samplePlateWizard_->isVisible())
		samplePlateWizard_->updateScene(view);
	if(rotationWizard_->isVisible())
		rotationWizard_->updateScene(view);
}

void AMSampleCameraView::showBeamMarker(int index)
{
	shapeModel_->addBeamMarker(index);
	refreshSceneView();
	beamWizard_->updateScene(shapeScene_);
}

void AMSampleCameraView::transmitMotorMovementEnabled()
{
	emit motorMovementEnabled(shapeModel_->motorMovementEnabled());
}

void AMSampleCameraView::transmitMotorRotation()
{
    emit motorRotation(shapeModel_->motorRotation());
}

void AMSampleCameraView::updateShapeName(QString newName)
{
	if(currentView_ == NAME)
	{
		if(currentIndex() >= 0 && textItems_.count() > currentIndex())
			textItems_[currentIndex()]->setPlainText(shapeModel_->name(currentIndex()));
		else
			refreshSceneView();
	}
}

void AMSampleCameraView::updateDataOne(QString data)
{
	if(currentView_ == DATA)
	{
		if(currentIndex() >= 0 && textItems_.count() > currentIndex())
		{
			qDebug()<<"AMSampleCameraView::updateDataOne - attempting to update textbox otherdata";
			textItems_[currentIndex()]->setPlainText(shapeModel_->otherDataOne(currentIndex()));
		}
		else refreshSceneView();
	}
}

void AMSampleCameraView::updateDataTwo(QString data)
{
	if(currentView_ == ID)
	{
		if(currentIndex() >= 0 && textItems_.count() > currentIndex())
		{
			qDebug()<<"AMSampleCameraView::updateDataTwo - attempting to update textbox otherdatatwo";
			textItems_[currentIndex()]->setPlainText(shapeModel_->otherDataTwo(currentIndex()));
		}
		else refreshSceneView();
	}
}

void AMSampleCameraView::onShowSamplePlateStateChanged(bool state)
{
	refreshSceneView();
}

void AMSampleCameraView::onSamplePlateWizardFinished()
{
	showSamplePlate_->setChecked(true);
	shapeModel_->saveSamplePlate();
}

void AMSampleCameraView::setMotorCoordinate(double x, double y, double z, double r)
{
	shapeModel_->setMotorCoordinate(x,y,z,r);
	emit currentChanged();
	refreshSceneView();
}

double AMSampleCameraView::motorRotation() const
{
	return (shapeModel_->motorRotation());
}

double AMSampleCameraView::motorX() const
{
	return shapeModel_->motorX();
}

double AMSampleCameraView::motorY() const
{
	return shapeModel_->motorY();
}

double AMSampleCameraView::motorZ() const
{
	return shapeModel_->motorZ();
}

QString AMSampleCameraView::currentName() const
{
	return shapeModel_->currentName();
}

void AMSampleCameraView::setCurrentName(QString name)
{
	shapeModel_->setCurrentName(name);
}

QString AMSampleCameraView::currentInfo() const
{
	return shapeModel_->currentInfo();
}

void AMSampleCameraView::setCurrentInfo(const QString &info)
{
	shapeModel_->setCurrentInfo(info);
}

int AMSampleCameraView::currentIndex()
{
	return shapeModel_->currentIndex();
}

double AMSampleCameraView::xCoordinate()
{
	double coordinate = shapeModel_->currentCoordinate().x();
	return coordinate;
}

double AMSampleCameraView::yCoordinate()
{
	return shapeModel_->currentCoordinate().y();
}

double AMSampleCameraView::zCoordinate()
{

	return shapeModel_->currentCoordinate().z();
}

double AMSampleCameraView::rotation()
{
	return shapeModel_->rotation();
}

double AMSampleCameraView::tilt()
{
	return shapeModel_->tilt();
}

void AMSampleCameraView::setX(QString x)
{

	QVector3D currentCoordinate = shapeModel_->currentCoordinate();
	double y = currentCoordinate.y();
	double z = currentCoordinate.z();
	shapeModel_->setCoordinates(x.toDouble(),y,z);
}

void AMSampleCameraView::setY(QString y)
{
	QVector3D currentCoordinate = shapeModel_->currentCoordinate();
	double x = currentCoordinate.x();
	double z = currentCoordinate.z();
	shapeModel_->setCoordinates(x,y.toDouble(),z);
}

void AMSampleCameraView::setZ(QString z)
{
	QVector3D currentCoordinate = shapeModel_->currentCoordinate();
	double y = currentCoordinate.y();
	double x = currentCoordinate.x();
	shapeModel_->setCoordinates(x,y,z.toDouble());
}

void AMSampleCameraView::setRotation(QString rotation)
{
	shapeModel_->setRotation(rotation.toDouble());
}

void AMSampleCameraView::onMotorMoved()
{
	motorXEdit_->setText(QString::number(motorX()));
	motorYEdit_->setText(QString::number(motorY()));
	motorZEdit_->setText(QString::number(motorZ()));
	motorREdit_->setText(QString::number(motorRotation()));
	refreshSceneView();
}

void AMSampleCameraView::setUseMotorCoordinate(bool use)
{
	useMotorCoordinate_ = use;
}

void AMSampleCameraView::setUseCameraMatrix(bool use)
{
	shapeModel_->setUseCameraMatrix(use);
	shapeModel_->updateAllShapes();
	refreshSceneView();
}

void AMSampleCameraView::showCameraBeamWindow()
{
	configurationWindow_->show();
	configurationWindow_->adjustSize();
}


void AMSampleCameraView::setDrawOnShape()
{
	shapeModel_->setDrawOnShape();
}

void AMSampleCameraView::setDrawOnShapeEnabled(bool enable)
{
	shapeModel_->setDrawOnShapeEnabled(enable);
	drawOnShapePushButton_->setDisabled(!enable);
}

void AMSampleCameraView::reviewCameraConfiguration()
{
	bool review = false;
	const QList<QPointF*>* pointList = cameraWizard_->getPointList();
	foreach(QPointF* point, *pointList)
	{
		if(*point != QPointF(0,0))
			review = true;
	}

	const QList<QVector3D*>* coordinateList = cameraWizard_->getCoordinateList();
	QPointF positions[SAMPLEPOINTS];
	QVector3D coordinates[SAMPLEPOINTS];
	if(review)
	{
		for(int i = 0; i < SAMPLEPOINTS; i++)
		{
			positions[i] = mapPointToVideo(*pointList->at(i));
			coordinates[i] = *coordinateList->at(i);
		}
		shapeModel_->findCamera(positions,coordinates);
		shapeModel_->deleteCalibrationPoints();
		cameraConfiguration_->updateAll();
		shapeModel_->updateAllShapes();
		refreshSceneView();
	}
//	delete cameraWizard_;
//	cameraWizard_ = NULL;
}

void AMSampleCameraView::onMoveToBeamToggled(bool checked)
{
	if(checked)
		moveToBeam_->setText("Move to Beam");
	else
		moveToBeam_->setText("Move to Crosshair");

	shapeModel_->setMoveToBeam(checked);

}

void AMSampleCameraView::onMoveOnSamplePlateToggled(bool checked)
{
	if(checked)
	{
		moveOnSamplePlate_->setText("Movement on Sample Plate");
	}
	else
	{
		moveOnSamplePlate_->setText("Movement on Screen Plane");
	}
	shapeModel_->setMoveOnShape(checked);
}

void AMSampleCameraView::samplePlateSelected()
{
	samplePlateSelected_ = true;
}

void AMSampleCameraView::setTilt(QString tilt)
{
	double newTilt = tilt.toDouble();
	shapeModel_->setTilt(newTilt);
}



void AMSampleCameraView::toggleDistortion()
{
	shapeModel_->toggleDistortion();
	refreshSceneView();
}

void AMSampleCameraView::setMedia(QMediaContent url)
{
	shapeScene_->mediaPlayer()->setMedia(url);
	qDebug() << "Status? " << shapeScene_->mediaPlayer()->mediaStatus();
}

#include <QApplication>
void AMSampleCameraView::play()
{
	qDebug()<<"supported"<<QMediaPlayer::supportedMimeTypes();
	shapeScene_->mediaPlayer()->play();
}

QMediaPlayer* AMSampleCameraView::mediaPlayer() const
{
	return shapeScene_->mediaPlayer();
}

QPointF AMSampleCameraView::mapPointToVideo(QPointF position)
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

bool AMSampleCameraView::isValid(int index) const
{
	return (shapeModel_->isValid(index) && shapes_[index]);
}

bool AMSampleCameraView::loadBeam()
{
	AMDatabase* db = AMDatabase::database("user");
	QVariantList matchList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(), "id");

	if(matchList.count() <= 0)
		return false;

	bool* success = new bool(true);
	int id = matchList.last().toInt(success);

	if(!(*success))
		return false;

	AMBeamConfiguration* beamToLoad = new AMBeamConfiguration();
	beamToLoad->loadFromDb(db,id);
	shapeModel_->setBeamModel(beamToLoad);
	refreshSceneView();
	return true;
}

bool AMSampleCameraView::loadCamera()
{
	AMDatabase* db = AMDatabase::database("user");
	QVariantList matchList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(),"id");


	if(matchList.count() <= 0)
	{
		return false;
	}

	AMCameraConfiguration* cameraToLoad = new AMCameraConfiguration();
	bool* success = new bool(true);
	int id = matchList.last().toInt(success);

	if(!success)
	{
		return false;
	}

	cameraToLoad->loadFromDb(db,id);
	shapeModel_->setCameraModel(cameraToLoad);
	refreshSceneView();

	return true;
}

bool AMSampleCameraView::loadSamplePlate()
{
	QString samplePlateName = "Sample Plate";
	AMDatabase* db = AMDatabase::database("user");
	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "name", samplePlateName);

	if(matchList.count() <= 0)
		return false;

	AMSample* samplePlate = new AMSample();
	samplePlate->loadFromDb(db,matchList.last());

	AMControlSet *samplePositioner = AMBeamline::bl()->currentSamplePositioner();
	if(!samplePositioner){
		QMessageBox::warning(this, "Cannot Reload Sample Plate", "Sorry, Acquaman doesn't seem to be able to reload the sample plate because the beamline sample manipulator isn't connected", QMessageBox::Ok);
		return false;
	}

	QStringList positionsInNote = samplePlate->notes().split(';');
	for(int x = 0; x < samplePositioner->count(); x++){
		if(!samplePositioner->at(x)->withinTolerance(positionsInNote.at(x).toDouble())){

			QString initialMessageString = QString("Sorry, Acquaman doesn't seem to be able to reload the sample plate because the sample manipulator is at a different position");
			QString failureMessageString = QString("Motor:%1 Current:%2 Original:%3 Tolerance:%4").arg(samplePositioner->at(x)->name()).arg(samplePositioner->at(x)->value()).arg(positionsInNote.at(x).toDouble()).arg(samplePositioner->at(x)->tolerance());
			QString suggestedActionString = QString("Acquaman can solve this problem by moving the sample manipulator to ");
			for(int y = 0; y < samplePositioner->count(); y++)
				suggestedActionString.append(QString("%1: %2 ").arg(samplePositioner->at(y)->description()).arg(positionsInNote.at(y).toDouble()));
			int retVal = QMessageBox::warning(this, "Cannot Reload Sample Plate", QString("%1\n\n%2\n\n%3").arg(initialMessageString).arg(failureMessageString).arg(suggestedActionString), QMessageBox::Ok, QMessageBox::Cancel);
			switch(retVal){
			case QMessageBox::Ok:
				for(int y = 0; y < samplePositioner->count(); y++)
					samplePositioner->at(y)->move(positionsInNote.at(y).toDouble());

				break;
			case QMessageBox::Cancel:
				break;
			}

			return false;
		}
	}

	AMShapeData* samplePlateShape = samplePlate->sampleShapePositionData();
	drawOnShapeCheckBox_->setChecked(true);
	setDrawOnShapeEnabled(true);
	shapeModel_->setSamplePlate(samplePlateShape);
	refreshSceneView();
	return true;

}

bool AMSampleCameraView::loadRotationalOffset()
{
	QString rotationalOffsetName = "LastRotationalOffset";
	AMDatabase *db = AMDatabase::database("user");
	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMRotationalOffset>(), "name", rotationalOffsetName);

	if(matchList.count() <= 0)
	{
		return false;
	}

	AMRotationalOffset *rotationalOffset = new AMRotationalOffset();
	rotationalOffset->loadFromDb(db,matchList.last());
	shapeModel_->setRotationalOffset(rotationalOffset->rotationalOffset());
	delete rotationalOffset;
	refreshSceneView();

	if(showGrid_){

		AMShapeData *shapeData;
		QVector<QVector3D> *coordinates;

		for(int x = 1, size = 11; x < size; x+=2){
			shapeData = new AMShapeData();
			coordinates = new QVector<QVector3D>();
			coordinates->append(QVector3D(-11.5, -rotationalOffset->rotationalOffset().y(), x+0.5));
			coordinates->append(QVector3D( 11.5, -rotationalOffset->rotationalOffset().y(), x+0.5));
			coordinates->append(QVector3D( 11.5, -rotationalOffset->rotationalOffset().y(), x-0.5));
			coordinates->append(QVector3D(-11.5, -rotationalOffset->rotationalOffset().y(), x-0.5));
			shapeData->setCoordinateShape(*coordinates);
			shapeModel_->insertItem(shapeData);

			shapeData = new AMShapeData();
			coordinates = new QVector<QVector3D>();
			coordinates->append(QVector3D(-11.5, -rotationalOffset->rotationalOffset().y(), -x+0.5));
			coordinates->append(QVector3D( 11.5, -rotationalOffset->rotationalOffset().y(), -x+0.5));
			coordinates->append(QVector3D( 11.5, -rotationalOffset->rotationalOffset().y(), -x-0.5));
			coordinates->append(QVector3D(-11.5, -rotationalOffset->rotationalOffset().y(), -x-0.5));
			shapeData->setCoordinateShape(*coordinates);
			shapeModel_->insertItem(shapeData);

			shapeData = new AMShapeData();
			coordinates = new QVector<QVector3D>();
			coordinates->append(QVector3D(x+0.5, -rotationalOffset->rotationalOffset().y(), 11.5));
			coordinates->append(QVector3D(x+0.5, -rotationalOffset->rotationalOffset().y(), 11.5));
			coordinates->append(QVector3D(x-0.5, -rotationalOffset->rotationalOffset().y(), -11.5));
			coordinates->append(QVector3D(x-0.5, -rotationalOffset->rotationalOffset().y(), -11.5));
			shapeData->setCoordinateShape(*coordinates);
			shapeModel_->insertItem(shapeData);

			shapeData = new AMShapeData();
			coordinates = new QVector<QVector3D>();
			coordinates->append(QVector3D(-x+0.5, -rotationalOffset->rotationalOffset().y(), 11.5));
			coordinates->append(QVector3D(-x+0.5, -rotationalOffset->rotationalOffset().y(), 11.5));
			coordinates->append(QVector3D(-x-0.5, -rotationalOffset->rotationalOffset().y(), -11.5));
			coordinates->append(QVector3D(-x-0.5, -rotationalOffset->rotationalOffset().y(), -11.5));
			shapeData->setCoordinateShape(*coordinates);
			shapeModel_->insertItem(shapeData);
		}
	}

	return true;


}

AMSampleCamera* AMSampleCameraView::sampleCamera(){
	return shapeModel_;
}

void AMSampleCameraView::setCrosshairColor(const QColor &color)
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

void AMSampleCameraView::setCrosshairLineThickness(int thickness)
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

void AMSampleCameraView::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMSampleCameraView::setCrosshairVisible(bool crosshairVisible)
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

void AMSampleCameraView::setCrosshairLocked(bool doLock)
{
	shapeModel_->setCrosshairLocked(doLock);

	if(lockCrosshairCheckBox_->isChecked() != doLock)
	{
		lockCrosshairCheckBox_->blockSignals(true);
		lockCrosshairCheckBox_->setChecked(doLock);
		lockCrosshairCheckBox_->blockSignals(false);
	}
}

void AMSampleCameraView::setCameraModel(AMCameraConfiguration *model)
{
	shapeModel_->setCameraModel(model);
	refreshSceneView();
}

void AMSampleCameraView::intersection()
{
	if(shapeModel_->findIntersections())
	{
		if(!intersections_.isEmpty()) clearIntersections();
		createIntersectionShapes(shapeModel_->intersections());
	}
}

void AMSampleCameraView::hideCameraParameters(bool hide)
{
	cameraConfiguration_->hideCameraParameters(hide);
	configurationWindow_->adjustSize();
}

void AMSampleCameraView::startCameraWizard()
{
//	if(cameraWizard_)
//	{
		delete cameraWizard_;
//	}
	cameraWizard_ = new AMCameraConfigurationWizard();
	connect(cameraWizard_, SIGNAL(done()), this, SLOT(reviewCameraConfiguration()));
	connect(cameraWizard_, SIGNAL(done()), this, SIGNAL(cameraWizardFinished()));
	connect(cameraWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
	connect(cameraWizard_, SIGNAL(moveTo(QVector3D)), this, SLOT(moveBeamSamplePlate(QVector3D)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), cameraWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this,SIGNAL(moveSucceeded()), cameraWizard_, SLOT(testMoveSlot()));
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView(0);
	view->setScene(shapeScene_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
	cameraWizard_->setView(view);
	cameraWizard_->show();
}

void AMSampleCameraView::startBeamWizard()
{
//	if(beamWizard_)
//	{
		delete beamWizard_;
//	}
	beamWizard_ = new AMBeamConfigurationWizard();
	connect(beamWizard_, SIGNAL(showShape(int)), this, SLOT(beamShape(int)));
	connect(beamWizard_, SIGNAL(done()), this, SLOT(beamCalibrate()));
	connect(beamWizard_, SIGNAL(done()), this, SIGNAL(beamWizardFinished()));
	connect(beamWizard_, SIGNAL(moveTo(QVector3D)), this, SLOT(moveBeamSamplePlate(QVector3D)));
	connect(beamWizard_, SIGNAL(showBeamMarker(int)), this, SLOT(showBeamMarker(int)));
	connect(beamWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
	connect(this, SIGNAL(motorMovementEnabled(bool)), beamWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), beamWizard_, SIGNAL(moveSucceeded()));
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView(0);
	view->setScene(shapeScene_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
	beamWizard_->setView(view);
	beamWizard_->show();
	showSamplePlate_->setChecked(true);
}

void AMSampleCameraView::startSampleWizard()
{
//	if(samplePlateWizard_)
//	{
		delete samplePlateWizard_;
//	}
	samplePlateWizard_ = new AMSamplePlateWizard();
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView();
	connect(samplePlateWizard_, SIGNAL(done()), this, SLOT(samplePlateCreate()));
	connect(samplePlateWizard_, SIGNAL(done()), this, SIGNAL(samplePlateWizardFinished()));
	connect(samplePlateWizard_, SIGNAL(movePlate(int)), this, SLOT(moveSamplePlate(int)));
	connect(samplePlateWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
	connect(samplePlateWizard_, SIGNAL(moveTo(QVector3D,double)), this, SLOT(moveBeamSamplePlate(QVector3D,double)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), samplePlateWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), samplePlateWizard_, SIGNAL(moveSucceeded()));
	connect(samplePlateWizard_, SIGNAL(requestRotation()), this, SLOT(transmitMotorRotation()));
	connect(this, SIGNAL(motorRotation(double)), samplePlateWizard_, SLOT(setCurrentRotation(double)));
	view->setScene(shapeScene_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
	samplePlateWizard_->setView(view);
	samplePlateWizard_->show();
	showSamplePlate_->setChecked(true);
}

void AMSampleCameraView::startRotationWizard()
{
//	if(rotationWizard_)
//	{
		delete rotationWizard_;
//	}
	rotationWizard_ = new AMRotationWizard();
	AMSampleCameraGraphicsView* view = new AMSampleCameraGraphicsView();
	/// \todo other things here
	connect(rotationWizard_, SIGNAL(done()), this, SLOT(rotationConfiguration()));
	connect(rotationWizard_, SIGNAL(done()), this, SIGNAL(rotationWizardFinished()));
	connect(rotationWizard_, SIGNAL(requestMotorMovementEnabled()), this, SLOT(transmitMotorMovementEnabled()));
	connect(rotationWizard_, SIGNAL(moveTo(QVector3D,double)), this, SLOT(moveBeamSamplePlate(QVector3D,double)));
	connect(this, SIGNAL(motorMovementEnabled(bool)), rotationWizard_, SLOT(setMotorMovementEnabled(bool)));
	connect(this, SIGNAL(moveSucceeded()), rotationWizard_, SIGNAL(moveSucceeded()));
	view->setScene(shapeScene_->scene());
	view->setSceneRect(QRectF(QPointF(0,0),shapeScene_->size()));
	rotationWizard_->setView(view);
	rotationWizard_->show();

}


void AMSampleCameraView::setSamplePlate()
{
	shapeModel_->setSamplePlate();
	refreshSceneView();
}

void AMSampleCameraView::setCameraConfigurationShape()
{
	shapeModel_->setCameraConfigurationShape();
	refreshSceneView();
}

void AMSampleCameraView::moveSamplePlate(int movement)
{
	int relativeMovement = movement - samplePlateMovement_;
	samplePlateMovement_ = movement;
	shapeModel_->moveSamplePlate(relativeMovement);
	refreshSceneView();
	AMSampleCameraGraphicsView* view = shapeScene_;
	samplePlateWizard_->updateScene(view);
}

void AMSampleCameraView::showBeamOutline(bool show)
{
	showBeamOutline_ = show;
	refreshSceneView();
}

void AMSampleCameraView::moveTestSlot()
{
	emit moveSucceeded();
}


void AMSampleCameraView::requestUpdate(){
	refreshSceneView();
}

#include <QTextCursor>
void AMSampleCameraView::shapeDrawingFinished()
{
	if(mode_ == DRAW)
	{
		if(isValid(currentIndex()))
		{
			textItems_[currentIndex()]->setSelectAll(true);
			textItems_[currentIndex()]->setFocus();
			/*
   qDebug() << "Set focus, what is the selectedText()? " << textItems_[currentIndex()]->textCursor().selectedText();
   QTextCursor selectText = textItems_[currentIndex()]->textCursor();
   selectText.select(QTextCursor::Document);
   textItems_[currentIndex()]->setTextCursor(selectText);
   qDebug() << "Tried setting selected, what is the selectedText()? " << textItems_[currentIndex()]->textCursor().selectedText();
   */
		}
	}
}

void AMSampleCameraView::requestLoadBeam()
{
	bool success = loadBeam();
	if(success)
	{
		emit beamWizardFinished();
	}
	else
	{
		qDebug()<<"Loading default beam configuration.";
		shapeModel_->loadDefaultBeam();
	}
}

void AMSampleCameraView::requestLoadCamera()
{
	bool success = loadCamera();
	if(success)
	{
		emit cameraWizardFinished();
	}
	else
	{
		qDebug()<<"Loading default camera configuration";
		shapeModel_->loadDefaultCamera();
	}
}

void AMSampleCameraView::requestLoadSamplePlate()
{
	bool success = loadSamplePlate();
	if(success)
	{
		emit samplePlateWizardFinished();
	}
//	else
//	{
//		qDebug()<<"Loading default sample plate";
//		shapeModel_->loadDefaultSamplePlate();
//	}
}

void AMSampleCameraView::requestLoadRotationConfiguration()
{
	bool success = loadRotationalOffset();
	if(success)
	{
		emit rotationWizardFinished();
	}
	else
	{
		qDebug()<<"Need to load default rotational offset";
	}
}


void AMSampleCameraView::updateCurrentShape()
{
	emit updateShapes(current_);
	refreshSceneView();
}

void AMSampleCameraView::createIntersectionShapes(QVector<QPolygonF> shapes)
{
	/*
<<<<<<< HEAD
*/
    intersections_.clear();
    QPen pen(colour(SAMPLEPLATEINTERSECTION));
    QBrush brush(colour(SAMPLEPLATEINTERSECTION));
    QPolygonF polygon(QRectF(5,5,20,20));
    if(!shapes.isEmpty())
    {
	if(!showBeamOutline_)
	{
		pen.setColor(colour(HIDEINTERSECTION));
		brush.setColor(colour(HIDEINTERSECTION));
	}
	intersections_<<shapeScene_->scene()->addPolygon(polygon, pen, brush);
	intersections_[0]->setPolygon(shapes.last());
	shapes.remove(shapes.indexOf(shapes.last()));
	if(showBeamOutline_)
	{
		pen.setColor(colour(INTERSECTION));
		brush.setColor(colour(INTERSECTION));
	}
	for(int i = 1; !shapes.isEmpty(); i++)
	{
		intersections_<<shapeScene_->scene()->addPolygon(polygon,pen,brush);
		intersections_[i]->setPolygon(shapes.first());
		shapes.remove(0);
	}
    }
    /*
=======
	intersections_.clear();
	for(int i =0; !shapes.isEmpty(); i++)
	{
		QPen pen(colour(INTERSECTION));
		QBrush brush(colour(INTERSECTION));
		if(shapes.first() == shapes.last())
		{
			brush.setColor(colour(SAMPLEPLATEINTERSECTION));
			pen.setColor(colour(SAMPLEPLATEINTERSECTION));
		}

		if(!showBeamOutline_)
		{
			pen.setColor(colour(HIDEINTERSECTION));
			brush.setColor(colour(HIDEINTERSECTION));
		}
		QPolygonF polygon(QRectF(5,5,20,20));
		intersections_<<shapeScene_->scene()->addPolygon(polygon,pen,brush);
		intersections_[i]->setPolygon(shapes.first());
		if(shapes.first() == shapes.last())
		{
			///do stuff
		}

		shapes.remove(0);
	}
>>>>>>> 79443148503d2ac896f197767ce994576bd48390
*/
}

void AMSampleCameraView::clearIntersections()
{
	while(!intersections_.isEmpty())
	{
		shapeScene_->scene()->removeItem(intersections_.first());
		intersections_.remove(0);
	}
}

void AMSampleCameraView::resizeEvent(QResizeEvent *event)
{

	shapeScene_->resizeEvent(event);
	refreshSceneView();
}
#include <QMessageBox>
void AMSampleCameraView::mousePressHandler(QPointF position)
{
	if(shapeModel_->overrideMouseSelection())
	{
		//        shapeModel_->setOverrideMouseSelection(false);
		return;
	}
	connect(this, SIGNAL(mouseMove(QPointF)), this, SLOT(mouseMoveHandler(QPointF)));

	if(mode_ == DRAW && samplePlateSelected_)
	{
		emit mousePressed((position));
		connect(this,SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishRectangle(QPointF)));


	}
	else if(mode_ == DRAW && !samplePlateSelected_)
	{
		QMessageBox* message = new QMessageBox();
		message->setText("Cannot create sample without first creating or loading a sample plate.");
		message->exec();
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
		qDebug() << "Position: " << position << " crosshair: " << shapeModel_->crosshair();
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
	refreshSceneView();
}

void AMSampleCameraView::mouseRightClickHandler(QPointF position)
{
	connect(this, SIGNAL(mouseMove(QPointF)), this, SLOT(mouseMoveHandler(QPointF)));

	if (mode_ == MOVE)
	{
		emit mouseMoveRightPressed(position);
		currentSelectionChanged();
		connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(zoomShape(QPointF)));
		connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
	}

	else if (mode_ == EDIT)
	{
		emit mouseEditRightPressed(position);
		currentSelectionChanged();
		connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(rotateRectangle(QPointF)));
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
		refreshSceneView();
		currentSelectionChanged();

	}
	else emit mouseRightClicked((position));
	refreshSceneView();

}

void AMSampleCameraView::mouseLeftReleaseHandler(QPointF position)
{
	if(shapeModel_->overrideMouseSelection())
	{
		shapeModel_->setOverrideMouseSelection(false);
		return;
	}
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
	refreshSceneView();
}

void AMSampleCameraView::mouseRightReleaseHandler(QPointF position)
{
	emit mouseReleased((position));
	disconnect(shapeModel_, SLOT(zoomAllShapes(QPointF)));
	disconnect(this, SIGNAL(currentChanged()));
	disconnect(shapeModel_, SLOT(rotateRectangle(QPointF)));
	disconnect(shapeModel_, SLOT(zoomShape(QPointF)));
	disconnect(this, SLOT(mouseMoveHandler(QPointF)));
	refreshSceneView();
}



void AMSampleCameraView::mouseDoubleClickHandler(QPointF position)
{
	doubleClickInProgress_ = true;
	if(mode_ == MULTIDRAW)
	{
		emit mouseMultiDrawDoubleClicked(position);
		refreshSceneView();
		emit currentChanged();
	}
	else if(!crosshairLocked())
	{
		setCrosshairPosition(position);
	}

}

void AMSampleCameraView::mouseMoveHandler(QPointF position)
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


}






/// append a new rectangle to the current list, add it to the scene
void AMSampleCameraView::addNewShape()
{
	index_++;
	QPen pen(colour(BORDER));
	QBrush brush(colour(FILL));
	QPolygonF polygon(QRectF(5,5,20,20));
	shapes_.insert(index_, shapeScene_->scene()->addPolygon(polygon,pen,brush));
	AMGraphicsTextItem* newItem = new AMGraphicsTextItem();
	shapeScene_->scene()->addItem(newItem);
	textItems_.insert(index_,newItem) ;
	textItems_[index_]->setZValue(1000);
	textItems_[index_]->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextEditable);
	textItems_[index_]->setShapeIndex(index_);
	connect(textItems_[index_], SIGNAL(textChanged(int)), this, SLOT(updateItemName(int)));
	connect(textItems_[index_], SIGNAL(gotFocus(int)), shapeModel_, SLOT(setCurrentShapeIndex(int)));
	connect(textItems_[index_], SIGNAL(gotFocus(int)), this, SLOT(currentSelectionChanged()));
	connect(textItems_[index_], SIGNAL(returnPressed(int)), this, SLOT(updateItemReturnPressed(int)));
}

/// Remove a rectangle from the scene
void AMSampleCameraView::deleteShape()
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

/// change the currently selected item, outline it in blue
void AMSampleCameraView::currentSelectionChanged()
{
	if(isValid(current_))
        {
		shapes_[current_]->setPen(colour(BORDER));
        }
	current_ = shapeModel_->currentIndex();
	if(motorREdit_)
		motorREdit_->setText(QString::number(motorRotation()));
	if(motorXEdit_)
		motorXEdit_->setText(QString::number(motorX()));
	if(motorYEdit_)
		motorYEdit_->setText(QString::number(motorY()));
	if(motorZEdit_)
		motorZEdit_->setText(QString::number(motorZ()));


	if(isValid(current_))
	{
		shapes_[current_]->setPen(colour(ACTIVEBORDER));

	}
	refreshSceneView();

}


void AMSampleCameraView::createGroupRectangle()
{
	//QBrush penBrush(QColor(Qt::magenta));
	QBrush penBrush(Qt::magenta);
	QPen pen(penBrush,1,Qt::DotLine);
	QBrush brush(Qt::transparent);
	groupRectangle_ = shapeScene_->scene()->addPolygon(QPolygonF(QRectF(5,5,20,20)),pen,brush);
	groupRectangleActive_ = true;

}

void AMSampleCameraView::destroyGroupRectangle()
{
	shapeScene_->scene()->removeItem(groupRectangle_);
	groupRectangleActive_ = false;
}


void AMSampleCameraView::setGUI(ViewType viewType)
{

	///GUI Setup

	QMargins itemMargins(12,0,12,0);
	QMargins frameMargins(0,0,0,0);
	int space = 20;

	advancedButton_ = new QPushButton("Advanced");

	QFrame* crosshairFrame = new QFrame();
	QHBoxLayout* chl = new QHBoxLayout();
	chl->setContentsMargins(itemMargins);
	chl->addWidget(showCrosshairCheckBox_ = new QCheckBox("Crosshair:"));
	chl->addSpacing(space);
	//chl->addWidget(new QLabel("Color:"));
	chl->addWidget(crosshairColorPicker_ = new AMColorPickerButton2(Qt::red));
	//chl->addWidget(new QLabel("Line:"));
	chl->addWidget(crosshairThicknessSlider_ = new QSlider(Qt::Horizontal));
	crosshairThicknessSlider_->setMaximumWidth(80);
	crosshairThicknessSlider_->setRange(1,6);
	crosshairThicknessSlider_->setValue(1);
	chl->addSpacing(space);
	chl->addWidget(lockCrosshairCheckBox_ = new QCheckBox("Lock position"));

	showCrosshairCheckBox_->setVisible(false);
	crosshairColorPicker_->setVisible(false);
	crosshairThicknessSlider_->setVisible(false);
	lockCrosshairCheckBox_->setVisible(false);

	advancedButton_->setVisible(false);

	if(viewType == DEBUG)
	{
		chl->addSpacing(space);
		chl->addWidget(enableMotorMovement_ = new QCheckBox("Enable Motor Movement"));
		enableMotorMovement_->setChecked(false);
		chl->addSpacing(space);
		chl->addWidget(enableMotorTracking_ = new QCheckBox("Enable Motor Tracking"));
		enableMotorTracking_->setChecked(false);
		chl->addWidget(configureCameraButton_ = new QPushButton("Calibrate Camera"));
		chl->addSpacing(space);
		chl->addWidget(configurationWindowButton_ = new QPushButton("Configure Settings"));
	}
	else if(viewType == CONDENSED)
	{
		chl->addSpacing(space);
		chl->addWidget(advancedButton_);
	}
	chl->addStretch();
	crosshairFrame->setLayout(chl);
	showCrosshairCheckBox_->setChecked(true);

	QFrame* toolFrame  = new QFrame();
	QHBoxLayout* tfl = new QHBoxLayout();
	tfl->setContentsMargins(itemMargins);



	tfl->addWidget(toolBar_ = new QToolBar("Tool Bar"));
	if(viewType == DEBUG)
	{
		tfl->addSpacing(space);
		tfl->addWidget(motorXEdit_ = new QLineEdit());
		tfl->addSpacing(10);
		tfl->addWidget(motorYEdit_ = new QLineEdit());
		tfl->addSpacing(10);
		tfl->addWidget(motorZEdit_ = new QLineEdit());
		tfl->addSpacing(10);
		tfl->addWidget(motorREdit_ = new QLineEdit());
		tfl->addSpacing(space);
		tfl->addWidget(setMotorCoordinate_ = new QPushButton("Set"));
	}
	tfl->addStretch();
	toolFrame->setLayout(tfl);




	QFrame* shapeFrame = new QFrame();
	QHBoxLayout* shapeHorizontalLayout =  new QHBoxLayout();
	shapeHorizontalLayout->setContentsMargins(itemMargins);
	if(viewType == DEBUG)
	{
		shapeHorizontalLayout->addWidget(drawOnShapeCheckBox_ = new QCheckBox("Draw on shape"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(drawOnShapePushButton_ = new QPushButton("Select Shape"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(distortionButton_ = new QPushButton("Distortion"));
		shapeHorizontalLayout->addSpacing(space);
	}
	shapeHorizontalLayout->addWidget(labelToolBar_ = new QToolBar("Labels"));
	shapeHorizontalLayout->addSpacing(space);
	if(viewType == DEBUG)
	{
		shapeHorizontalLayout->addWidget(moveToBeam_ = new QCheckBox("Move to Beam"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(moveOnSamplePlate_ = new QCheckBox("Move on sample Plate"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(cameraWizardButton_ = new QPushButton("Camera Wizard"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(beamWizardButton_ = new QPushButton("Beam Wizard"));
		shapeHorizontalLayout->addSpacing(space);
		shapeHorizontalLayout->addWidget(samplePlateWizardButton_ = new QPushButton("Sample Wizard"));
	}
	shapeHorizontalLayout->addStretch();
	shapeFrame->setLayout(shapeHorizontalLayout);



	if(viewType == DEBUG)
	{
		drawOnShapeCheckBox_->setChecked(false);
		drawOnShapePushButton_->setDisabled(true);
	}


	QVBoxLayout *vbl = new QVBoxLayout();
	vbl->setContentsMargins(frameMargins);
	//vbl->addWidget(crosshairFrame);
	vbl->addWidget(shapeScene_);
	QHBoxLayout *toolBarHL = new QHBoxLayout();
	toolBarHL->addWidget(shapeFrame);
	toolBarHL->addWidget(toolFrame);
	toolBarHL->setContentsMargins(frameMargins);
	vbl->addLayout(toolBarHL);
	QHBoxLayout *drawOverlaysLayout = new QHBoxLayout();
	drawOverlaysLayout->addWidget(showBeamOutlineCheckBox_ = new QCheckBox("Show Beam"));
	drawOverlaysLayout->addWidget(showSamplePlate_ = new QCheckBox("Show Sample Plate"));
	drawOverlaysLayout->addStretch();
	vbl->addLayout(drawOverlaysLayout);
	//vbl->addWidget(shapeFrame);
	//vbl->addWidget(toolFrame);
	setLayout(vbl);


	configurationWindow_ = new QFrame();
	QVBoxLayout *cvl = new QVBoxLayout();
	cvl->setContentsMargins(frameMargins);
	cvl->addWidget(cameraConfiguration_);
	cvl->addWidget(beamConfiguration_);

	QFrame* samplePlateFrame = new QFrame();
	QHBoxLayout* samplePlateLayout = new QHBoxLayout();
	samplePlateLayout->setContentsMargins(itemMargins);
	samplePlateLayout->addWidget(samplePlateButton_ = new QPushButton("Set Sample Plate"));
	samplePlateLayout->addSpacing(space);
	samplePlateLayout->addWidget(saveSamplePlate_ = new QPushButton("Save Sample Plate"));
	//samplePlateLayout->addSpacing(space);
	//samplePlateLayout->addWidget(showSamplePlate_ = new QCheckBox("Show Sample Plate"));
	samplePlateLayout->addSpacing(space);
	samplePlateLayout->addWidget(cameraConfigurationShapeButton_ = new QPushButton("Set Outer Plate"));
	//samplePlateLayout->addStretch(space);
	//samplePlateLayout->addWidget(showBeamOutlineCheckBox_ = new QCheckBox("Show beam area"));
	samplePlateLayout->addStretch();
	samplePlateFrame->setLayout(samplePlateLayout);

	cvl->addWidget(samplePlateFrame);

	showSamplePlate_->setChecked(true);

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
		cchl[i]->setContentsMargins(itemMargins);
		cchl[i]->addWidget(pointLineEdit_[2*i]);
		cchl[i]->addSpacing(space);
		cchl[i]->addWidget(pointLineEdit_[2*i+1]);
		cchl[i]->addSpacing(space);
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
		clhl[i]->setContentsMargins(itemMargins);
		for(int j = 0; j < 3; j++)
		{
			clhl[i]->addWidget(coordinateLineEdit_[3*i+j]);
			clhl[i]->addSpacing(space);
		}
		clhl[i]->addStretch();
		coordinateLineFrame[i]->setLayout(clhl[i]);
	}


	QFrame* configurationOptionsFrame = new QFrame();
	QHBoxLayout* cohl = new QHBoxLayout();
	cohl->setContentsMargins(itemMargins);
	cohl->addWidget(new QLabel("Use motor coordinates:"));
	cohl->addWidget(motorCoordinateCheckBox_ = new QCheckBox());
	cohl->addSpacing(space);
	cohl->addWidget(deleteCalibrationPoints_ = new QPushButton("Delete calibration points"));
	cohl->addWidget(new QLabel("Use camera Matrix"));
	cohl->addWidget(cameraMatrixCheckBox_ = new QCheckBox());
	cohl->addStretch();
	configurationOptionsFrame->setLayout(cohl);

	/// camera configuration window (finds camera)
	cameraConfigurationWindow_ = new QFrame();

	QVBoxLayout* ccvl = new QVBoxLayout();
	ccvl->setContentsMargins(frameMargins);
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
	markAction_->setCheckable(true);
	markAction_->setChecked(true);
	moveAction_ = new QAction("Move", actionGroup);
	moveAction_->setCheckable(true);
	editAction_ = new QAction("Edit", actionGroup);
	editAction_->setCheckable(true);
	shiftAction_ = new QAction("Shift", actionGroup);
	shiftAction_->setCheckable(true);
	//operationAction_ = new QAction("Operation", actionGroup);
	operationAction_ = new QAction("Click to Move", actionGroup);
	operationAction_->setCheckable(true);
	groupAction_ = new QAction("Group", actionGroup);
	groupAction_->setCheckable(true);









	toolBar_->addAction(markAction_);
	toolBar_->addAction(moveAction_);
	toolBar_->addAction(editAction_);
	if(viewType == DEBUG)
		toolBar_->addAction(shiftAction_);
	toolBar_->addAction(operationAction_);
	if(viewType == DEBUG)
		toolBar_->addAction(groupAction_);

	markAction_->setToolTip("Press twice for polygon marking.");

	QActionGroup* labelActionGroup = new QActionGroup(this);
	viewName_ = new QAction("Name", labelActionGroup);
	viewOtherData_ = new QAction("Tag", labelActionGroup);
	viewIdNumber_ = new QAction("Elements", labelActionGroup);
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

	if(viewType == CONDENSED)
	{
		advancedWindow_ = new QFrame();
		QFrame* topBar = new QFrame();
		QFrame* middleBar = new QFrame();
		QFrame* rotationalBar = new QFrame();
		QFrame* bottomBar = new QFrame();

		QHBoxLayout* topLayout = new QHBoxLayout();
		topLayout->setContentsMargins(itemMargins);
		topLayout->addWidget(enableMotorMovement_ = new QCheckBox("Enable Motor Movement"));
		topLayout->addSpacing(space);
		topLayout->addWidget(enableMotorTracking_ = new QCheckBox("Enable Motor Tracking"));
		topLayout->addSpacing(space);
		topLayout->addWidget(configureCameraButton_ = new QPushButton("Calibrate Camera"));
		topLayout->addSpacing(space);
		topLayout->addWidget(configurationWindowButton_ = new QPushButton("Configure Settings"));
		topLayout->addStretch();
		topBar->setLayout(topLayout);

		QHBoxLayout* bottomLayout = new QHBoxLayout();
		bottomLayout->setContentsMargins(itemMargins);
		bottomLayout->addWidget(drawOnShapeCheckBox_ = new QCheckBox("Draw on Shape"));
		bottomLayout->addSpacing(space);
		bottomLayout->addWidget(drawOnShapePushButton_ = new QPushButton("Select Shape"));
		bottomLayout->addSpacing(space);
		bottomLayout->addWidget(distortionButton_ = new QPushButton("Toggle Distortion"));
		bottomLayout->addSpacing(space);
		bottomLayout->addWidget(moveToBeam_ = new QCheckBox("Move to Beam"));
		bottomLayout->addSpacing(space);
		bottomLayout->addWidget(moveOnSamplePlate_ = new QCheckBox("Move on Sample Plate"));
		bottomLayout->addStretch();
		bottomBar->setLayout(bottomLayout);

		QHBoxLayout* middleLayout = new QHBoxLayout();
		middleLayout->setContentsMargins(itemMargins);
		middleLayout->addWidget(loadDefaultBeam_ = new QPushButton("Load Default Beam"));
		middleLayout->addSpacing(space);
		middleLayout->addWidget(loadDefaultCamera_ = new QPushButton("Load Default Camera"));
		middleLayout->addSpacing(space);
		middleLayout->addWidget(loadDefaultSamplePlate_ = new QPushButton("Load Default Sample Plate"));
		middleLayout->addStretch();
		middleBar->setLayout(middleLayout);

		QHBoxLayout* rotationalLayout = new QHBoxLayout();
		rotationalLayout->setContentsMargins(itemMargins);
		rotationalLayout->addWidget(new QLabel("Motor Rotation Offset"));
		for(int i = 0; i < 3; i++)
		{
			rotationalLayout->addWidget(rotationalOffset_[i] = new QLineEdit());
			rotationalLayout->addSpacing(space);
		}
		rotationalLayout->addStretch();
		rotationalBar->setLayout(rotationalLayout);

		QVBoxLayout* advancedLayout = new QVBoxLayout();
		advancedLayout->setContentsMargins(frameMargins);
		advancedLayout->addWidget(topBar);
		advancedLayout->addWidget(middleBar);
		advancedLayout->addWidget(rotationalBar);
		advancedLayout->addWidget(bottomBar);
		advancedWindow_->setLayout(advancedLayout);

		motorXEdit_ = 0;
		motorYEdit_ = 0;
		motorZEdit_ = 0;
		motorREdit_ = 0;

		drawOnShapeCheckBox_->setChecked(false);
		drawOnShapePushButton_->setDisabled(true);

		if(QApplication::instance()->arguments().contains("--enableMotorMovement"))
			enableMotorMovement_->setChecked(true);
		else
			enableMotorMovement_->setChecked(false);

		enableMotorTracking_->setChecked(false);



	}

	moveToBeam_->setChecked(true);
	moveOnSamplePlate_->setChecked(true);

	bool defaultUseCameraMatrix = true;
	cameraMatrixCheckBox_->setChecked(defaultUseCameraMatrix);
	setUseCameraMatrix(defaultUseCameraMatrix);
	hideCameraParameters(defaultUseCameraMatrix);

}

void AMSampleCameraView::makeConnections(ViewType viewType)
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
	connect(this, SIGNAL(mouseMoveRightPressed(QPointF)), shapeModel_, SLOT(setZoomPoint(QPointF)));
	connect(this, SIGNAL(mouseEditPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
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
	if(viewType == DEBUG)
	{
		connect(setMotorCoordinate_, SIGNAL(clicked()), this, SLOT(setMotorCoordinatePressed()));
		connect(shapeModel_, SIGNAL(onMotorMoved()), this, SLOT(onMotorMoved()));
		connect(cameraWizardButton_, SIGNAL(clicked()), this, SLOT(startCameraWizard()));
		connect(beamWizardButton_, SIGNAL(clicked()), this, SLOT(startBeamWizard()));
		connect(samplePlateWizardButton_, SIGNAL(clicked()), this, SLOT(startSampleWizard()));
	}



	/// shape view
	connect(this, SIGNAL(applyDistortion()), this, SLOT(toggleDistortion()));


	connect(cameraConfiguration_, SIGNAL(update(AMCameraConfiguration*)), this, SLOT(setCameraModel(AMCameraConfiguration*)));


	//    connect(shapeView_, SIGNAL(updateShapes()), this, SLOT(updateCurrentShape()));
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
	connect(saveSamplePlate_, SIGNAL(clicked()), shapeModel_, SLOT(saveSamplePlate()));
	connect(showSamplePlate_, SIGNAL(toggled(bool)), this, SLOT(onShowSamplePlateStateChanged(bool)));

	connect(cameraConfigurationShapeButton_, SIGNAL(clicked()), this, SLOT(setCameraConfigurationShape()));

	connect(showBeamOutlineCheckBox_, SIGNAL(toggled(bool)), this, SLOT(showBeamOutline(bool)));

	connect(drawOnShapePushButton_, SIGNAL(clicked()), this, SLOT(setDrawOnShape()));
	connect(drawOnShapeCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setDrawOnShapeEnabled(bool)));
	connect(shapeModel_, SIGNAL(drawOnShapeEnabledChanged(bool)), drawOnShapeCheckBox_, SLOT(setChecked(bool)));


	connect(distortionButton_, SIGNAL(clicked()), this, SIGNAL(applyDistortion()));



	/// allows non-rectangle drawing

	connect(pressTimer_, SIGNAL(timeout()), this, SLOT(setMultiDrawMode()));
	connect(this, SIGNAL(modeChange()), this, SLOT(changeDrawButtonText()));

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
	connect(shapeModel_, SIGNAL(shapeNameChanged(QString)), this, SLOT(updateShapeName(QString)));
	connect(shapeModel_, SIGNAL(otherDataOneChanged(QString)), this, SLOT(updateDataOne(QString)));
	connect(shapeModel_, SIGNAL(otherDataTwoChanged(QString)), this, SLOT(updateDataTwo(QString)));

	//connect(advancedButton_, SIGNAL(clicked()), advancedWindow_, SLOT(show()));
	connect(advancedButton_, SIGNAL(clicked()), this, SLOT(showAdvancedWindow()));
	connect(moveToBeam_, SIGNAL(toggled(bool)), this, SLOT(onMoveToBeamToggled(bool)));
	connect(moveOnSamplePlate_, SIGNAL(toggled(bool)), this, SLOT(onMoveOnSamplePlateToggled(bool)));

	connect(shapeModel_, SIGNAL(shapeFinished()), this, SLOT(shapeDrawingFinished()));


	connect(shapeModel_, SIGNAL(cameraConfigurationChanged(AMCameraConfiguration*)), cameraConfiguration_, SLOT(onCameraConfigurationChanged(AMCameraConfiguration*)));
	connect(this, SIGNAL(samplePlateWizardFinished()), this, SLOT(onSamplePlateWizardFinished()));

	connect(shapeModel_, SIGNAL(shapeDataChanged()), this, SLOT(refreshSceneView()));

	connect(loadDefaultBeam_, SIGNAL(clicked()), shapeModel_, SLOT(loadDefaultBeam()));
	connect(loadDefaultCamera_, SIGNAL(clicked()), shapeModel_, SLOT(loadDefaultCamera()));
	connect(loadDefaultSamplePlate_, SIGNAL(clicked()), shapeModel_, SLOT(loadDefaultSamplePlate()));

	if(viewType == CONDENSED)
	{
		connect(rotationalOffset_[0], SIGNAL(textEdited(QString)), this, SLOT(setRotationOffsetX(QString)));
		connect(rotationalOffset_[1], SIGNAL(textEdited(QString)), this, SLOT(setRotationOffsetY(QString)));
		connect(rotationalOffset_[2], SIGNAL(textEdited(QString)), this, SLOT(setRotationOffsetZ(QString)));
		connect(shapeModel_, SIGNAL(rotationalOffsetChanged(QVector3D)), this, SLOT(onRotationalOffsetChanged(QVector3D)));
	}

	onRotationalOffsetChanged(shapeModel_->rotationalOffset());


}

QColor AMSampleCameraView::colour(AMSampleCameraView::ShapeColour role)
{
	switch(role)
	{
	case ACTIVEBORDER:
		return QColor(Qt::blue);
	case BORDER:
		return QColor(Qt::red);
	case INTERSECTION:
		return QColor(Qt::yellow);
	case SAMPLEPLATEINTERSECTION:
		return QColor(Qt::green);
	case SAMPLEBORDER:
		return QColor(Qt::cyan);
	case SAMPLEFILL:
	case HIDEINTERSECTION:
	case BACKWARDSFILL:
	case FILL:
	default:
		return QColor(Qt::transparent);
	}
}

void AMSampleCameraView::drawSamplePlate()
{
	QPen pen(colour(SAMPLEBORDER));
	QBrush brush(colour(SAMPLEFILL));
	QPolygonF samplePlate = shapeModel_->samplePlate();
	if(!samplePlate.isEmpty())
	{

		if(!(samplePlate_ && samplePlate_->polygon() == samplePlate))
		{
                    if(!samplePlate_)
                    {
                        qDebug()<<"AMSampleCameraView::drawSamplePlate - samplePlate_ is null";
                    }
			shapeScene_->scene()->removeItem(samplePlate_);
			samplePlate_ = shapeScene_->scene()->addPolygon(samplePlate, pen, brush);
		}
	}
	if(samplePlate_)
		samplePlate_->setVisible(showSamplePlate_->isChecked());
}

void AMSampleCameraView::setRotationOffsetX(QString offset)
{
	shapeModel_->setRotationalOffsetX(offset.toDouble());
}

void AMSampleCameraView::setRotationOffsetY(QString offset)
{
	shapeModel_->setRotationalOffsetY(offset.toDouble());
}

void AMSampleCameraView::setRotationOffsetZ(QString offset)
{
	shapeModel_->setRotationalOffsetZ(offset.toDouble());
}

void AMSampleCameraView::onRotationalOffsetChanged(QVector3D offset)
{
	QString x = QString("%1").arg(offset.x());
	QString y = QString("%1").arg(offset.y());
	QString z = QString("%1").arg(offset.z());
	if(rotationalOffset_[0]->text() != x && !rotationalOffset_[0]->hasFocus())
		rotationalOffset_[0]->setText(x);
	if(rotationalOffset_[1]->text() != y && !rotationalOffset_[1]->hasFocus())
		rotationalOffset_[1]->setText(y);
	if(rotationalOffset_[2]->text() != z && !rotationalOffset_[2]->hasFocus())
		rotationalOffset_[2]->setText(z);

	refreshSceneView();
}

void AMSampleCameraView::showAdvancedWindow(){
	if(advancedWindow_)
		advancedWindow_->show();
}


