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
	setNumberOfPages(6);
	setNumberOfPoints(numberOfPages());
	setFreePage(Page_Free);

    setPage(Page_Intro, new IntroPage);
    setPage(Page_Check, new CheckPage);
    setPage(Page_Final, new AMWizardPage);
    setPage(Page_Option, new AMWizardOptionPage);
	for(int i = 0; i < numberOfPages(); i++)
	{
		setPage(pageSet(i), new SelectPage);
		setPage(pageWait(i), new WaitPage);
	}
    setStartId(Page_Intro);
	setHasHelpButton(true);
	setDefaultWindowTitle();
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

	for(int i = 0; i < numberOfPoints(); i++)
    {
		pointListAppend(new QPointF(0,0));
    }
    /// set the coordinates
	coordinateListAppend(new  QVector3D(0,6,0));
	coordinateListAppend(new  QVector3D(-5,6,0));
	coordinateListAppend(new  QVector3D(0,6,-20));
	coordinateListAppend(new  QVector3D(10,2,-8));
	coordinateListAppend(new  QVector3D(-10,2,-8));
	coordinateListAppend(new  QVector3D(0,2,8.5));

    //qDebug()<<"AMCameraConfigurationWizard::AMCameraConfigurationWizard - finished constructor";
}

AMCameraConfigurationWizard::~AMCameraConfigurationWizard()
{
}

int AMCameraConfigurationWizard::nextId() const
{
	int pageId = currentId();
	switch(pageId)
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
				return pageWait(0);
            }
	case Page_Final:
	default:
		int id = relativeId();
		if(isSetPage(pageId))
		{
			if(id == numberOfPages() - 1)
			{
				return Page_Final;
			}
			else
			{
				return pageWait(id+1);
			}
		}
		else if(isWaitPage(pageId))
		{
			return pageSet(id);
		}
		else
		{
			return FINISHED;
		}
    }
}

void AMCameraConfigurationWizard::addPoint(QPointF position)
{
    qDebug()<<"AMCameraConfigurationWizard::addPoint - adding point from page"<<currentId();
    QPointF* newPoint;
	int index = relativeId();
	if (!isSetPage(currentId()))
	{
		return;
	}

    if(checked(Page_Check))
    {
		clearPoints();
        return;
    }
	else if(index < 0)
	{
		return;
	}


	newPoint = pointList()->at(index);
    QPointF newPosition = mapPointToVideo(position);
    *newPoint = newPosition;

	foreach(QPointF* point, *pointList())
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

void AMCameraConfigurationWizard::clearPoints()
{
	QPointF *newPoint;
	for(int i = 0; i < pointList()->count(); i++)
	{
		qDebug()<<"Deleting point"<<i;
		newPoint = pointList()->at(i);
		*newPoint = QPointF(0,0);
	}
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
	case Page_Option:
		switch(messageType)
		{
		case Title:
			return QString(tr("Motor movement coordinates"));
		case Text:
			return QString(tr("Set the coordinates to move to for this configuration."));
		case Help:
			return QString(tr("Changing the values on this page will set the motor coordinates to be moved to for the camera configuration."));
		case Other:
		case Default:
		default:
			break;
		}
		break;
	default:
		if(isSetPage(currentId()))
		{
			switch(messageType)
			{
			case Title:
				return QString(tr("Selection Page %1")).arg(relativeId()+1);
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
		}
		else if(isWaitPage(currentId()))
		{
			switch(messageType)
			{
			case Title:
				return QString(tr("Moving to position %1")).arg(relativeId()+1);
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
		}
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

//void AMCameraConfigurationWizard::waitPage()
//{
//    qDebug()<<"AMCameraConfigurationWizard::waitPage";
//	emit moveTo(*coordinateList()->at(relativeId()));
//}

void AMCameraConfigurationWizard::back()
{
    // this makes sure that the motors move to the
    // correct position upon pressing back.
	int pageId = currentId();
	int id = relativeId();
	if(isWaitPage(pageId))
	{
		((WaitPage*)page(pageId))->stopTimer();
		QWizard::back();
		if(id == 0)
		{
			cleanupPage(Page_Check);
			initializePage(Page_Check);
		}
	}
	else if(isSetPage(pageId))
	{
		int prevPage = pageWait(id-1);
		if(id == 0)
		{
			prevPage = Page_Check;
		}
		while(currentId() != prevPage) QWizard::back();
		if(currentId() == prevPage)
		{

			if(id == 0)
			{
				// page check needs a cleanup or it will attempt to
				// display everything twice
				cleanupPage(prevPage);
			}
			initializePage(prevPage);
		}
		else
		{
			qDebug()<<"AMCameraConfigurationWizard::back - could not reach correct page";
		}
	}
	else
	{
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







