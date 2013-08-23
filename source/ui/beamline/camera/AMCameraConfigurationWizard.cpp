#include "AMCameraConfigurationWizard.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QVariant>
#include <QTimer>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include "AMSampleCameraGraphicsView.h"
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QTextDocument>
#include <QScrollBar>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QVector3D>
#include "beamline/camera/AMGraphicsVideoSceneCopier.h"
#include <QTimerEvent>
#include "AMGraphicsViewWizard.h"


AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    numberOfPoints_ = 6;
    showOptionPage_ = false;

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
    setPage(Page_Option, new AMWizardOptionPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
//    setPixmap(QWizard::LogoPixmap, QPixMap());
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

    addOptionPage(Page_Intro);

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
    coordinateList_->append(new  QVector3D(0,6,0));
    coordinateList_->append(new  QVector3D(-5,6,0));
    coordinateList_->append(new  QVector3D(0,6,-20));
    coordinateList_->append(new  QVector3D(10,2,-8));
    coordinateList_->append(new  QVector3D(-10,2,-8));
    coordinateList_->append(new  QVector3D(0,2,8.5));

    //qDebug()<<"AMCameraConfigurationWizard::AMCameraConfigurationWizard - finished constructor";
}

AMCameraConfigurationWizard::~AMCameraConfigurationWizard()
{
}

int AMCameraConfigurationWizard::nextId() const
{
    qDebug()<<"AMCameraConfigurationWizard::nextId";

    switch(currentId())
    {
        case Page_Intro:
        if(showOptionPage())
            return Page_Option;
        else return Page_Check;
    case Page_Option:
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
            return QString(tr("Introduction Page"));
        case Text:
            return QString(tr("On the first page, check to see if the camera appears to line up correctly with the video image.")
                              + tr("  If it does, check that it is correct and continue.  If it is not lined up correctly")
                              + tr(" you will have to run through the calibration.  To perform the calibration simply select")
                              + tr(" the same point on the sample manipulator as it moves around."));
        case Help:
            return QString(tr("This is the camera configuration wizard, which is designed to ensure that the camera is properly")
                           + tr(" calibrated to track the sample plate as it moves around the chamber.  To start the calibration")
                           + tr(" select next on this page.  The next page will give you the option to skip the calibration, ")
                           + tr("if it is already correct."));
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
            return QString(tr("Camera Calibration"));
        case Text:
             return QString(tr("Check to see if the camera is correctly lined up."));
        case Help:
             return QString(tr("If the camera appears to be correctly lined up, simply make sure that the checkbox at the top of the")
                            + tr(" page is checked.  If the camera is not lined up, or if you are unsure if it is, make sure that the")
                            + tr(" checkbox at the top of the page is not checked.  Then select next to continue.  \n For a more detailed")
                            + tr(" explanation of this wizard, select the button marked Intro."));
        case Other:
            return QString(tr("Is the camera correct?"));
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
            return QString(tr("Selection Page %1")).arg(relativeId());
        case Text:
            return QString(tr("Select the point corresponding to the coordinate: %1, %2, %3")).arg(coordinateX(relativeId())).arg(coordinateY(relativeId())).arg(coordinateZ(relativeId()));
        case Help:
            return QString(tr("For each selection you will need to click on the same point on the sample plate.  Upon clicking, the")
                           + tr(" sample manipulator will automatically move on to the next point.  If the manipulator has not moved")
                           + tr(" movement may have been disabled.  Check to see if the motor movement enabled box is checked in the settings."));
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
            return QString(tr("Moving to position %1")).arg(relativeId());
        case Text:
             return QString(tr("Please wait until the manipulator has finished moving."));
        case Help:
            return QString(tr("The sample manipulator is attempting to move to the next point for calibration.  If")
                           + tr(" the video does not reappear within a few moments there may be a problem communicating")
                           + tr(" with the motors.  Please ensure that motor movement is enabled.  You may re-attempt")
                           + tr(" this movement by selecting back, followed by next."));
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
            return QString(tr("Calibration Complete"));
        case Text:
            return QString(tr("The calibration has been completed.  Select 'Finish' to exit the calibration."));
        case Help:
            return QString(tr("The calibration has now been completed.  If you would like to reselect any point, simply navigate using 'back'")
                           + tr(" until you reach the point you wish to reselect.  Any position where you do not click on the screen will")
                           + tr(" keep the point that you had previously selected."));
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
        return QString(tr("Default Title"));
    case Text:
         return QString(tr("If this text is appearing there is an error.  The wizard is unable to identify the page that it")
                        + tr(" is currently on."));
    case Help:
        return QString(tr("This help message is indicative of an error.  For some reason the current page cannot be identified."));
    case Other:
    case Default:
    default:
        return QString(tr("Error - unknown page and unknown message type."));
    }
}

int AMCameraConfigurationWizard::relativeId()
{
    // this function relates each page number with an
    // appropriate integer, used for naming and indexing.
    // This prevents the need of a unique page type for
    // each page while simpifying logic for indexing
    // coordinates.
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

void AMCameraConfigurationWizard::waitPage()
{
    qDebug()<<"AMCameraConfigurationWizard::waitPage";
    emit moveTo(*coordinateList()->at(relativeId()-1));
}




void AMCameraConfigurationWizard::back()
{
    // this makes sure that the motors move to the
    // correct position upon pressing back.
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
            while(currentId() != Page_Check) QWizard::back();
            if(currentId() == Page_Check)
            {
                // page check needs a cleanup or it will attempt to
                // display everything twice
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
    AMWaitPage::initializePage();
    AMWaitPage::startTimer(1000);

}







