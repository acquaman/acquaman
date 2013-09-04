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
#include "AMSampleCameraGraphicsView.h"
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
    coordinateList_->append(new QVector3D(0,-1,0));
    coordinateList_->append(new QVector3D(0,1,0));
    coordinateList_->append(new QVector3D(0,0,0));
    coordinateList_->append(new QVector3D(0,-1,0));
    coordinateList_->append(new QVector3D(0,1,0));


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
    if(type == Wizard_Title) return QString(tr("Beam Configuration Wizard"));
    else if(type == Help_Title) return QString(tr("Help"));

    switch(currentId())
    {
    case Page_Intro:
        switch(type)
        {
        case Title:
            return QString(tr("Beam Configuration Wizard"));
        case Text:
            return QString(tr("This wizard will help you to configure the beam settings. \n") +
                           tr("The sample plate will be moved to three different locations. ") +
                    tr("If, at any of the three locations, ") +
                    tr("the rectangle on screen does not match up well to the location of the beamspot uncheck the checkbox on that ") +
                    tr("page.  You will then be asked to draw a rectangle around the beamspot in each of the three positions.  For further ") +
                    tr("explanation, click the \"Help\" button for detailed information for that page."));
        case Help:
            return QString(tr("To start the configuration of the beam, click next on this page.")
                              + tr("  If all the beamspots correspond to the visible beam, check the boxes")
                              + tr(" to indicate that the beam is correct.  If the beamspots do not line up, they")
                              + tr(" can be configured by marking the beam as being incorrect.  If there is no")
                              + tr(" visible beam, it may be that the visible beam has not been enabled.  If it has")
                              + tr(" and the beam is still not visible, ensure that the beam is on.  If the beam is on")
                              + tr(" and is visible, but is not appearing on the sample plate, it may need to be manipulated")
                              + tr(" until it is under the beam.  This may require a change in sample points.  These can")
                              + tr(" can be configured in 'options' (on this page).  Changing these values will change where")
                              + tr(" the sample plate moves to for the beam configuration."));
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
            return QString(tr("Confirm Beam"));
        case Text:
            return QString(tr("Check to see if beam %1 is in the correct configuration.")).arg(relativeId());
        case Help:
            return QString(tr("Check to see if the rectangle appears around the beamspot.  \n") +
                           tr("If the beamspot is not visible, ensure that visible light has been turned on.  \n") +
                           tr("If the beamspot is still not visible, the beam may be moved drastically away from its usual spot.  ") +
                           tr("This may require changing of the coordinates on the options page, accessible from the introduction page."));
        case Other:
            return QString(tr("Is the beam correct?"));
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
            return QString(tr("Set Beam"));
        case Text:
            return QString(tr("Draw a box over beam position %1")).arg(relativeId());
        case Help:
            return QString(tr("Draw a box over the visible beamspot.  To draw a box click and drag to create the corners.  "))
                    + tr("Try to fit the box to best encompass the entire beamspot.");
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
            return QString(tr("Please Wait"));
        case Text:
            return QString(tr("Moving to position %1")).arg(relativeId());
        case Help:
            return QString(tr("If the configuration is stuck here please check to make sure that motor movement is enabled.")
                              + tr(" If movement is enabled there may be a problem communicating with the sample manipulator motors."));
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
            return QString(tr("Configuration complete"));
        case Text:
            return QString(tr("Configuration has been completed.  The beam path has been recalculated."));
        case Help:
            return QString(tr("If the beam appears to be in the wrong place, this configuration may be rerun.  If the beam is known to be divergent")
                              + tr(" it may need to be selected from the main sample camera configuration settings."));
        case Other:
        case Default:
        default:
            break;
        }
    case Page_Option:
	    switch(type)
	    {
	    case Title:
		    return QString(tr("Movement Configuration"));
	    case Text:
		    return QString(tr("Set the points to move the motor to here.  These may need to be changed if something")
				   + tr(" has been moved inside the end station."));
	    case Help:
		    return QString(tr("Set the coordinates that will be moved to in the wizard here.  For the Beam Configuration, all points must have different y values"));
	    case Other:
	    case Default:
	    default:
		    break;
	    }

    default:
        return QString(tr("Error - unknown page."));
    }

    return QString(tr("Error - unknown message type."));
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
    emit moveTo(*coordinateList()->at(relativeId() - 1));
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

   qDebug()<<"Calling startTimer";
    AMWaitPage::startTimer(1000);

    qDebug()<<"Calling wait page";
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


