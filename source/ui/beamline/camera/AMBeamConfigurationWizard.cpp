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
#include <QTimer>
#include <QTimerEvent>


AMBeamConfigurationWizard::AMBeamConfigurationWizard(QWidget* parent)
	: AMGraphicsViewWizard(parent)
{
	/// two points for each square, three squares.
	setNumberOfPoints(6);
	setNumberOfPages(numberOfPoints());
	setFreePage(Page_Free);

	setRotationEnabled(false);

	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Final, new AMWizardPage);
	setPage(Page_Option, new AMWizardOptionPage);
	for(int i = 0; i < numberOfPages(); i++)
	{
		setPage(pageWait(i), new AMBeamWaitPage);
		/* kind of a dirty hack here, but this makes the
		   wait logic cleaner */
		if(i < 3)
			setPage(pageSet(i), new AMBeamCheckPage);
		else
			setPage(pageSet(i), new AMBeamSelectPage);
	}
	setStartId(Page_Intro);
	setOption(HaveHelpButton, true);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
	setWindowTitle(message(Wizard_Title));
	/* have to disconnect/connect the buttons to get them to work with the new slots */
	disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	disconnect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));
	connect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));

	setMinimumSize(600,600);

	setting_ = false;
	reviewBeamShape_ = true;




	for(int i = 0; i < numberOfPoints(); i++)
	{
		pointListAppend(new QPointF(0,0));
	}

	topLeft_ = true;

	coordinateListAppend(new QVector3D(0,0,0));
	coordinateListAppend(new QVector3D(0,-1,0));
	coordinateListAppend(new QVector3D(0,1,0));
	coordinateListAppend(new QVector3D(0,0,0));
	coordinateListAppend(new QVector3D(0,-1,0));
	coordinateListAppend(new QVector3D(0,1,0));


	addOptionPage(Page_Intro);

}

AMBeamConfigurationWizard::~AMBeamConfigurationWizard()
{
}

int AMBeamConfigurationWizard::nextId() const
{
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		if(showOptionPage())
			return Page_Option;
		else
			return pageWait(0);
	default:
		if(isWaitPage(pageId))
		{
			return pageSet(relativeId());
		}
		else if(isSetPage(pageId))
		{
			if((lastCheckPage() && allChecked()) || (relativeId() >= (numberOfPages() - 1)))
			{
					return Page_Final;
			}
			else if(isCheckPage() && !allChecked())/* change this to check all lower number pages */
			{
				return pageWait(numberOfPages()/2);
			}
			else
			{
				return pageWait(relativeId() + 1);
			}
		}
		else
		{
			return -1;
		}
	}
}


QString AMBeamConfigurationWizard::message(int type)
{
	if(type == Wizard_Title) return QString(tr("Beam Configuration Wizard"));
	else if(type == Help_Title) return QString(tr("Help"));
	int pageId = currentId();
	switch(pageId)
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
		if(isWaitPage(pageId))
		{
			switch(type)
			{
			case Title:
				return QString(tr("Please Wait"));
			case Text:
				return QString(tr("Moving to position %1")).arg(relativeId() + 1);
			case Help:
				return QString(tr("If the configuration is stuck here please check to make sure that motor movement is enabled.")
					       + tr(" If movement is enabled there may be a problem communicating with the sample manipulator motors."));
			case Other:
			case Default:
			default:
				break;
			}
			break;
		}
		else if(isSetPage(pageId))
		{
			if(relativeId(pageId) < numberOfPages()/2)
			{
				switch(type)
				{
				case Title:
					return QString(tr("Confirm Beam"));
				case Text:
					return QString(tr("Check to see if beam %1 is in the correct configuration.")).arg(relativeId()%3 + 1);
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
			}
			else
			{
				switch(type)
				{
				case Title:
					return QString(tr("Set Beam"));
				case Text:
					return QString(tr("Draw a box over beam position %1")).arg(relativeId()%3 + 1);
				case Help:
					return QString(tr("Draw a box over the visible beamspot.  To draw a box click and drag to create the corners.  "))
							+ tr("Try to fit the box to best encompass the entire beamspot.");
				case Other:
				case Default:
				default:
					break;
				}
				break;
			}
		}
		return QString(tr("Error - unknown page."));
	}

	return QString(tr("Error - unknown message type."));
}



void AMBeamConfigurationWizard::addPoint(QPointF position)
{

	int index = relativeId() - numberOfPoints()/2;
	QPointF* newPoint;

	if(topLeft_)
	{
		newPoint = (*pointList())[2*(index)];
		connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
		topLeft_ = !topLeft_;
	}
	else
	{
		newPoint = (*pointList())[2*(index) + 1];
	}

	*newPoint = mapPointToVideo(position);

	emit showShape(index);


}


void AMBeamConfigurationWizard::endPoint()
{
	disconnect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
	topLeft_ = true;
}



void AMBeamConfigurationWizard::showBeamShape()
{
	emit showBeamMarker(relativeId());
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
	int pageId = currentId();
	int targetPage;
	switch(pageId)
	{
	case Page_Final:
		targetPage = pageWait(numberOfPages() - 1);
		while(currentId() != targetPage)
			QWizard::back();
		if(currentId() == targetPage)
			initializePage(targetPage);
		break;
	default:
		if(isWaitPage(pageId))
		{
			bool reinitialize = true;

			((AMWaitPage*)page(pageId))->stopTimer();

			if(relativeId(pageId) == 0)
			{
				targetPage = Page_Intro;
				reinitialize = false;
			}
			else
			{
				targetPage = pageWait(relativeId(pageId) - 1);
			}

			while(currentId() != targetPage)
			{
				QWizard::back();
			}

			if(currentId() == targetPage && reinitialize)
			{
				initializePage(targetPage);
			}
		}
		else if(isSetPage(pageId))
		{
			bool reinitialize = true;
			if(relativeId(pageId) == 0)
			{
				targetPage = Page_Intro;
				reinitialize = false;
			}
			else
			{
				targetPage = pageWait(relativeId(pageId) - 1);
			}
			while(currentId() != targetPage)
			{
				QWizard::back();
			}
			if(currentId() == targetPage && reinitialize)
			{
				initializePage(targetPage);
			}
		}
		else
		{
			AMGraphicsViewWizard::back();
		}
		break;
	}

}

bool AMBeamConfigurationWizard::allChecked() const
{
	bool checkedSet = true;
	for(int i = 0; i < numberOfPages()/2; i++)
	{
		checkedSet *= checked(pageSet(i));
	}

	return checkedSet;
}

bool AMBeamConfigurationWizard::lastCheckPage() const
{
	return relativeId() == (numberOfPages()/2 - 1);
}

bool AMBeamConfigurationWizard::lastSetPage() const
{
	return relativeId() >= (numberOfPages() - 1);
}

bool AMBeamConfigurationWizard::isCheckPage() const
{
	return currentId() < numberOfPages()/2;
}


void AMBeamIntroPage::initializePage()
{
	AMWizardPage::initializePage();
	startTimer(0);
}

void AMBeamIntroPage::timerEvent(QTimerEvent *event)
{
	killTimer(event->timerId());
	viewWizard()->next();
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












