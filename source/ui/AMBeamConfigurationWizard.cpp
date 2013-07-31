#include "AMBeamConfigurationWizard.h"
#include <QWizard>
#include <QList>
#include "AMGraphicsViewWizard.h"
#include <QLabel>
#include <QAbstractButton>
#include <QLayout>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>
#include "AMShapeDataSetGraphicsView.h"
#include <QVector3D>



AMBeamConfigurationWizard::AMBeamConfigurationWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    setPage(Page_Intro, new AMWizardPage);
    setPage(Page_Check_One, new AMBeamCheckPage);
    setPage(Page_Check_Two, new AMBeamCheckPage);
    setPage(Page_Check_Three, new AMBeamCheckPage);
    setPage(Page_Wait_One, new AMBeamWaitPage);
    setPage(Page_Wait_Two, new AMBeamWaitPage);
    setPage(Page_Wait_Three, new AMBeamWaitPage);
    setPage(Page_Set_One, new AMBeamSelectPage);
    setPage(Page_Set_Two, new AMBeamSelectPage);
    setPage(Page_Set_Three, new AMBeamSelectPage);
    setPage(Page_Final, new AMWizardPage);
    setPage(Page_Option, new AMWizardOptionPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    disconnect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));
    connect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));

    setMinimumSize(600,600);

    setting_ = false;
    reviewBeamShape_ = true;

    /// two points for each square, three squares.
    numberOfPoints_ = 6;


    for(int i = 0; i < numberOfPoints_; i++)
    {
        pointList_->append(new QPointF(0,0));
    }

    topLeft_ = true;

    coordinateList_->clear();
    coordinateList_->append(new QVector3D(0,0,0));
    coordinateList_->append(new QVector3D(0,0,-0.5));
    coordinateList_->append(new QVector3D(0,0,0.5));
    coordinateList_->append(new QVector3D(0,0,0));
    coordinateList_->append(new QVector3D(0,0,-0.5));
    coordinateList_->append(new QVector3D(0,0,0.5));


    addOptionPage(Page_Intro);

}

AMBeamConfigurationWizard::~AMBeamConfigurationWizard()
{
}

int AMBeamConfigurationWizard::nextId() const
{
    switch(currentId())
    {
    case Page_Intro:
        if(showOptionPage())
            return Page_Option;
        else
            return Page_Wait_One;
    case Page_Check_One:
        return Page_Wait_Two;
    case Page_Check_Two:
        return Page_Wait_Three;
    case Page_Check_Three:
        if(checked(Page_Check_One) && checked(Page_Check_Two) && checked(Page_Check_Three))
            return Page_Final;
        else
            return Wait_One_Again;
    case Page_Wait_One:
        if(setting_) return Page_Set_One;
        else return Page_Check_One;
    case Page_Wait_Two:
        if(setting_) return Page_Set_Two;
        else return Page_Check_Two;
    case Page_Wait_Three:
        if(setting_) return Page_Set_Three;
        else return Page_Check_Three;
    case Page_Set_One:
        return Wait_Two_Again;
    case Page_Set_Two:
        return Wait_Three_Again;
    case Page_Set_Three:
        return Page_Final;
    default:
        return -1;
    }
}

void AMBeamConfigurationWizard::next()
{
    if(nextId() == Wait_One_Again)
    {
        setting_ = true;
        reviewBeamShape_ = false;
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
    }
    else if (nextId() == Wait_Two_Again)
    {
        setting_ = false;
        QWizard::back();
        while(currentId() != Page_Wait_Two)
            QWizard::next();
        if(currentId() == Page_Wait_Two)
        {
            setting_ = true;
            initializePage(Page_Wait_Two);
        }
    }
    else if(nextId() == Wait_Three_Again)
    {
        setting_ = false;
         QWizard::back();
        while(currentId() != Page_Wait_Three)
            QWizard::next();
        if(currentId() == Page_Wait_Three)
        {
            setting_ = true;
            initializePage(Page_Wait_Three);
        }
    }
    else if((currentId() == Page_Check_One && !(field(QString("configured%1").arg(Page_Check_One)).toBool()))
            || (currentId() == Page_Check_Two && !(field(QString("configured%1").arg(Page_Check_Two)).toBool())))
    {
        while(currentId() != Page_Check_Three)
        {
            QWizard::next();
        }
        next();
    }
    else AMGraphicsViewWizard::next();
}

QString AMBeamConfigurationWizard::message(int type)
{
    if(type == Wizard_Title) return "Beam Configuration Wizard";
    else if(type == Help_Title) return "Help";

    switch(currentId())
    {
    case Page_Intro:
        switch(type)
        {
        case Title:
            return "Beam Configuration Wizard";
        case Text:
            return QString(tr("This wizard will help you to configure the beam settings. \n") +
                           ("The sample plate will be moved to three different locations. ") +
                    ("If, at any of the three locations, ") +
                    "the rectangle on screen does not match up well to the location of the beamspot uncheck the checkbox on that " +
                    "page.  You will then be asked to draw a rectangle around the beamspot in each of the three positions.  For further " +
                    "explanation, click the \"Help\" button for detailed information for that page.");
        case Help:
            return "Intro page help";
        case Other:
        case Default:
        default:
            break;
        }
        break;

    case Page_Check_One:
    case Page_Check_Two:
    case Page_Check_Three:
        switch(type)
        {
        case Title:
            return "Confirm Beam";
        case Text:
            return QString("Check to see if beam %1 is in the correct configuration.").arg(relativeId());
        case Help:
            return QString("Check to see if the rectangle appears around the beamspot.  \n") +
                           "If the beamspot is not visible, ensure that visible light has been turned on.  \n" +
                           "If the beamspot is still not visible, the beam may be moved drastically away from its usual spot.  " +
                           "This may require changing of the coordinates on the options page, accessible from the introduction page.  ";
        case Other:
            return "Is the beam correct?";
        case Default:
        default:
            break;
        }
        break;
    case Page_Set_One:
    case Page_Set_Two:
     case Page_Set_Three:
        switch(type)
        {
        case Title:
            return "Set Beam";
        case Text:
            return QString("Draw a box over beam position %1").arg(relativeId());
        case Help:
            return QString("Draw a box over the visible beamspot.  To draw a box click and drag to create the corners.  ")
                    + "Try to fit the box to best encompass the whole beamspot.";
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Wait_One:
    case Page_Wait_Two:
    case Page_Wait_Three:
        switch(type)
        {
        case Title:
            return "Please Wait";
        case Text:
            return QString("Moving to position %1").arg(relativeId());
        case Help:
            return "Wait page help";
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Final:
        switch(type)
        {
        case Title:
            return "Final page Title";
        case Text:
            return (QString("Check Page %1 has value: %2 \n").arg(Page_Check_One).arg(field(QString("configured%1").arg(Page_Check_One)).toBool())
                    + QString("Check Page %1 has value: %2 \n").arg(Page_Check_Two).arg(field(QString("configured%1").arg(Page_Check_Two)).toBool())
                                 + QString("Check Page %1 has value: %2").arg(Page_Check_Three).arg(field(QString("configured%1").arg(Page_Check_Three)).toBool()));
        case Help:
            return "Final page help";
        case Other:
        case Default:
        default:
            break;
        }
    default:
        return "";
    }

    return "";
}



void AMBeamConfigurationWizard::addPoint(QPointF position)
{

    int index = relativeId() - 1;
    QPointF* newPoint;

    if(topLeft_)
    {
        newPoint = (*pointList_)[2*(index)];
        connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
        topLeft_ = !topLeft_;
    }
    else
    {
        newPoint = (*pointList_)[2*(index) + 1];
    }

    *newPoint = mapPointToVideo(position);

    emit showShape(index);


}


void AMBeamConfigurationWizard::endPoint()
{
    disconnect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
    topLeft_ = true;
}


int AMBeamConfigurationWizard::relativeId()
{
    switch(currentId())
    {
    case Page_Check_One:
        case Page_Wait_One:
        case Page_Set_One:
        return 1;
    case Page_Check_Two:
    case Page_Wait_Two:
    case Page_Set_Two:
        return 2;
    case Page_Check_Three:
    case Page_Wait_Three:
    case Page_Set_Three:
        return 3;
    default:
        return 0;
    }
}

void AMBeamConfigurationWizard::waitPage()
{
    emit moveTo(relativeId() - 1);
}

void AMBeamConfigurationWizard::showBeamShape()
{
    emit showBeamMarker(relativeId()-1);
}

bool AMBeamConfigurationWizard::setting()
{
    return setting_;
}

bool AMBeamConfigurationWizard::reviewBeamShape()
{
    return reviewBeamShape_;
}



void AMBeamConfigurationWizard::back()
{
    switch(currentId())
    {
    case Page_Check_One:
        while(currentId() != Page_Intro)
            QWizard::back();
        break;
    case Page_Check_Two:
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Check_Three:
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Wait_One:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        if(setting_)
        {
            setting_ = false;
            while(currentId() != Page_Wait_Three)
                next();
            if(currentId() == Page_Wait_Three)
                initializePage(Page_Wait_Three);

        }
        else
        {
            QWizard::back();
        }
        break;
    case Page_Wait_Two:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Wait_Three:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Set_One:
        setting_ = false;
        reviewBeamShape_ = true;
        QWizard::back();
        while(currentId() != Page_Wait_Three)
            QWizard::next();
        if(currentId() == Page_Wait_Three)
            initializePage(Page_Wait_Three);
        break;
    case Page_Set_Two:
        setting_ = true;
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Set_Three:
        setting_ = true;
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Final:
        while(currentId() != Page_Wait_Three)
            QWizard::back();
        if(currentId() == Page_Wait_Three)
            initializePage(Page_Wait_Three);
        break;
    default:
        AMGraphicsViewWizard::back();
        break;
    }

}


void AMBeamCheckPage::initializePage()
{
    if(((AMBeamConfigurationWizard*)viewWizard())->reviewBeamShape())
    {
        ((AMBeamConfigurationWizard*)viewWizard())->showBeamShape();
    }
    AMCheckPage::initializePage();
}

void AMBeamWaitPage::initializePage()
{

   AMWaitPage::initializePage();

    AMWaitPage::startTimer(1000);

    viewWizard()->waitPage();
}

void AMBeamSelectPage::initializePage()
{
    AMViewPage::initializePage();

    disconnect(view(), SIGNAL(mousePressed(QPointF)), viewWizard(), SLOT(addPoint(QPointF)));
    disconnect(view(), SIGNAL(mouseLeftReleased(QPointF)), viewWizard(), SLOT(endPoint()));
    connect(view(), SIGNAL(mousePressed(QPointF)), viewWizard(), SLOT(addPoint(QPointF)));
    connect(view(), SIGNAL(mouseLeftReleased(QPointF)), viewWizard(), SLOT(endPoint()));


}


