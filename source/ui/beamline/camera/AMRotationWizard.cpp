#include "AMRotationWizard.h"

#include <QWizard>
#include "AMSampleCameraGraphicsView.h"
#include <QAbstractButton>

AMRotationWizard::AMRotationWizard(QWidget *parent)
	: AMGraphicsViewWizard(parent)
{
	/// need only two points to calculate since the first one (rotation == 0) is inherently known
	numberOfPoints_ = 2;
	showOptionPage_ = false;
	/// allow rotation during wizard operation
	setRotationEnabled(true);
	coordinateList_->clear();
	rotations_->clear();
	/// place two zero coordinates in the list
	coordinateList_->append(new QVector3D(0,0,0));
	coordinateList_->append(new QVector3D(0,0,0));

	/// add some rotations
	rotations_->append(15);
	rotations_->append(30);

	setPage(Page_Intro, new AMWizardPage);
	setPage(Page_Check, new AMRotationCheckPage);
	setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);
	setPage(Page_Set_One, new AMRotationSetPage);
	setPage(Page_Set_Two, new AMRotationSetPage);
	setPage(Page_Wait_One, new AMRotationWaitPage);
	setPage(Page_Wait_Two, new AMRotationWaitPage);

	setStartId(Page_Intro);
	setOption(HaveHelpButton, true);
	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
	setWindowTitle(message(Wizard_Title));


	/// need to disconnect and reconnect buttons to override functionality properly

	disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
	connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

	setMinimumSize(600,600);

	addOptionPage(Page_Intro);
	pointList_->clear();
	pointList_->append(new QPointF(0,0));
	pointList_->append(new QPointF(0,0));

}

AMRotationWizard::~AMRotationWizard()
{
}

int AMRotationWizard::nextId() const
{
	switch(currentId())
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
			return Page_Wait_One;
	case Page_Final:
		return -1;
	case Page_Set_One:
		return Page_Wait_Two;
	case Page_Set_Two:
		return Page_Final;
	case Page_Wait_One:
//		((AMRotationSetPage*)page(Page_Set_Two))->disconnectMouseSignal();
		return Page_Set_One;
	case Page_Wait_Two:
//		((AMRotationSetPage*)page(Page_Set_One))->disconnectMouseSignal();
		return Page_Set_Two;
	default:
		return -1;
	}
}

void AMRotationWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()-1),rotations_->at(relativeId()-1));
}

int AMRotationWizard::relativeId()
{
	switch(currentId())
	{
	case Page_Set_One:
	case Page_Wait_One:
		return 1;
	case Page_Set_Two:
	case Page_Wait_Two:
		return 2;
	default:
		return 0;
	}
}

QString AMRotationWizard::message(int type)
{
	if(type == Wizard_Title) return QString(tr("Rotation Wizard"));
	else if(type == Help_Title) return QString(tr("Help"));
	switch(currentId())
	{
	case Page_Intro:
		switch(type)
		{
		case Title:
			return QString(tr("Centre of Rotation Configuration Wizard"));
		case Text:
			return QString(tr("This wizard is used to determine the centre of rotaiton of")
						   +tr(" the sample plate.  This is required for accurate tracking when rotation occur."));
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
		case Text:
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
		case Text:
		case Help:
		case Other:
		case Default:
		default:
			return QString(tr("Error - final page - unknown message type"));
		}
	case Page_Set_One:
	case Page_Set_Two:
		switch(type)
		{
		case Title:
		case Text:
		case Help:
		case Other:
		case Default:
		default:
			return QString(tr("Error - set page - unknown message type"));
		}
	case Page_Wait_One:
	case Page_Wait_Two:
		switch(type)
		{
		case Title:
		case Text:
		case Help:
		case Other:
		case Default:
		default:
			return QString(tr("Error - wait page - unknown message type"));
		}
	default:
		return QString(tr("Error - unknown page"));
	}
}

void AMRotationWizard::back()
{
	int id = currentId();
	switch(id)
	{
	case Page_Wait_One:
		((AMRotationWaitPage*)page(id))->stopTimer();
		QWizard::back();
		cleanupPage(Page_Check);
		initializePage(Page_Check);
		break;
	case Page_Wait_Two:
		((AMRotationWaitPage*)page(id))->stopTimer();
		QWizard::back();
		break;
	case Page_Set_One:
		while(currentId() != Page_Check) QWizard::back();
		if(currentId() == Page_Check)
		{
			cleanupPage(Page_Check);
			initializePage(Page_Check);
		}
		break;
	case Page_Set_Two:
		while(currentId() != Page_Wait_One) QWizard::back();
		if(currentId() == Page_Wait_One)
		{
			initializePage(Page_Wait_One);
		}
		break;
	default:
		QWizard::back();
	}
}

void AMRotationWizard::addPoint(QPointF position)
{
	int index = relativeId() -1;
	QPointF* newPoint;
	if(index >= 0)
	{
		newPoint = pointList_->at(index);
		QPointF newPosition = mapPointToVideo(position);
		*newPoint = newPosition;
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
	disconnect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
	connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}

void AMRotationSetPage::addPoint(QPointF position)
{
	((AMRotationWizard*)viewWizard())->addPoint(position);
}
