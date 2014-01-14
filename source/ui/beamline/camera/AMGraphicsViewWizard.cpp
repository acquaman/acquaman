#include "AMGraphicsViewWizard.h"

#include <QPointF>
#include "beamline/camera/AMGraphicsVideoSceneCopier.h"
#include "AMSampleCameraGraphicsView.h"
#include <QLayout>

#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <QVector3D>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsVideoItem>
#include <QScrollBar>
#include <QAbstractButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QMediaPlayer>

AMGraphicsViewWizard::AMGraphicsViewWizard(QWidget* parent)
    :QWizard(parent)
{
    view_ = new AMSampleCameraGraphicsView();
    scale_ = new QPointF(1,1);
    pointList_ = new QList<QPointF*>();
    coordinateList_ = new QList<QVector3D*>();
	rotations_ = new QList<double>();
    showOptionPage_ = false;
    optionsPage_ = -1;
    motorMovementEnabled_ = false;
	rotationEnabled_ = false;
    mediaPlayer_ = new QMediaPlayer();


    connect(QWizard::button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));
}

AMGraphicsViewWizard::~AMGraphicsViewWizard()
{
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
        QVideoWidget* nullPlayer = 0;
        player->stop();
        player->setVideoOutput(nullPlayer);
        view_->scene()->removeItem(videoItem);
        delete player;
        delete videoItem;
    }
}

AMSampleCameraGraphicsView *AMGraphicsViewWizard::view() const
{
    return view_;
}

QPointF AMGraphicsViewWizard::scale() const
{
    return *scale_;
}

void AMGraphicsViewWizard::setScale(QPointF scale)
{
    view_->scale(1/scale_->x(), 1/scale_->y());
    *scale_ = scale;
    view_->scale(scale_->x(),scale_->y());
}

void AMGraphicsViewWizard::setScale(double scaleFactor)
{
    QPointF scalePoint(scaleFactor,scaleFactor);
    setScale(scalePoint);
}

void AMGraphicsViewWizard::waitPage()
{
    emit moveTo(*coordinateList()->at(currentId()));
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

int AMGraphicsViewWizard::numberOfPoints() const
{
    return numberOfPoints_;
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
    //              return "Title Text";
    //          case Help:
    //              return "Help message text";
    //  etc.
    if(type)
    {
        return "Default message";
    }
    else
    {
        return "Default Error message";
    }
}

QPointF AMGraphicsViewWizard::mapPointToVideo(QPointF point) const
{
    QList<QGraphicsItem*> list = view()->items();
    QGraphicsVideoItem* videoItem;
    foreach(QGraphicsItem* item, list)
    {
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

bool AMGraphicsViewWizard::motorMovementEnabled()
{
    bool returnValue = motorMovementEnabled_;
    motorMovementEnabled_ = false;
    return returnValue;
}

void AMGraphicsViewWizard::checkMotorMovementState()
{
    qDebug()<<"AMGraphicsViewWizard::checkMotorMovementState - emitting request for enabled state";
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

    /// This item prevents the screen from flashing or going blank
    /// when there is a QGraphicsItem or a descendent in less than
    /// all the views, but at least one.
    /// It seems that placing a QGraphicsTextItem with it's bounding rectangle
    /// touching the current views boundaries (adjusted for scrolling) will
    /// stop the behaviour.
    fixItem_ = new QGraphicsTextItem("Fix");
    fixItem_->setObjectName("Fix");
    fixItem_->setZValue(INT_MAX);
    fixItem_->setPos(-1*fixItem_->boundingRect().width(), -1*fixItem_->boundingRect().height());
    view_->scene()->addItem(fixItem_);


    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    fixText();

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

//    connect(mediaPlayer_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaPlayerStateChanged(QMediaPlayer::MediaStatus)));
    connect(mediaPlayer_, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(mediaPlayerErrorChanged(QMediaPlayer::Error)));

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

void AMGraphicsViewWizard::updateScene(AMSampleCameraGraphicsView *view)
{
    view_->scene()->removeItem(fixItem_);

    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->updateChange(view_->scene(),view->scene());

    view_->scene()->addItem(fixItem_);

}

void AMGraphicsViewWizard::updateShape(QGraphicsPolygonItem *item)
{
    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->updateShape(item,view_->scene());
}


void AMGraphicsViewWizard::fixText()
{
//    QList<QGraphicsItem*> list = this->view()->items();
//    QGraphicsTextItem* fixTextItem;
//    foreach(QGraphicsItem* item, list)
//    {

//        if(item->type() == QGraphicsTextItem::Type)
//        {
//            QGraphicsTextItem* textItem = (QGraphicsTextItem*)(item);
//            if(textItem->objectName() == "Fix")
//            {
//                fixTextItem = textItem;
//            }
//        }
//    }


    QRect visibleRect = view_->viewport()->rect();

    visibleRect.moveTopLeft((QPoint(view_->horizontalScrollBar()->value(),
    view_->verticalScrollBar()->value())));

    QPointF fixLocation = visibleRect.topLeft() - (fixItem_->boundingRect().bottomRight());
    fixItem_->setPos(fixLocation);

}

void AMGraphicsViewWizard::testMoveSlot()
{
    qDebug()<<"Move signal received by GraphicsViewWizard";
    emit moveSucceeded();
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
    qDebug()<<"Media player error is"<<mediaPlayer_->error();

    if(id == optionsPage_)
    {
        button(CustomButton1)->show();
    }
    else if(button(CustomButton1)->isVisible())
    {
        button(CustomButton1)->hide();
    }
}

void AMGraphicsViewWizard::mediaPlayerStateChanged(QMediaPlayer::MediaStatus state)
{
    qDebug()<<"AMGraphicsViewWizard::mediaPlayerStateChanged"<<state;
    qDebug()<<"Media player error is"<<mediaPlayer_->error();
    if(state == QMediaPlayer::InvalidMedia)
    {
        qDebug()<<"State is InvalidMedia";
        QUrl mediaUrl = mediaPlayer_->media().canonicalUrl();
        mediaPlayer_->stop();
        mediaPlayer_->setMedia(mediaUrl);
        mediaPlayer_->play();
    }
}

void AMGraphicsViewWizard::mediaPlayerErrorChanged(QMediaPlayer::Error state)
{
    qDebug()<<"AMGraphicsViewWizard::mediaPlayerErrorChanged"<<state;
    if(state == QMediaPlayer::NetworkError)
    {
        qDebug()<<"Error is NetworkError";
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
        if(success)
        {
            qDebug()<<"Deleting and adding new video";
            delete mediaPlayer_;
            mediaPlayer_ = new QMediaPlayer();
            mediaPlayer_->setMedia(mediaUrl);
            mediaPlayer_->setVideoOutput(videoItem);
            mediaPlayer_->play();
        }


//        mediaPlayer_->setMedia(mediaUrl);
//        mediaPlayer_->play();
    }

}

void AMGraphicsViewWizard::setRotationEnabled(bool rotationEnabled)
{
	rotationEnabled_ = rotationEnabled;
}

bool AMGraphicsViewWizard::rotationEnabled() const
{
	return rotationEnabled_;
}

void AMGraphicsViewWizard::setMotorMovementEnabled(bool motorMovementEnabled)
{
    qDebug()<<"AMGraphicsViewWizard::setMotorMovementEnabled - setting state";
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
    viewWizard()->checkMotorMovementState();
    bool state = viewWizard()->motorMovementEnabled();
    if(!state)
        waitTimer_->start(msec);
    else
    {
        viewWizard()->waitPage();
    }

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
    qDebug()<<"Check box changed to"<<state;
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
//    int numberOfRows = points/2 + points%2;
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
		coordinateEdit_[numberOfDimensions*i+X]->setText(QString("%1").arg(viewWizard()->coordinateList()->at(i)->x()));
		connect(coordinateEdit_[numberOfDimensions*i+X], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		coordinateEdit_[numberOfDimensions*i+Y]->setText(QString("%1").arg(viewWizard()->coordinateList()->at(i)->y()));
		connect(coordinateEdit_[numberOfDimensions*i+Y], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		coordinateEdit_[numberOfDimensions*i+Z]->setText(QString("%1").arg(viewWizard()->coordinateList()->at(i)->z()));
		connect(coordinateEdit_[numberOfDimensions*i+Z], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		if(viewWizard()->rotationEnabled())
		{
			coordinateEdit_[numberOfDimensions*i+ROTATION]->setText(QString("%1").arg(viewWizard()->rotations()->at(i)));
			connect(coordinateEdit_[numberOfDimensions*i+ROTATION], SIGNAL(textEdited(QString)), this, SLOT(textChanged()));
		}
    }


}

void AMWizardOptionPage::cleanupPage()
{
    layout()->removeWidget(coordinateFrame_);
    delete coordinateFrame_;
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



