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
	qDebug()<<"Here in AMSimpleBeamConfigurationWizard::nextId()";
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
		{
			return pageSet(id);
		}
		else if(isSetPage(pageId))
		{
			if(id == numberOfPages() - 1)
			{
				return Page_Final;
			}
			else
			{
				return pageWait(id + 1);
			}
		}
		return -1;
	}
}

void AMSimpleBeamConfigurationWizard::waitPage()
{
	qDebug()<<"Here in  AMSimpleBeamConfigurationWizard::waitPage";
	emit moveTo(*coordinateList()->at(relativeId()));
	qDebug()<<"Leaving  AMSimpleBeamConfigurationWizard::waitPage";
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
	qDebug()<<"AMSimpleBeamConfigurationWizard::mousePressedHandler";
	emit mousePressed(mapPointToVideo(point), relativeId());
}

void AMSimpleBeamConfigurationWizard::moveBeamShapeHandler(QPointF point)
{
	qDebug()<<"AMSimpleBeamConfigurationWizard::moveBeamShapeHandler";
	emit moveBeamShape(mapPointToVideo(point), relativeId());
}

void AMSimpleBeamConfigurationSetPage::initializePage()
{
	qDebug()<<"Start AMSimpleBeamConfigurationSetPage::initializePage";
	AMViewPage::initializePage();
	disconnectMouseSignal();
	qDebug()<<"AMSimpleBeamConfigurationSetPage::initializePage - done disconnect";
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(selectShape(QPointF)));
	connect(view(), SIGNAL(mouseLeftReleased(QPointF)), this, SLOT(releaseShape()));
	qDebug()<<"AMSimpleBeamConfigurationSetPage::initializePage - done connect";
	emit initBeamShape();
	qDebug()<<"Done AMSimpleBeamConfigurationSetPage::initializePage";
}

void AMSimpleBeamConfigurationWaitPage::initializePage()
{
	AMWaitPage::initializePage();
	AMWaitPage::startTimer(1000);
	qDebug()<<"About to call wait page AMSimpleBeamConfigurationWaitPage::initializePage";
	viewWizard()->waitPage();
	qDebug()<<"Done calling wait page AMSimpleBeamConfigurationWaitPage::initializePage";
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



