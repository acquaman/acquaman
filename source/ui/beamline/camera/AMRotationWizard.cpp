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


#include "AMRotationWizard.h"

#include <QWizard>
#include "AMSampleCameraGraphicsView.h"
#include <QAbstractButton>
#include <QDebug>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMMAXvMotor.h"

AMRotationWizard::AMRotationWizard(QWidget *parent)
	: AMGraphicsViewWizard(parent)
{
	setNumberOfPoints(3);
	setNumberOfPages(numberOfPoints());
	setFreePage(Page_Free);
	/// allow rotation during wizard operation
	setRotationEnabled(true);
	/// place two zero coordinates in the list
	for(int i = 0; i < numberOfPoints(); i ++)
	{
		coordinateListAppend(new QVector3D(0,0,0));
		pointListAppend(new QPointF(0,0));

	}

	/// add some rotations
	rotationsAppend(-30);
	rotationsAppend( 10);
	rotationsAppend( 50);

	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Check, new AMRotationCheckPage);
	setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);
	for(int i = 0; i < numberOfPages(); i++)
	{
		setPage(pageSet(i), new AMRotationSetPage);
		setPage(pageWait(i), new AMRotationWaitPage);
	}


	setStartId(Page_Intro);
	setHasHelpButton(true);
	setDefaultWindowTitle();


	/// need to disconnect and reconnect buttons to override functionality properly
	// can this go in base class?
	disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

	setMinimumSize(600,600);

	addOptionPage(Page_Intro);


}

AMRotationWizard::~AMRotationWizard()
{
}

int AMRotationWizard::nextId() const
{
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		if(showOptionPage())
			return Page_Option;
		else
			return Page_Check;
	case Page_Check:
		if(checked(Page_Check))
			return FINISHED;
		else
			return pageWait(0);
	case Page_Final:
		return FINISHED;
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
			/*
			((AMRotationSetPage*)page(Page_Set_Two))->disconnectMouseSignal();
			*/
			return pageSet(id);
		}
		else
		{
			return FINISHED;
		}

	}
}

void AMRotationWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()),rotations()->at(relativeId()));
}


QString AMRotationWizard::message(int type)
{
	if(type == Wizard_Title) return QString(tr("Rotation Wizard"));
	else if(type == Help_Title) return QString(tr("Help"));
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		switch(type)
		{
		case Title:
			return QString(tr("Centre of Rotation Configuration Wizard"));
		case Text:
			return QString(tr("This wizard is used to determine the centre of rotation of")
						   +tr(" the sample plate.  This is required for accurate tracking when rotations occur."));
		case Help:
			return QString(tr("Rotation Wizard intro page help message"));
		case Other:
		case Default:
		default:
			return QString(tr("Error - intro page - unknown message type"));
		}
	case Page_Check:
		switch(type)
		{
		case Title:
			return QString(tr("Sample Plate Alignment"));
		case Text:
			return QString(tr("Check to see if the sample plate is lined up with its outline"));
		case Help:
			return QString(tr("If the centre of rotation appears to be configured correctly, set the checkbox")
						   +tr(" to checked and click finish.  If it is not correct or is uncertain, clear the ")
						   +tr("checkbox and click next"));
		case Other:
			return QString(tr("Is the centre of rotation correct?"));
		case Default:
		default:
			return QString(tr("Error - check page - unknown message type"));
		}
	case Page_Option:
		switch(type)
		{
		case Title:
			return QString(tr("Rotation Coordinates"));
		case Text:
			return QString(tr("Set the coordinates to use in the rotation calibration.")
						   +tr("The x y and z coordinates should be left at zero, only the rotation ")
						   +tr("should be adjusted"));
		case Help:
		case Other:
		case Default:
		default:
			return QString(tr("Error - option page - unknown message type"));
		}
	case Page_Final:
		switch(type)
		{
		case Title:
			return QString(tr("Finished"));
		case Text:
			return QString(tr("Rotation calibration is now complete."));
		case Help:
		case Other:
		case Default:
		default:
			return QString(tr("Error - final page - unknown message type"));
		}
	default:
		if(isSetPage(pageId))
		{
			switch(type)
			{
			case Title:
				return QString(tr("Select Page %1 ")).arg(relativeId());
			case Text:
			case Help:
			case Other:
			case Default:
			default:
				return QString(tr("Error - set page - unknown message type"));
			}
		}
		else if(isWaitPage(pageId))
		{
			switch(type)
			{
			case Title:
				return QString(tr("Wait Page %1")).arg(relativeId());
			case Text:
			case Help:
			case Other:
			case Default:
			default:
				return QString(tr("Error - wait page - unknown message type"));
			}
		}
		else
		{
			return QString(tr("Error - unknown page"));
		}
	}
}

void AMRotationWizard::back()
{
	int id = currentId();
	if(isWaitPage(id))
	{
		((AMRotationWaitPage*)page(id))->stopTimer();
		QWizard::back();
		if(relativeId() == 0)
		{
			cleanupPage(Page_Check);
			initializePage(Page_Check);
		}
	}
	else if(isSetPage(id))
	{
		int prevId = pageWait(relativeId()-1);
		if(relativeId() == 0)
		{
			prevId = Page_Check;
		}
		while(currentId() != prevId) QWizard::back();
		if(currentId() == prevId)
		{
			if(relativeId() == 0)
			{
				cleanupPage(prevId);
			}
			initializePage(prevId);
		}
	}
	else
	{
		QWizard::back();
	}
}

void AMRotationWizard::addPoint(QPointF position)
{
	int index = relativeId();
	QPointF* newPoint;
	if(index >= 0)
	{
		newPoint = pointList()->at(index);
		QPointF newPosition = mapPointToVideo(position);
		*newPoint = newPosition;

		QVector3D *oneActualPosition = new QVector3D(SGMBeamline::sgm()->ssaManipulatorX()->value(), SGMBeamline::sgm()->ssaManipulatorY()->value(), SGMBeamline::sgm()->ssaManipulatorZ()->value());
		actualPositionListAppend(oneActualPosition);
		actualRotationListAppend(SGMBeamline::sgm()->ssaManipulatorRot()->value());

		next();
	}
}


void AMRotationWaitPage::initializePage()
{
	AMWaitPage::initializePage();
	startTimer(1000);
}


void AMRotationCheckPage::checkBoxChanged(bool state)
{
	setFinalPage(state);
}


void AMRotationSetPage::initializePage()
{
	AMViewPage::initializePage();
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}

/// this happens every time the page is left.
bool AMRotationSetPage::validatePage()
{
	disconnectSignals();
	return AMViewPage::validatePage();
}

void AMRotationSetPage::cleanupPage()
{
	disconnectSignals();
}

void AMRotationSetPage::addPoint(QPointF position)
{
	((AMRotationWizard*)viewWizard())->addPoint(position);
}

void AMRotationSetPage::disconnectSignals()
{
	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}
