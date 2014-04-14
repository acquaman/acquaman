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
		setPage(pageWait(i), new AMSimpleSampleWaitPage);
		setPage(pageSet(i), new AMSimpleSampleSetPage);
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
		else
			return pageSet(0);
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
				return QString(tr("Message - intro page - Title"));
			case Text:
				return QString(tr("Message - intro page - Text"));
			case Help:
				return QString(tr("Message - intro page - Help text"));
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
				return QString(tr("Message - final page - Title"));
			case Text:
				return QString(tr("Message - final page - Text"));
			case Help:
				return QString(tr("Message - final page - Help text"));
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
				return QString(tr("Message - set page - Title"));
			case Text:
				return QString(tr("Message - set page - Text"));
			case Help:
				return QString(tr("Message - set page - Help text"));
			case Other:
			case Default:
			default:
				return QString(tr("Error - set page - unknown message type."));
			}
		}
		return QString(tr("Error message - unknown page - unknown message type"));
	}
}

bool AMSimpleSamplePlateWizard::isPointInShape(QPointF point)
{
	return false;
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
//	sampleShape_ = new AMShapeData();
//	QVector3D baseCoordinate = *coordinateList()->at(0);
//	QVector3D plateWidth = QVector3D(20,0,0);
//	QVector3D plateHeight = QVector3D(0,0,-20);
//	QVector<QVector3D> shape;
//	shape<<QVector3D(baseCoordinate)
//		 <<QVector3D(baseCoordinate + plateWidth)
//		 <<QVector3D(baseCoordinate + plateWidth + plateHeight)
//		 <<QVector3D(baseCoordinate + plateHeight);
//	sampleShape_->setCoordinateShape(shape);
////	sampleShapeItem_ = view()->scene()->addPolygon(*sampleShape_->shape());
//	QVector<QPointF> points;
//	for(int i = 0; i < 4; i++)
//		points<<QPointF(shape.at(i).toPointF());
//	QBrush brush = QBrush(Qt::green);
//	QPen pen = QPen(Qt::green);
//	QPolygonF polygon(QRectF(5, 5, 20, 20));
	//	sampleShapeItem_ = view()->scene()->addPolygon(polygon,pen, brush);
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

bool AMSimpleSampleSetPage::pointInShape(QPointF point)
{
	return ((AMSimpleSamplePlateWizard*)wizard())->isPointInShape(point);
}

void AMSimpleSampleSetPage::selectShape(QPointF point)
{
	if(pointInShape(point) || true)
	{
		emit signalMousePressed(point);
		connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(moveShape(QPointF)));
	}
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
}

