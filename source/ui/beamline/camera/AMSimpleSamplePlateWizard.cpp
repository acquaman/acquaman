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


#include "AMSimpleSamplePlateWizard.h"

#include "beamline/camera/AMShapeData.h"
#include <QGraphicsItem>
#include <QAbstractButton>
#include "AMSampleCameraGraphicsView.h"
#include <QVector>
#include <QVector3D>
#include <QDebug>

AMSimpleSamplePlateWizard::AMSimpleSamplePlateWizard(QWidget *parent)
	: AMGraphicsViewWizard(parent)
{
	/* page automation initialization */
	setNumberOfPoints(1);
	setNumberOfPages(numberOfPoints());
	setFreePage(Page_Free);

	setRotationEnabled(true);

	coordinateListAppend(new QVector3D(0,0,0));
	rotationsAppend(0);

	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);

	for(int i = 0; i < numberOfPages(); i++)
	{
		qDebug() << "Setting wait page and set page for index " << i;
		setPage(pageSet(i), new AMSimpleSampleSetPage);
		setPage(pageWait(i), new AMSimpleSampleWaitPage);
		connect(page(pageSet(i)), SIGNAL(signalMousePressed(QPointF)), this, SLOT(setPageMousePressedHandler(QPointF)));
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

AMSimpleSamplePlateWizard::~AMSimpleSamplePlateWizard()
{
}

int AMSimpleSamplePlateWizard::nextId() const
{
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		if(showOptionPage())
			return Page_Option;
		else{
			return pageWait(0);
//			return pageSet(0);
		}
	case Page_Final:
		return -1;
	default:
		int id = relativeId();
		if(isWaitPage(pageId))
		{
			for(int j = 0; j < numberOfPages(); j++)
			{
				/* make sure clicks are disconnected (just in case) */
				if(j != id)
				{
//					((AMSimpleSampleSetPage*)page(pageSet(j)))->disconnectMouseSignal();
				}
			}

			return pageSet(id);
		}
		else if(isSetPage(pageId))
		{
			if(id == numberOfPages() -1)
			{
				return Page_Final;
			}
			else
			{
				return pageWait(id+1);
			}
		}
		return -1;
	}
}

void AMSimpleSamplePlateWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()), rotations()->at(relativeId()));
}

QString AMSimpleSamplePlateWizard::message(int type)
{
	if(type == Wizard_Title) return QString(tr("Sample Plate Wizard"));
	else if(type == Help_Title) return QString(tr("Help"));
	int pageId = currentId();
	switch(pageId)
	{
	case Page_Intro:
		switch(type)
		{
			case Title:
				return QString(tr("Introduction to Sample Plate Wizard"));
			case Text:
			return QString(tr("On the first page, grab the box and move it so that the light blue box outlines the sample plate."));
			case Help:
				return QString(tr("This is the sample plate configuration wizard, which is designed to ensure that your plate is properly")
					       + tr(" aligned in the outer sample holder.  To start the calibration")
					       + tr(" select next on this page."));
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
				return QString(tr("Calibration Complete"));
			case Text:
				return QString(tr("The calibration has been completed and the sample plate is ready.  Select 'Finish' to exit the calibration."));
			case Help:
				return QString(tr("The calibration has now been completed.  If you would like to reset the sample plate position simply run the wizard again."));
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
				return QString(tr("Message - wait page - Title"));
			case Text:
				return QString(tr("Message - wait page - Text"));
			case Help:
				return QString(tr("Message - wait page - Help text"));
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
				return QString(tr("Set the Sample Plate"));
			case Text:
				return QString(tr("You can align the sample plate by selecting the light blue box and placing it over the sample plate.")
					       + tr(" The simplest approach is usually to align the left side of the blue box with the left side of the plate")
					       + tr(" and the top of the blue box with the top of the sample plate."));
			case Help:
				return QString(tr("If you have any issues check that the manipulator rotation is set to 0.0. It's often easiest to position the plate")
					       + tr(" from the Measurement Position. You can make sure this is the case by pressing the 'Measure Position' button located")
					       + tr(" below the sample camera video."));
			case Other:
			case Default:
			default:
				return QString(tr("Error - set page - unknown message type."));
			}
		}
		return QString(tr("Error message - unknown page - unknown message type"));
	}
}

void AMSimpleSamplePlateWizard::back()
{
	int pageId = currentId();
	int pageRelativeId = relativeId();
	if(isWaitPage(pageId))
	{
		((AMWaitPage*)page(pageId))->stopTimer();
		QWizard::back();
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

void AMSimpleSamplePlateWizard::addShape(AMShapeData *shape)
{
	QGraphicsPolygonItem *sampleShapeItem = new QGraphicsPolygonItem(*shape->shape());
	view()->scene()->addItem(sampleShapeItem);
}

void AMSimpleSamplePlateWizard::shiftSampleShape(QPointF shift)
{
	emit signalShiftSampleShape(mapPointToVideo(shift));
}

void AMSimpleSamplePlateWizard::initializeShape()
{
	emit signalInitializeSampleShape();

}

void AMSimpleSamplePlateWizard::setPageMousePressedHandler(QPointF point)
{
	emit signalSamplePlateWizardMousePressed(point);
}



void AMSimpleSampleSetPage::initializePage()
{
	AMViewPage::initializePage();
	disconnectMouseSignal();
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(selectShape(QPointF)));
	connect(view(), SIGNAL(mouseLeftReleased(QPointF)), this, SLOT(releaseShape()));
	((AMSimpleSamplePlateWizard*)wizard())->initializeShape();
}

void AMSimpleSampleSetPage::selectShape(QPointF point)
{
	emit signalMousePressed(point);
	connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(moveShape(QPointF)));
}

void AMSimpleSampleSetPage::releaseShape()
{
	disconnect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(moveShape(QPointF)));
}

void AMSimpleSampleSetPage::moveShape(QPointF point)
{

	((AMSimpleSamplePlateWizard*)wizard())->shiftSampleShape(point);
}

void AMSimpleSampleSetPage::disconnectMouseSignal()
{
	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(selectShape(QPointF)));
	disconnect(view(), SIGNAL(mouseReleased(QPointF)), this, SLOT(releaseShape()));
}


void AMSimpleSampleWaitPage::initializePage()
{
	qDebug() << "Here in AMSimpleSampleWaitPage::initializePage";
	AMWaitPage::initializePage();
	AMWaitPage::startTimer(1000);
	viewWizard()->waitPage();
}

