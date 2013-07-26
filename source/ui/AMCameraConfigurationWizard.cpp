#include "AMCameraConfigurationWizard.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QVariant>
#include <QTimer>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include "AMShapeDataSetGraphicsView.h"
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QTextDocument>
#include <QScrollBar>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QVector3D>
#include "AMGraphicsVideoSceneCopier.h"
#include <QTimerEvent>
#include "AMGraphicsViewWizard.h"


AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    numberOfPoints_ = 6;
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Check, new CheckPage);
    setPage(Page_Final, new AMWizardPage);
    setPage(Page_Select_One, new SelectPage);
    setPage(Page_Select_Two, new SelectPage);
    setPage(Page_Select_Three, new SelectPage);
    setPage(Page_Select_Four, new SelectPage);
    setPage(Page_Select_Five, new SelectPage);
    setPage(Page_Select_Six, new SelectPage);
    setPage(Page_Wait_One, new WaitPage);
    setPage(Page_Wait_Two, new WaitPage);
    setPage(Page_Wait_Three, new WaitPage);
    setPage(Page_Wait_Four, new WaitPage);
    setPage(Page_Wait_Five, new WaitPage);
    setPage(Page_Wait_Six, new WaitPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
//    setPixmap(QWizard::LogoPixmap, QPixMap());
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));


    QSize maxSize(0,0);
    QList<int> pageNumbers = pageIds();
    foreach(int pageNumber, pageNumbers)
    {
        page(pageNumber)->adjustSize();
        if(page(pageNumber)->size().width() > maxSize.width() || page(pageNumber)->height() > maxSize.height())
            maxSize = page(pageNumber)->size();
    }

    setMinimumSize(600,600);

    pointList_->clear();
    for(int i = 0; i < numberOfPoints_; i++)
    {
        pointList_->append(new QPointF(0,0));
    }
    /// set the coordinates
    coordinateList_->clear();
    coordinateList_->append(new  QVector3D(0,0,0));
    coordinateList_->append(new  QVector3D(0,0,12));
    coordinateList_->append(new  QVector3D(5,0,5));
    coordinateList_->append(new  QVector3D(0,5,0));
    coordinateList_->append(new  QVector3D(12,0,0));
    coordinateList_->append(new  QVector3D(12,5,0));


}

AMCameraConfigurationWizard::~AMCameraConfigurationWizard()
{
}

int AMCameraConfigurationWizard::nextId() const
{
    switch(currentId())
    {
        case Page_Intro:
            return Page_Check;
        case Page_Check:
        if(checked(Page_Check))
            {
                return Page_Final;
            }
            else
            {

                return Page_Wait_One;
            }
        case Page_Select_One:
            return Page_Wait_Two;
        case Page_Select_Two:
            return Page_Wait_Three;
        case Page_Select_Three:
            return Page_Wait_Four;
        case Page_Select_Four:
            return Page_Wait_Five;
        case Page_Select_Five:
            return Page_Wait_Six;
        case Page_Select_Six:
            return Page_Final;
        case Page_Wait_One:
            return Page_Select_One;
        case Page_Wait_Two:
            return Page_Select_Two;
        case Page_Wait_Three:
            return Page_Select_Three;
        case Page_Wait_Four:
            return Page_Select_Four;
        case Page_Wait_Five:
            return Page_Select_Five;
        case Page_Wait_Six:
            return Page_Select_Six;
        case Page_Final:
        default:
            return -1;
    }
}

void AMCameraConfigurationWizard::addPoint(QPointF position)
{
    qDebug()<<"AMCameraConfigurationWizard::addPoint - adding point from page"<<currentId();
    QPointF* newPoint;
    int index;
    switch(currentId())
    {
    case Page_Select_One:
        index = 0;
        break;
    case Page_Select_Two:
        index = 1;
        break;
    case Page_Select_Three:
        index = 2;
        break;
    case Page_Select_Four:
        index = 3;
        break;
    case Page_Select_Five:
        index = 4;
        break;
    case Page_Select_Six:
        index = 5;
        break;
    default:
        index = -1;
        break;
    }

    if(checked(Page_Check))
    {
        for(int i = 0; i < pointList_->count(); i++)
        {
            qDebug()<<"Deleting point"<<i;
            newPoint = pointList_->at(i);
            *newPoint = QPointF(0,0);
        }
        return;
    }
    else if(index < 0)return;


    newPoint = pointList_->at(index);
    QPointF newPosition = mapPointToVideo(position);
    *newPoint = newPosition;

    foreach(QPointF* point, *pointList_)
    {
        qDebug()<<*point;
    }

    next();

}

double AMCameraConfigurationWizard::coordinateX(int id)
{
    return coordinateList()->at(id-1)->x();
}

double AMCameraConfigurationWizard::coordinateY(int id)
{
    return coordinateList()->at(id-1)->y();
}

double AMCameraConfigurationWizard::coordinateZ(int id)
{
    return coordinateList()->at(id-1)->z();
}


QString AMCameraConfigurationWizard::message(int messageType)
{
    if(messageType == Wizard_Title)
        return QString("Camera Wizard");
    else if(messageType == Help_Title)
        return QString("Camera Wizard Help");
    switch(currentId())
    {
    case Page_Intro:
        switch(messageType)
        {
        case Title:
            return QString("Introduction Page");
        case Text:
            return QString(tr("This is the introduction page of the camera configuration wizard."));
        case Help:
            return QString("Help message for the intro page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Check:
        switch(messageType)
        {
        case Title:
            return QString("Check page");
        case Text:
             return QString(tr("This is the page where you check to see if the camera is correctly lined up."));
        case Help:
             return QString("Help message for check page.");
        case Other:
            return QString("Is the camera correct?");
        case Default:
            return QString(tr("< &Intro"));
        default:
            break;
        }
        break;
    case Page_Select_One:
    case Page_Select_Two:
    case Page_Select_Three:
    case Page_Select_Four:
    case Page_Select_Five:
    case Page_Select_Six:
        switch(messageType)
        {
        case Title:
            return QString("Selection Page %1").arg(relativeId());
        case Text:
            return QString("Select the point corresponding to the coordinate: %1, %2, %3").arg(coordinateX(relativeId())).arg(coordinateY(relativeId())).arg(coordinateZ(relativeId()));
        case Help:
            return QString("Help message for selection page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Wait_One:
    case Page_Wait_Two:
    case Page_Wait_Three:
    case Page_Wait_Four:
    case Page_Wait_Five:
    case Page_Wait_Six:
        switch(messageType)
        {
        case Title:
            return QString("Moving to position %1").arg(relativeId());
        case Text:
             return QString(tr("Please wait until the next page appears."));
        case Help:
            return QString("Help message for wait page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Final:
        switch(messageType)
        {
        case Title:
            return QString("Final Page");
        case Text:
            return QString("This is the final page of the wizard.");
        case Help:
            return QString("Help message for final page.");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    }
    switch(messageType)
    {
    case Title:
        return "Default Title";
    case Text:
         return QString("Default message.");
    case Help:
        return QString("Default help message.");
    case Other:
    case Default:
    default:
        return "";
    }
}

int AMCameraConfigurationWizard::relativeId()
{
    switch(currentId())
    {
    case Page_Select_One:
    case Page_Wait_One:
        return 1;
    case Page_Select_Two:
     case Page_Wait_Two:
        return 2;
    case Page_Select_Three:
    case Page_Wait_Three:
        return 3;
    case Page_Select_Four:
    case Page_Wait_Four:
        return 4;
    case Page_Select_Five:
    case Page_Wait_Five:
        return 5;
    case Page_Select_Six:
    case Page_Wait_Six:
        return 6;
    default:
        return 0;
    }
}




void AMCameraConfigurationWizard::back()
{
    int id = currentId();
    switch(id)
    {
        case Page_Wait_One:
            ((WaitPage*)page(id))->stopTimer();
            QWizard::back();
            cleanupPage(Page_Check);
            initializePage(Page_Check);
            break;
        case Page_Wait_Two:
        case Page_Wait_Three:
        case Page_Wait_Four:
        case Page_Wait_Five:
        case Page_Wait_Six:
            ((WaitPage*)page(id))->stopTimer();
            QWizard::back();
            break;
        case Page_Select_One:
            while(currentId() != Page_Check)
            {
                QWizard::back();

            }
            if(currentId() == Page_Check)
            {
                cleanupPage(Page_Check);
                initializePage(Page_Check);
            }
            break;
        case Page_Select_Two:
            while(currentId() != Page_Wait_One) QWizard::back();
            if(currentId() == Page_Wait_One) initializePage(Page_Wait_One);
            break;
        case Page_Select_Three:
            while(currentId() != Page_Wait_Two) QWizard::back();
            if(currentId() == Page_Wait_Two) initializePage(Page_Wait_Two);
            break;
        case Page_Select_Four:
            while(currentId() != Page_Wait_Three) QWizard::back();
            if(currentId() == Page_Wait_Three) initializePage(Page_Wait_Three);
            break;
        case Page_Select_Five:
            while(currentId() != Page_Wait_Four) QWizard::back();
            if(currentId() == Page_Wait_Four) initializePage(Page_Wait_Four);
            break;
        case Page_Select_Six:
            while(currentId() != Page_Wait_Five) QWizard::back();
            if(currentId() == Page_Wait_Five) initializePage(Page_Wait_Five);
            break;
        default:
            QWizard::back();
    }
}

void IntroPage::initializePage()
{
    AMWizardPage::initializePage();
    startTimer(0);

}

void IntroPage::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    viewWizard()->next();
}


void CheckPage::initializePage()
{
    AMCheckPage::initializePage();
    setButtonText(QWizard::BackButton,message(Default));
}

void SelectPage::initializePage()
{
    AMViewPage::initializePage();
    disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
    connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}

void SelectPage::addPoint(QPointF position)
{
    ((AMCameraConfigurationWizard*)viewWizard())->addPoint(position);
}

void WaitPage::initializePage()
{
    AMWaitPage::startTimer(1000);
    AMWaitPage::initializePage();
}







