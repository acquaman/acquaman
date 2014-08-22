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


#include "AMSimpleBeamConfigurationWizard.h"

#include "AMSampleCameraGraphicsView.h"
#include <QAbstractButton>
#include <QDebug>

AMSimpleBeamConfigurationWizard::AMSimpleBeamConfigurationWizard(QWidget *parent)
	: AMGraphicsViewWizard(parent)
{
	setNumberOfPoints(3);
	setNumberOfPages(numberOfPoints());
	setFreePage(Page_Free);

	coordinateListAppend(new QVector3D(0,0,0));
	coordinateListAppend(new QVector3D(0,2,0));
	coordinateListAppend(new QVector3D(0,-2,0));

	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);
	for(int i = 0; i < numberOfPages(); i++)
	{
		setPage(pageWait(i), new AMSimpleBeamConfigurationWaitPage);
		setPage(pageSet(i), new AMSimpleBeamConfigurationSetPage);
		connect(page(pageSet(i)), SIGNAL(initBeamShape()), this, SLOT(initBeamShape()));
		connect(page(pageSet(i)), SIGNAL(signalMousePressed(QPointF)), this, SLOT(mousePressedHandler(QPointF)));
		connect(page(pageSet(i)), SIGNAL(moveBeamShape(QPointF)), this, SLOT(moveBeamShapeHandler(QPointF)));
	}

	setStartId(Page_Intro);
	setOption(HaveHelpButton, true);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));

	setWindowTitle(message(Wizard_Title));

	disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

	setMinimumSize(600,600);

	/// Add options button to intro page
	addOptionPage(Page_Intro);

}

AMSimpleBeamConfigurationWizard::~AMSimpleBeamConfigurationWizard()
{
}

int AMSimpleBeamConfigurationWizard::nextId() const
{
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		if(showOptionPage())
			return Page_Option;
		else
			return pageWait(0);
	case Page_Final:
		return -1;
	default:
		int id = relativeId();
		if(isWaitPage(pageId))
			return pageSet(id);
		else if(isSetPage(pageId))
		{
			if(id == numberOfPages() - 1)
				return Page_Final;
			else
				return pageWait(id + 1);
		}
		return -1;
	}
}

void AMSimpleBeamConfigurationWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()));
}


QString AMSimpleBeamConfigurationWizard::message(int type)
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
			return QString(tr("Introduction to Beam Configuration Wizard"));
		case Text:
			return QString(tr("On the first page, check to see if the beam appears to line up correctly with the video image.")
				       + tr("  If it does, check that it is correct and continue.  If it is not lined up correctly")
				       + tr(" you will have to run through the calibration.  To perform the calibration simply drag")
				       + tr(" the box over the visible light for each page the wizard provides."));
		case Help:
			return QString(tr("This is the beam configuration wizard, which is designed to ensure that the beam spot is properly")
				       + tr(" calibrated. Make sure that the visible light is turned on before you start."));
		case Other:
		case Default:
		default:
			return QString(tr("Error - intro page - unknown message type."));
		}
	case Page_Option:
		switch(type)
		{
		case Title:
			return QString(tr("Message - option page - Title"));
		case Text:
			return QString(tr("Message - option page - Text"));
		case Help:
			return QString(tr("Message - option page - Help text"));
		case Other:
		case Default:
		default:
			return QString(tr("Error - option page - unknown message type."));
		}
	case Page_Final:
		switch(type)
		{
		case Title:
			return QString(tr("Calibration Completed"));
		case Text:
			return QString(tr("The calibration has been completed. Select 'Finish' to exit."));
		case Help:
			return QString(tr("The calibration has now been completed.  If you would like to reposition any box, simply navigate using 'back'")
				       + tr(" until you reach the box you wish to reselect.  Any page where you do not click on the screen will")
				       + tr(" keep the box that you had previously selected."));
		case Other:
		case Default:
		default:
			return QString(tr("Error - final page - unknown message type."));
		}
	default:
		if(isWaitPage(pageId))
		{
			switch(type)
			{
			case Title:
				return QString(tr("Moving to position %1")).arg(relativeId()+1);
			case Text:
				return QString(tr("Please wait until the manipulator has finished moving."));
			case Help:
				return QString(tr("The sample manipulator is attempting to move to the next position for calibration.  If")
					       + tr(" the video does not reappear within a few moments there may be a problem communicating")
					       + tr(" with the motors.  Please ensure that motor movement is enabled.  You may re-attempt")
					       + tr(" this movement by selecting back, followed by next."));
			case Other:
			case Default:
			default:
				return QString(tr("Error - wait page - unknown message type."));
			}
		}
		else if(isSetPage(pageId))
		{
			switch(type)
			{
			case Title:
				return QString(tr("Selection Page %1")).arg(relativeId()+1);
			case Text:
				return QString(tr("Position the box over the beam corresponding to the coordinate: %1, %2, %3")).arg(coordinateList()->at(relativeId())->x()).arg(coordinateList()->at(relativeId())->y()).arg(coordinateList()->at(relativeId())->z());
			case Help:
				return QString(tr("For each selection you will need to position the box over the visible beam spot.  Upon clicking next, the")
					       + tr(" sample manipulator will automatically move on to the next point.  If the manipulator has not moved")
					       + tr(" movement may have been disabled.  Check to see if the motor movement enabled box is checked in the settings."));
			case Other:
			case Default:
			default:
				return QString(tr("Error - set page - unknown message type."));
			}
		}
		return QString(tr("Error message - unknown page - unknown message type"));
	}
}

void AMSimpleBeamConfigurationWizard::back()
{
	int pageId = currentId();
	int pageRelativeId = relativeId();
	if(isWaitPage(pageId))
	{
		((AMWaitPage*)page(pageId))->stopTimer();
		if(relativeId() == 0)
		{
			((AMWaitPage*)page(pageId))->stopTimer();
			QWizard::back();
		}
		else
		{
			while(currentId() != pageWait(pageRelativeId - 1))
			{
				QWizard::back();
			}
			if(currentId() == pageWait(pageRelativeId - 1))
			{
				initializePage(pageWait(pageRelativeId - 1));
			}
		}
	}
	else if (isSetPage(pageId))
	{
		//		((AMSimpleSampleSetPage*)page(pageId))->disconnectMouseSignal();
		if(relativeId() == 0)
		{
			while(currentId() != Page_Intro)
			{
				QWizard::back();
			}
		}
		else
		{
			while(currentId() != pageWait(pageRelativeId))
			{
				QWizard::back();
			}
			if(currentId() == pageWait(pageRelativeId))
			{
				initializePage(pageWait(pageRelativeId));
			}
		}
	}
	else
	{
		QWizard::back();
	}
}

void AMSimpleBeamConfigurationWizard::initBeamShape()
{
	emit createBeamShape(relativeId());
}

void AMSimpleBeamConfigurationWizard::mousePressedHandler(QPointF point)
{
	emit mousePressed(mapPointToVideo(point), relativeId());
}

void AMSimpleBeamConfigurationWizard::moveBeamShapeHandler(QPointF point)
{
	emit moveBeamShape(mapPointToVideo(point), relativeId());
}

void AMSimpleBeamConfigurationSetPage::initializePage()
{
	AMViewPage::initializePage();
	disconnectMouseSignal();
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(selectShape(QPointF)));
	connect(view(), SIGNAL(mouseLeftReleased(QPointF)), this, SLOT(releaseShape()));
	emit initBeamShape();
}

void AMSimpleBeamConfigurationWaitPage::initializePage()
{
	AMWaitPage::initializePage();
	AMWaitPage::startTimer(1000);
	viewWizard()->waitPage();
}


void AMSimpleBeamConfigurationSetPage::selectShape(QPointF point)
{
	emit signalMousePressed(point);
	connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(moveShape(QPointF)));
}

void AMSimpleBeamConfigurationSetPage::releaseShape()
{
	disconnect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(moveShape(QPointF)));
}

void AMSimpleBeamConfigurationSetPage::moveShape(QPointF point)
{
	emit moveBeamShape(point);
}

void AMSimpleBeamConfigurationSetPage::disconnectMouseSignal()
{
	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(selectShape(QPointF)));
	disconnect(view(), SIGNAL(mouseReleased(QPointF)), this, SLOT(releaseShape()));
}



