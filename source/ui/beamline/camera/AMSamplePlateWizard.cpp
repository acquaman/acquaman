#include "AMSamplePlateWizard.h"

#include <QAbstractButton>
#include <QSlider>
#include <QLayout>
#include <QGraphicsRectItem>

#include "beamline/camera/AMBeamConfiguration.h"
#include "beamline/camera/AMSampleCamera.h"
#include "AMSampleCameraGraphicsView.h"
#include "util/AMErrorMonitor.h"

AMSamplePlateWizard::AMSamplePlateWizard(QWidget* parent)
	: AMGraphicsViewWizard(parent)
{
	/// Used to generate and allow modification of the option page
	// 2 points should be sufficient to get an accurate result
	setNumberOfPoints(4);
	setNumberOfPages(numberOfPoints());
	setFreePage(Page_Free);
	/// allow the rotation to be changed during wizard operation
	setRotationEnabled(true);
	/// need a coordinate per point
	coordinateListAppend(new QVector3D(0,0,0));
	coordinateListAppend(new QVector3D(5,5,2));
	coordinateListAppend(new QVector3D(2.5,-4,2.5));
	coordinateListAppend(new QVector3D(-5,2,1));

	rotationsAppend(0);
	rotationsAppend(-20);
	rotationsAppend(20);
	rotationsAppend(40);

	/// Add the pages
	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Check, new AMSampleCheckPage);
	setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);
	/// Add the automatic set and wait pages
	for(int i = 0; i < numberOfPages(); i++)
	{
		setPage(pageWait(i),new AMSampleWaitPage);
		setPage(pageSet(i), new AMSampleSetPage);
	}
	setStartId(Page_Intro);
	setOption(HaveHelpButton, true);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
	setWindowTitle(message(Wizard_Title));

	/// need to disconnect and reconnect buttons to override functionality properly

	disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));


	setMinimumSize(600,600);

	/// Add the options button to the intro page
	addOptionPage(Page_Intro);
	for(int i = 0; i < numberOfPages(); i++)
	{
		addResetPointsButton(pageSet(i));
	}
	/// make sure the reset button isn't shown right away
	showResetButton(currentId());

}

AMSamplePlateWizard::~AMSamplePlateWizard()
{
}

int AMSamplePlateWizard::nextId() const
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
			return -1;
		else
			return pageWait(0);
	case Page_Final:
		return -1;
	default:
		int id = relativeId();
		if(isWaitPage(pageId))
		{
			if(currentId() == pageWait(id))
			{
				for(int j = 0; j < numberOfPages(); j++)
				{
					if(j != id)
					{
						/// make sure all the click signals are disconnected
						((AMSampleSetPage*)page(pageSet(j)))->disconnectMouseSignal();
					}
				}
				return pageSet(id);
			}
			else
				AMErrorMon::alert(this, AMSAMPLEPLATEWIZARD_NEXTID_ERROR_FOR_WAITPAGE, QString("There was an error determining the next page for AMSamplePlateWizard with current page %1.").arg(pageId) );
		}
		else if(isSetPage(pageId))
		{
			if(currentId() == pageSet(id))
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
			else
				AMErrorMon::alert(this, AMSAMPLEPLATEWIZARD_NEXTID_ERROR_FOR_SETPAGE, QString("There was an error determining the next page for AMSamplePlateWizard with current page %1.").arg(pageId) );
		}

		return -1;
	}
}

void AMSamplePlateWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()),rotations()->at(relativeId()));
}


QString AMSamplePlateWizard::message(int type)
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
			return QString(tr("Sample Plate Configuration"));
		case Text:
			return QString(tr("Use this configuration tool to determine the position of the sample plate."));
		case Help:
			return QString(tr("This configuration allows you to determine the position of the sample plate")
				       +tr("by identifying the face of the plate as it moves around."));
		case Other:
		case Default:
		default:
			return QString(tr("Error- Intro default - unknown message type."));
		}
	case Page_Check:
		switch(type)
		{
		case Title:
			return QString(tr("Sample Plate Alignment"));
		case Text:
			return QString(tr("Check to see if the sample plate is properly lined up with its outline."));
		case Help:
			return QString(tr("If the sample plate appears to be lined up with the visible outline, ensure that")
				       + tr(" the checkbox on this page is checked.  If the sample plate does not line up,")
				       + tr(" ensure the box is not checked."));
		case Other:
			return QString(tr("Does the sample plate line up with the outline?"));
		case Default:
		default:
			return QString(tr("Error - check page - unknown message type."));
		}
	case Page_Option:
		switch(type)
		{
		case Title:
			return QString(tr("Coordinate Set Up"));
		case Text:
			return QString(tr("Set the coordinates to move to from here."));
		case Help:
			return QString(tr("Set the motor coordinates that will be moved to")
				       + tr(" during the sample plate setup from this page."));
		case Other:
		case Default:
		default:
			return QString(tr("Error message - option page - unknown message type."));
		}
	case Page_Final:
		switch(type)
		{
		case Title:
			return QString(tr("Calibration Complete"));
		case Text:
			return QString(tr("Calibration is complete.  If the sample plate does not line up, ")
				       +tr("ensure that the camera and rotation are correctly configured."));
		case Help:
			return QString(tr("The sample plate calibration depends on the camera and rotation to ")
				       +tr("be correctly configured before it will work.  If you have run both")
				       +tr("configurations already, make sure that the direction of rotation is correct."));
		case Other:
		case Default:
			return QString(tr("Error message - final page - unknown message type"));
		}
		/// handle the wait and set pages here
	default:
		int id = relativeId(pageId);
		if(isWaitPage(id))
		{
			if(currentId() == pageWait(id))
			{
				switch(type)
				{
				case Title:
					return QString(tr("Moving to position %1")).arg(relativeId() + 1);
				case Text:
					return QString(tr("Wait while the sample plate moves."));
				case Help:
					return QString(tr("If this window is stuck in this state, there may be a problem communicating with")
						       + tr(" the motor.  Ensure that motor movement is enabled."));
				case Other:
				case Default:
				default:
					return QString(tr("Error - Wait page - unknown message type."));
				}
			}
		}
		else if(isSetPage(id))
		{
			if(currentId() == pageSet(id))
			{
				switch(type)
				{
				case Title:
					return QString(tr("Sample Plate Adjustment"));
				case Text:
					return QString(tr("Select the plate corresponding to the coordinate: %1, %2, %3")).arg(coordinateX(relativeId())).arg(coordinateY(relativeId())).arg(coordinateZ(relativeId()));
				case Help:
					return QString(tr("To configure the sample plate, select each corner of the sample plate on this page.")
						       +tr("When you click a point, a small square should appear whose top left corner is aligned with")
						       +tr("where you have clicked.  You may press reset to clear all the points selected on this")
						       +tr("page (it will keep all points selected on other pages"));
				case Other:
				case Default:
				default:
					return QString(tr("Error message - set page - unknown message type."));
				}
			}
		}
	}
	return QString(tr("Error message - unknown page type."));
}


void AMSamplePlateWizard::back()
{
	int id = currentId();
	int pageRelativeId = relativeId();
	if(isWaitPage(id))
	{
		((AMWaitPage*)page(id))->stopTimer();
		QWizard::back();
		if(currentId() == Page_Check)
		{
			cleanupPage(Page_Check);
			initializePage(Page_Check);
		}
	}
	else if(isSetPage(id))
	{
		if(relativeId() == 0)
		{
			((AMSampleSetPage*)page(id))->disconnectMouseSignal();
			while(currentId() != Page_Check)
			{
				QWizard::back();
			}
			if(currentId() == Page_Check)
			{
				cleanupPage(Page_Check);
				initializePage(Page_Check);
			}
		}
		else
		{
			((AMSampleSetPage*)page(id))->disconnectMouseSignal();
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


void AMSamplePlateWizard::addPoint(QPointF position)
{
	/// This should add a point to the list every time the view is clicked.
	int pageId = relativeId();

	QPointF* newPoint = new QPointF(position);
	/// default point size
	const QSizeF defaultSize = QSizeF(10,10);
	QPen defaultPen;
	/// Must keep track of which points are from which page.
	/// Pad the list with QPointF(0,0) seperators
	/// list will go (0,0),...page1...,(0,0),...page2...,(0,0)...etc...,...finalpage...,(0,0)
	if(pointList()->isEmpty())
	{
		for(int i = 0; i < numberOfPages() + 1; i ++)
		{
			pointListAppend(new QPointF(0,0));
		}
	}
	/// shift if it is (0,0) so that it is not confused with padding
	/// shifting over by 0.00001 is much less than the minimum mouse movement
	if(*newPoint == QPointF(0,0))
	{
		*newPoint = *newPoint + QPointF(0.00001,0);
	}
	int count = 0;
	int index = 0;
	/// need to count 2 sets of zero for page one, 3 for page two, etc.
	for(int i = 0; count < pageId+2 && i < pointList()->count(); i++)
	{
		if(*pointList()->at(i) == QPointF(0,0))
		{
			count++;
		}
		index = i;
	}
	pointList()->insert(index,newPoint);
	defaultPen = getDefaultPen();

	QRectF rectangle(view()->mapVideoToScene(*newPoint), defaultSize); /// need to map new point to the actual screen position
	/// add a square to show where the mouse was clicked
	QGraphicsRectItem* rectItem = view()->scene()->addRect(rectangle, defaultPen);
	samplePointShapes_<<rectItem;


	((AMSampleSetPage*)currentPage())->insertPoint(newPoint);

}

QPen AMSamplePlateWizard::getDefaultPen()
{
	switch(relativeId())
	{
	case 0:
		return QPen(Qt::yellow);
	case 1:
		return QPen(Qt::green);
	case 2:
		return QPen(Qt::cyan);
	default:
		return QPen(Qt::white);
	}
}

void AMSamplePlateWizard::removePoint(QPointF *point)
{
	if(pointList()->contains(point))
	{
		pointList()->removeAll(point);
		/// get rid of the drawing of the point.
		foreach(QGraphicsRectItem* rect, samplePointShapes_)
		{
			if(rect->rect().topLeft() == view()->mapVideoToScene(*point))
			{
				samplePointShapes_.remove(samplePointShapes_.indexOf(rect));
				view()->scene()->removeItem(rect);
				delete rect;
			}
		}

		delete(point);

	}
}



void AMSampleWaitPage::initializePage()
{
	AMWaitPage::initializePage();
	startTimer(1000);
}


void AMSampleSetPage::initializePage()
{
	AMViewPage::initializePage();
	/// disconnect and reconnect addPoint.
	//	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
	disconnectMouseSignal();
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));

}

void AMSampleSetPage::addPoint(QPointF position)
{
	((AMSamplePlateWizard*)viewWizard())->addPoint(position);
}

/// Insert the QPointF* into the local pages list.
void AMSampleSetPage::insertPoint(QPointF *position)
{
	localPoints_<<position;
}

void AMSampleSetPage::disconnectMouseSignal()
{
	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}


void AMSampleSetPage::resetPoints()
{
	/// This doesn't delete the points, as it didn't make them
	/// it is up to the wizard to delete the points.
	foreach(QPointF* point, localPoints_)
	{
		localPoints_.remove(localPoints_.indexOf(point));
		((AMSamplePlateWizard*)viewWizard())->removePoint(point);
	}
}



void AMSampleCheckPage::checkBoxChanged(bool state)
{
	setFinalPage(state);
}


double AMSamplePlateWizard::coordinateX(int id)
{
	return coordinateList()->at(id)->x();
}

double AMSamplePlateWizard::coordinateY(int id)
{
	return coordinateList()->at(id)->y();
}

double AMSamplePlateWizard::coordinateZ(int id)
{
	return coordinateList()->at(id)->z();
}

void AMSamplePlateWizard::addResetPointsButton(int id)
{
	setOption(HaveCustomButton2);
	setButtonText(CustomButton2, "Reset");
	connect(this, SIGNAL(customButtonClicked(int)), this, SLOT(triggerReset(int)));
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(showResetButton(int)));
	addResetPage(id);

}

void AMSamplePlateWizard::triggerReset(int id)
{
	if(id == CustomButton2)
	{
		if(resetPages_.contains(currentId()))
			((AMSampleSetPage*)currentPage())->resetPoints();
		else
			AMErrorMon::alert(this, AMSAMPLEPLATEWIZARD_TRIGGER_RESET_FROM_INVALID_PAGE, QString("AMSamplePlateWizard attempted to triggerReset() on an invalid page (%1).").arg(id) );
	}
}

void AMSamplePlateWizard::addResetPage(int id)
{
	resetPages_<<id;
}

void AMSamplePlateWizard::showResetButton(int id)
{
	if(resetPages_.contains(id))
	{
		button(CustomButton2)->show();
	}
	else if(button(CustomButton2)->isVisible())
	{
		button(CustomButton2)->hide();
	}
}


void AMSampleIntroPage::initializePage()
{
	AMWizardPage::initializePage();
	startTimer(0);
}

void AMSampleIntroPage::timerEvent(QTimerEvent *event)
{
	killTimer(event->timerId());
	viewWizard()->next();
}

double AMSamplePlateWizard::requestMotorRotation()
{
	emit requestRotation();
	return currentRotation();

}

void AMSamplePlateWizard::setCurrentRotation(double currentRotation)
{
	currentRotation_ = currentRotation;
}

double AMSamplePlateWizard::currentRotation()
{
	return currentRotation_;
}





