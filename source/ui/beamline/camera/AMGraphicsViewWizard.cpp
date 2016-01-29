/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMGraphicsViewWizard.h"

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#endif

#include <QPointF>
#include <QLayout>
#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <QVector3D>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QAbstractButton>
#include <QCheckBox>
#include <QLineEdit>

#include "AMQEvents.h"

#include "beamline/camera/AMGraphicsVideoSceneCopier.h"
#include "AMSampleCameraGraphicsView.h"
#include "util/AMErrorMonitor.h"

AMGraphicsViewWizard::AMGraphicsViewWizard(QWidget* parent)
	:QWizard(parent)
{
	view_ = new AMSampleCameraGraphicsView();
	scale_ = new QPointF(1,1);
	pointList_ = new QList<QPointF*>();
	pointList_->clear();
	coordinateList_ = new QList<QVector3D*>();
	coordinateList_->clear();
	rotations_ = new QList<double>();
	rotations_->clear();
	showOptionPage_ = false;
	optionsPage_ = -1;
	motorMovementEnabled_ = false;
	rotationEnabled_ = false;

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	mediaPlayer_ = new QMediaPlayer();
	#endif

	/// set the default free page. Make sure to set if not using
	/// default pages.  Set through setFreePage(freePageNumber)
	freePage_ = Default_Free;


	connect(QWizard::button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));
}

AMGraphicsViewWizard::~AMGraphicsViewWizard()
{
	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QGraphicsVideoItem* videoItem = 0;
	foreach(QGraphicsItem* item, view_->scene()->items())
	{
		if(item->type() == QGraphicsItem::UserType)
		{
			QGraphicsVideoItem* potentialItem = qgraphicsitem_cast<QGraphicsVideoItem*>(item);
			if(potentialItem)
			{
				videoItem = potentialItem;
			}
		}
	}
	if(videoItem)
	{
		QMediaPlayer* player = (QMediaPlayer*)videoItem->mediaObject();
		player->stop();
		view_->scene()->removeItem(videoItem);
		player->deleteLater();
		videoItem->deleteLater();
	}
	#endif
}

AMSampleCameraGraphicsView *AMGraphicsViewWizard::view() const
{
	return view_;
}

QPointF AMGraphicsViewWizard::scale() const
{
	return *scale_;
}

/// Sets the scale of the video
void AMGraphicsViewWizard::setScale(QPointF scale)
{
	/// first undo previous scaling
	view_->scale(1/scale_->x(), 1/scale_->y());
	*scale_ = scale;
	/// apply the new scaling
	view_->scale(scale_->x(),scale_->y());
}

void AMGraphicsViewWizard::setScale(double scaleFactor)
{
	QPointF scalePoint(scaleFactor,scaleFactor);
	setScale(scalePoint);
}

void AMGraphicsViewWizard::waitPage()
{
	if(relativeId() >= 0 && relativeId() < coordinateList()->count() && coordinateList()->at(relativeId()))
		emit moveTo(*coordinateList()->at(relativeId()));
}

QList<QPointF *> *AMGraphicsViewWizard::pointList() const
{
	return pointList_;
}

QList<QVector3D *> *AMGraphicsViewWizard::coordinateList() const
{
	return coordinateList_;
}

QList<double> *AMGraphicsViewWizard::rotations() const
{
	return rotations_;
}

void AMGraphicsViewWizard::copyView(AMSampleCameraGraphicsView *newView, AMSampleCameraGraphicsView *viewToCopy)
{

	newView->setAcceptDrops(viewToCopy->acceptDrops());
	newView->setAccessibleDescription(viewToCopy->accessibleDescription());
	newView->setAccessibleName(viewToCopy->accessibleName());
	newView->setAlignment(viewToCopy->alignment());
	newView->setAutoFillBackground(viewToCopy->autoFillBackground());
	newView->setBackgroundBrush(viewToCopy->backgroundBrush());
	newView->setBackgroundRole(viewToCopy->backgroundRole());
	newView->setBaseSize(viewToCopy->baseSize());
	newView->setCacheMode(viewToCopy->cacheMode());
	newView->setContentsMargins(viewToCopy->contentsMargins());
	newView->setContextMenuPolicy(viewToCopy->contextMenuPolicy());
	newView->setFrameRect(viewToCopy->frameRect());

}

int AMGraphicsViewWizard::numberOfPoints() const
{
	return numberOfPoints_;
}

int AMGraphicsViewWizard::numberOfPages() const
{
	return numberOfPages_;
}

void AMGraphicsViewWizard::setPoint(QPointF point, int index)
{
	if(pointList_->count() > index)
		(*pointList_->at(index)) = point;
}

void AMGraphicsViewWizard::setCoordinate(QVector3D coordinate, int index)
{
	if(coordinateList_->count() > index)
		(*coordinateList_->at(index)) = coordinate;
}

void AMGraphicsViewWizard::setRotation(double rotation, int index)
{
	if(rotations_->count() > index)
		rotations_->replace(index,rotation);
}

/// defines text in one location
QString AMGraphicsViewWizard::message(int type)
{
	// example of intended use:
	// switch(pageNumber)
	//  case page1:
	//      switch(type)
	//          case Title:
	//              return "Title Text"
	//          case Help:
	//              return "Help message text"
	//  etc.
	//  when using non-specified wait/set pages, put in default case:
	//	default:
	//		if(isSetPage(pageNumber)
	//			...
	//		else if(isWaitPage(pageNumber)
	//			...
	if(type)
	{
		return "Default message";
	}
	else
	{
		return "Default Error message";
	}
}

/// get a point in relative coordinates, so that it may be mapped correctly in the coordinate space
QPointF AMGraphicsViewWizard::mapPointToVideo(QPointF point) const
{
    Q_UNUSED(point)
	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QList<QGraphicsItem*> list = view()->items();
	QGraphicsVideoItem* videoItem;
	// might break if can't find video
	foreach(QGraphicsItem* item, list)
	{
		/// QGraphicsVideoItem has type QGraphicsItem::UserType
		if(item->type() == QGraphicsItem::UserType)
		{
			videoItem = (QGraphicsVideoItem*)item;
		}
	}
	QPointF topLeft = videoItem->sceneBoundingRect().topLeft();
	QPointF bottomRight = videoItem->sceneBoundingRect().bottomRight();
	QPointF sceneTopLeft = view()->mapSceneToVideo(topLeft);
	QPointF sceneBottomRight = view()->mapSceneToVideo(bottomRight);
	double left = sceneTopLeft.x();
	double right = sceneBottomRight.x();
	double top = sceneTopLeft.y();
	double bottom = sceneBottomRight.y();

	double positionX = (point.x() - left)/(right-left);
	double positionY = (point.y() - top)/(bottom - top);

	return QPointF(positionX, positionY);
	#else
	return QPointF(0, 0);
	#endif
}

bool AMGraphicsViewWizard::checked(int page) const
{
	return field(QString("configured%1").arg(page)).toBool();
}

/// call this function to make the option page accessible from
/// page with page number id.
void AMGraphicsViewWizard::addOptionPage(int id)
{
	// to access the "option" page, return the desired page
	// for the condition showOptionsPage().
	setOption(HaveCustomButton1);
	setButtonText(CustomButton1,"Options");
	connect(this, SIGNAL(customButtonClicked(int)), this, SLOT(showOptions(int)));
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(showOptionsButton(int)));
	setOptionPage(id);
}

bool AMGraphicsViewWizard::showOptionPage() const
{
	return showOptionPage_;
}

void AMGraphicsViewWizard::setOptionPage(int id)
{
	optionsPage_ = id;
}

/// after checking motorMovementEnabled set it to false
/// this prevents accidently moving when you shouldn't
bool AMGraphicsViewWizard::motorMovementEnabled()
{
	bool returnValue = motorMovementEnabled_;
	motorMovementEnabled_ = false;
	return returnValue;
}

/// call this prior to motorMovementEnabled to set the state correctly
void AMGraphicsViewWizard::checkMotorMovementState()
{
	emit requestMotorMovementEnabled();
}

void AMGraphicsViewWizard::setView(AMSampleCameraGraphicsView *view)
{
	view_ = view;

	view_->setObjectName("AMGraphicsViewWizard view 1");

	AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
	copier->cloneScene(view->scene());
	view_->setScene(copier->scene());
	view_->scene()->setObjectName("AMGraphicsViewWizard scene 1");

	/// fixItem_ prevents the screen from flashing or going blank
	/// when there is a QGraphicsItem or a descendent in less than
	/// all the views, but at least one.
	/// It seems that placing a QGraphicsTextItem with it's bounding rectangle
	/// touching the current view's boundaries (adjusted for scrolling) will
	/// stop the behaviour.
	fixItem_ = new QGraphicsTextItem("Fix");
	fixItem_->setObjectName("Fix");
	// make sure it is at the back
	fixItem_->setZValue(INT_MAX);
	// keep it just above the top left corner
	fixItem_->setPos(-1*fixItem_->boundingRect().width(), -1*fixItem_->boundingRect().height());
	view_->scene()->addItem(fixItem_);


	view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	view_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	view_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	fixText();

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	/// get the mediaObject
	foreach(QGraphicsItem* item, view_->scene()->items())
	{
		if(item->type() == QGraphicsItem::UserType)
		{
			QGraphicsVideoItem* videoItem = qgraphicsitem_cast<QGraphicsVideoItem*>(item);
			if(videoItem)
			{
				QMediaPlayer* videoPlayer = qobject_cast<QMediaPlayer*>(videoItem->mediaObject());
				if(videoPlayer)
				{
					mediaPlayer_ = videoPlayer;
				}
			}
		}
	}

	connect(mediaPlayer_, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaPlayerErrorChanged(QMediaPlayer::Error)));
	#endif
}

void AMGraphicsViewWizard::showHelp()
{
	QMessageBox::information(this, message(Help_Title), message(Help));
}



void AMGraphicsViewWizard::addPoint(QPointF position)
{
	QPointF* newPoint = new QPointF();
	*newPoint = position;
	pointList_->append(newPoint);
}

/// updates the wizard scene to match view
void AMGraphicsViewWizard::updateScene(AMSampleCameraGraphicsView *view)
{
	view_->scene()->removeItem(fixItem_);

	view_->setSceneRect(view->sceneRect());
	view_->setFrameRect(view->frameRect());

	AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
	copier->updateChange(view_->scene(),view->scene());
	delete(copier);

	view_->scene()->addItem(fixItem_);


}

void AMGraphicsViewWizard::updateShape(QGraphicsPolygonItem *item)
{
	AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
	copier->updateShape(item,view_->scene());
	delete(copier);
}


/// Readjusts the "fix text"
void AMGraphicsViewWizard::fixText()
{
	QRect visibleRect = view_->viewport()->rect();

	visibleRect.moveTopLeft((QPoint(view_->horizontalScrollBar()->value(),
					view_->verticalScrollBar()->value())));

	QPointF fixLocation = visibleRect.topLeft() - (fixItem_->boundingRect().bottomRight());
	fixItem_->setPos(fixLocation);

}

void AMGraphicsViewWizard::testMoveSlot()
{
	emit moveSucceeded();
}

void AMGraphicsViewWizard::setHasHelpButton(bool hasHelp)
{
	setOption(HaveHelpButton, hasHelp);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}

void AMGraphicsViewWizard::setDefaultWindowTitle()
{
	QWizard::setWindowTitle(message(Wizard_Title));
}

void AMGraphicsViewWizard::showOptions(int id)
{
	if(id == CustomButton1)
	{
		showOptionPage_ = true;
		next();
		showOptionPage_ = false;
	}
}

void AMGraphicsViewWizard::showOptionsButton(int id)
{
	if(id == optionsPage_)
	{
		button(CustomButton1)->show();
	}
	else if(button(CustomButton1)->isVisible())
	{
		button(CustomButton1)->hide();
	}
}

#ifdef AM_MOBILITY_VIDEO_ENABLED
void AMGraphicsViewWizard::mediaPlayerStateChanged(QMediaPlayer::MediaStatus state)
{
	if(state == QMediaPlayer::InvalidMedia)
	{
		// attempt to reload
		QUrl mediaUrl = mediaPlayer_->media().canonicalUrl();
		mediaPlayer_->stop();
		mediaPlayer_->setMedia(mediaUrl);
		mediaPlayer_->play();
	}
}

void AMGraphicsViewWizard::mediaPlayerErrorChanged(QMediaPlayer::Error state)
{
	if(state == QMediaPlayer::NetworkError)
	{
		QUrl mediaUrl = mediaPlayer_->media().canonicalUrl();
		QGraphicsVideoItem* videoItem;
		bool success = false;
		foreach(QGraphicsItem* item, view_->scene()->items())
		{
			if(item->type() == QGraphicsItem::UserType)
			{
				videoItem = qgraphicsitem_cast<QGraphicsVideoItem*>(item);
				if(videoItem) success = true;
			}
		}


		mediaPlayer_->stop();
		mediaPlayer_->setMedia(QMediaContent());
		// attempt to restart player
		if(success)
		{
			mediaPlayer_->deleteLater();
			mediaPlayer_ = new QMediaPlayer();
			mediaPlayer_->setMedia(mediaUrl);
			mediaPlayer_->setVideoOutput(videoItem);
			mediaPlayer_->play();
		}


	}
}
#endif

void AMGraphicsViewWizard::setRotationEnabled(bool rotationEnabled)
{
	rotationEnabled_ = rotationEnabled;
}

void AMGraphicsViewWizard::setNumberOfPages(int numberOfPages)
{
	numberOfPages_ = numberOfPages;
}

void AMGraphicsViewWizard::setNumberOfPoints(int numberOfPoints)
{
	numberOfPoints_ = numberOfPoints;
}

void AMGraphicsViewWizard::setShowOptionPage(bool showOptionPage)
{
	showOptionPage_ = showOptionPage;
}

void AMGraphicsViewWizard::coordinateListAppend(QVector3D *coordinate)
{
	coordinateList_->append(coordinate);
}

void AMGraphicsViewWizard::pointListAppend(QPointF *point)
{
	pointList_->append(point);
}

void AMGraphicsViewWizard::rotationsAppend(double rotation)
{
	rotations_->append(rotation);
}

void AMGraphicsViewWizard::setFreePage(int freePage)
{
	freePage_ = freePage;
}

int AMGraphicsViewWizard::coordinateListCount() const
{
	return coordinateList_->count();
}

int AMGraphicsViewWizard::relativeId() const
{
	return relativeId(currentId());
}

int AMGraphicsViewWizard::relativeId(int pageId) const
{
	if(pageId >= freePage())
	{
		if(pageId >= freePage() + numberOfPages())
		{
			return pageId - freePage() - numberOfPages();
		}
		else
		{
			return pageId - freePage();
		}
	}
	else
	{
		return -1;
	}
}

int AMGraphicsViewWizard::pageWait(int relativeId) const
{
	if(relativeId >= numberOfPages())
		AMErrorMon::alert(this, AMGRAPHICSVIEWWIZARD_PAGEWAIT_OUT_OF_BOUNDS_MAX, QString("Page %1 is greater than the maximum index of %2").arg(relativeId).arg(numberOfPages()) );
	else if(relativeId < 0)
		AMErrorMon::alert(this, AMGRAPHICSVIEWWIZARD_PAGEWAIT_OUT_OF_BOUNDS_MIN, QString("Page %1 is less than the minimum index of 0").arg(relativeId) );
	return freePage() + relativeId;
}

int AMGraphicsViewWizard::pageSet(int relativeId) const
{
	if(relativeId >= numberOfPages())
		AMErrorMon::alert(this, AMGRAPHICSVIEWWIZARD_PAGESET_OUT_OF_BOUNDS_MAX, QString("Page %1 is greater than the maximum index of %2").arg(relativeId).arg(numberOfPages()) );
	else if(relativeId < 0)
		AMErrorMon::alert(this, AMGRAPHICSVIEWWIZARD_PAGESET_OUT_OF_BOUNDS_MAX, QString("Page %1 is less than the minimum index of 0").arg(relativeId) );
	return freePage() + numberOfPages() + relativeId;
}

int AMGraphicsViewWizard::freePage() const
{
	return freePage_;
}

bool AMGraphicsViewWizard::isWaitPage(int pageNumber) const
{
	return (pageNumber >= freePage()) && (pageNumber < (freePage() + numberOfPages()));
}

bool AMGraphicsViewWizard::isSetPage(int pageNumber) const
{
	return (pageNumber >= (freePage() + numberOfPages())) && (pageNumber < (freePage() + 2*numberOfPages()));
}

bool AMGraphicsViewWizard::rotationEnabled() const
{
	return rotationEnabled_;
}

const QList<QVector3D *> *AMGraphicsViewWizard::getCoordinateList() const
{
	return coordinateList();
}

const QList<QPointF *> *AMGraphicsViewWizard::getPointList() const
{
	return pointList();
}

const QList<double> *AMGraphicsViewWizard::getRotationList() const
{
	return rotations();
}

void AMGraphicsViewWizard::setMotorMovementEnabled(bool motorMovementEnabled)
{
	motorMovementEnabled_ = motorMovementEnabled;
}

AMWizardPage::AMWizardPage(QWidget *parent)
	:QWizardPage(parent)
{
	setTitle("AMWizardPage Title");
	QVBoxLayout* selectLayout = new QVBoxLayout();
	selectLayout->setContentsMargins(0,0,0,0);
	selectLayout->addWidget(topLabel_ = new QLabel("AMWizardPage Label"));
	selectLayout->addStretch();
	setLayout(selectLayout);
	topLabel_->setWordWrap(true);
}

AMGraphicsViewWizard *AMWizardPage::viewWizard() const
{
	return (AMGraphicsViewWizard*)wizard();
}

void AMWizardPage::initializePage()
{
	setTitle(message(Title));
	setLabelText(message(Text));
}

QString AMWizardPage::message(int type)
{
	int messageType;
	switch(type)
	{
	case Title:
		messageType = AMGraphicsViewWizard::Title;
		break;
	case Text:
		messageType = AMGraphicsViewWizard::Text;
		break;
	case Other:
		messageType = AMGraphicsViewWizard::Other;
		break;
	case Help:
		messageType = AMGraphicsViewWizard::Help;
		break;
	default:
		messageType = AMGraphicsViewWizard::Default;
		break;
	}
	return viewWizard()->message(messageType);
}

void AMWizardPage::setLabelText(QString text)
{
	topLabel_->setText(text);
}


AMWaitPage::AMWaitPage(QWidget *parent)
	:AMWizardPage(parent)
{

	waitingToMove_ = false;
	waitTimer_ = new QTimer();
	connect(waitTimer_, SIGNAL(timeout()), this, SLOT(nextPage()));

}

void AMWaitPage::initializePage()
{
	AMWizardPage::initializePage();
	waitingToMove_ = true;
	connect(viewWizard(), SIGNAL(moveSucceeded()), this, SLOT(nextPage()));

}

bool AMWaitPage::isComplete() const
{
	return false;
}

void AMWaitPage::stopTimer()
{

	if(waitingToMove_)
	{
		bool state = checkState();
		if(!state)
			waitTimer_->stop();
		else
		{
			waitTimer_->stop();
		}
	}
	waitingToMove_ = false;
}

void AMWaitPage::startTimer(int msec)
{
	/// request the motor movement state
	viewWizard()->checkMotorMovementState();
	/// get the result of the request
	bool state = viewWizard()->motorMovementEnabled();
	/// if not moving, start the time, else move the motor
	if(!state)
		waitTimer_->start(msec);
	else
		viewWizard()->waitPage();
}

void AMWaitPage::nextPage()
{

	if(waitingToMove_)
	{
		bool state = checkState();
		if(!state)
			waitTimer_->stop();
		else
		{
			waitTimer_->stop();
		}
		wizard()->next();
	}
	waitingToMove_ = false;

}

bool AMWaitPage::checkState()
{
	viewWizard()->checkMotorMovementState();
	return viewWizard()->motorMovementEnabled();
}



AMViewPage::AMViewPage(QWidget *parent)
	: AMWizardPage(parent)
{
	viewFrame_ = new QFrame();
	view_ = new AMSampleCameraGraphicsView();

}

AMSampleCameraGraphicsView *AMViewPage::view()
{
	return view_;
}

void AMViewPage::setView(AMSampleCameraGraphicsView *view)
{
	view_ = view;
}


void AMViewPage::initializePage()
{
	AMWizardPage::initializePage();
	if(true)
	{
		setView(((AMGraphicsViewWizard*)wizard())->view());
	}
	addView();
	viewWizard()->fixText();
	connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), viewWizard(), SLOT(fixText()));
	connect(view_->horizontalScrollBar(), SIGNAL(valueChanged(int)), viewWizard(), SLOT(fixText()));
}

void AMViewPage::cleanupPage()
{
	layout()->removeWidget(viewFrame_);
}


void AMViewPage::addView()
{
	viewFrame_ = new QFrame();
	QHBoxLayout* viewLayout = new QHBoxLayout();
	viewLayout->addStretch();
	viewLayout->addWidget(view_);
	viewLayout->addStretch();
	viewFrame_->setLayout(viewLayout);
	layout()->addWidget(viewFrame_);
}


AMCheckPage::AMCheckPage(QWidget *parent)
	: AMViewPage(parent)
{
	isConfigured_ = new QCheckBox();
	isConfigured_->setChecked(true);
	layout()->addWidget(isConfigured_);
	connect(isConfigured_, SIGNAL(toggled(bool)), this, SLOT(checkBoxChanged(bool)));
}

void AMCheckPage::initializePage()
{
	isConfigured_->setText(message(Other));
	AMViewPage::initializePage();
	QString fieldName = QString("configured%1").arg(viewWizard()->currentId());
	if(field(fieldName).isNull())
	{
		registerField(fieldName, isConfigured_);
	}

}

void AMCheckPage::checkBoxChanged(bool state)
{
	Q_UNUSED(state)
}


AMWizardOptionPage::AMWizardOptionPage(QWidget *parent)
	: AMWizardPage(parent)
{
}

void AMWizardOptionPage::initializePage()
{
	int points = viewWizard()->numberOfPoints();
	int numberOfDimensions = 3;
	/// if including rotation, add a fourth element to each coordinate
	if(viewWizard()->rotationEnabled())
	{
		numberOfDimensions = 4;
	}
	coordinateEdit_ = new QLineEdit* [numberOfDimensions*points];
	for(int i = 0; i < numberOfDimensions*points; i++)
	{
		coordinateEdit_[i] = new QLineEdit();
	}
	QFrame* rowFrame [points];
	QHBoxLayout* rowLayout [points];
	coordinateFrame_ = new QFrame();
	QVBoxLayout* pageLayout = new QVBoxLayout();
	for(int i = 0; i < points; i++)
	{
		rowFrame[i] = new QFrame();
		rowLayout[i] = new QHBoxLayout();
		rowLayout[i]->setContentsMargins(12,4,12,4);
		for(int j = 0; j < numberOfDimensions; j++)
		{
			rowLayout[i]->addWidget(coordinateEdit_[numberOfDimensions*i + j]);
		}

		rowLayout[i]->addStretch();
		rowFrame[i]->setLayout(rowLayout[i]);
		pageLayout->addWidget(rowFrame[i]);
	}
	pageLayout->addStretch();
	coordinateFrame_->setLayout(pageLayout);

	layout()->addWidget(coordinateFrame_);

	AMWizardPage::initializePage();

	const int X = 0;
	const int Y = 1;
	const int Z = 2;
	const int ROTATION = 3;

	for(int i = 0; i < viewWizard()->numberOfPoints(); i++)
	{
		coordinateEdit_[numberOfDimensions*i+X]->setText(QString("%1").arg(viewWizard()->getCoordinateList()->at(i)->x()));
		connect(coordinateEdit_[numberOfDimensions*i+X], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		coordinateEdit_[numberOfDimensions*i+Y]->setText(QString("%1").arg(viewWizard()->getCoordinateList()->at(i)->y()));
		connect(coordinateEdit_[numberOfDimensions*i+Y], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		coordinateEdit_[numberOfDimensions*i+Z]->setText(QString("%1").arg(viewWizard()->getCoordinateList()->at(i)->z()));
		connect(coordinateEdit_[numberOfDimensions*i+Z], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		if(viewWizard()->rotationEnabled())
		{
			coordinateEdit_[numberOfDimensions*i+ROTATION]->setText(QString("%1").arg(viewWizard()->getRotationList()->at(i)));
			connect(coordinateEdit_[numberOfDimensions*i+ROTATION], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		}
	}


}

void AMWizardOptionPage::cleanupPage()
{
	layout()->removeWidget(coordinateFrame_);
	coordinateFrame_->deleteLater();
}

bool AMWizardOptionPage::isComplete() const
{
	return false;
}

void AMWizardOptionPage::textChanged()
{

	int numberOfDimensions = 3;
	/// if including rotation, add a fourth element to each coordinate
	if(viewWizard()->rotationEnabled())
	{
		numberOfDimensions = 4;
	}

	const int X = 0;
	const int Y = 1;
	const int Z = 2;
	const int ROTATION = 3;

	for(int i = 0; i < viewWizard()->numberOfPoints(); i++)
	{
		double x,y,z,rotation;
		x = coordinateEdit_[numberOfDimensions*i+X]->text().toDouble();
		y = coordinateEdit_[numberOfDimensions*i+Y]->text().toDouble();
		z = coordinateEdit_[numberOfDimensions*i+Z]->text().toDouble();
		if(viewWizard()->rotationEnabled())
		{
			rotation = coordinateEdit_[numberOfDimensions*i+ROTATION]->text().toDouble();
			viewWizard()->setRotation(rotation,i);
		}
		QVector3D newVector(x,y,z);
		viewWizard()->setCoordinate(newVector, i);

	}


}
