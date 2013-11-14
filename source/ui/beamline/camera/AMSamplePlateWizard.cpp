#include "AMSamplePlateWizard.h"

#include "beamline/camera/AMBeamConfiguration.h"
#include <QAbstractButton>
#include <QSlider>
#include <QLayout>
#include <QDebug>

AMSamplePlateWizard::AMSamplePlateWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
	/// Used to generate and allow modification of the option page
	numberOfPoints_ = 2;
	coordinateList_->clear();
    coordinateList_->append(new QVector3D(0,0,0));
    coordinateList_->append(new QVector3D(0,1,0));


    setPage(Page_Intro, new AMWizardPage);
    setPage(Page_Check, new AMSampleCheckPage);
	setPage(Page_Wait_One, new AMSampleWaitPage);
	setPage(Page_Wait_Two, new AMSampleWaitPage);
	setPage(Page_Set_One, new AMSampleSetPage);
	setPage(Page_Set_Two, new AMSampleSetPage);
    setPage(Page_Option, new AMWizardOptionPage);
	setPage(Page_Final, new AMWizardPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));

	/// need to disconnect and reconnect buttons to override functionality properly

    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

	connect(page(Page_Set_One), SIGNAL(slider()), this, SLOT(sliderChanged()));

    setMinimumSize(600,600);

    addOptionPage(Page_Intro);

	pointList_->clear();
	/// We don't know how many points there will be, so in addPoint, make sure point
	/// exists before initializing it.







}

AMSamplePlateWizard::~AMSamplePlateWizard()
{
}

int AMSamplePlateWizard::nextId() const
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
	case Page_Wait_One:
		return Page_Set_One;
	case Page_Set_One:
		return Page_Wait_Two;
	case Page_Wait_Two:
		return Page_Set_Two;
	case Page_Set_Two:
		return Page_Final;
	case Page_Final:
    default:
        return -1;
    }
}

void AMSamplePlateWizard::waitPage()
{
	emit moveTo(*coordinateList()->at(relativeId()-1));
}

int AMSamplePlateWizard::relativeId()
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

QString AMSamplePlateWizard::message(int type)
{
    if(type == Wizard_Title) return QString(tr("Sample Plate Wizard"));
    else if(type == Help_Title) return QString(tr("Help"));
    switch(currentId())
    {
    case Page_Intro:
        switch(type)
        {
        case Title:
            return QString(tr("Sample Plate Adjustment"));
        case Text:
            return QString(tr("Use this configuration tool to fine tune the position of the sample plate."));
        case Help:
            return QString(tr("This configuration simply adjusts the sample plate to account for how tightly it has")
                              + tr(" been placed in the sample plate holder.  It can be adjusted using a slider to move it left and right.")
                              + tr("  If the sample plate is significantly out of place, there may be some other error, which will need to be")
                              + tr(" resolved outside of this tool."));
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
	case Page_Wait_One:
	case Page_Wait_Two:
        switch(type)
        {
        case Title:
			return QString(tr("Moving to position %1")).arg(relativeId());
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
	case Page_Set_One:
	case Page_Set_Two:
        switch(type)
        {
        case Title:
            return QString(tr("Sample Plate Adjustment"));
        case Text:
			return QString(tr("Select the point corresponding to the coordinate: %1, %2, %3")).arg(coordinateX(relativeId())).arg(coordinateY(relativeId())).arg(coordinateZ(relativeId()));
        case Help:
            return QString(tr("To adjust the position of the sample plate, move the slider left and right.  If the outline cannot be closely")
                              + tr(" fit to the sample plate, it may require more detailed adjustment."));
        case Other:
        case Default:
        default:
            return QString(tr("Error message - set page - unknown message type."));
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
			return QString(tr("Final page"));
		case Text:
			return QString(tr("Final page text"));
		case Help:
			return QString(tr("Final Page Help"));
		case Other:
		case Default:
			return QString(tr("Error message - final page - unknown message type"));
		}
    }
    return QString(tr("Error message - unknown page type."));
}

void AMSamplePlateWizard::back()
{
	int id = currentId();
	switch(id)
    {
	case Page_Wait_One:
	case Page_Wait_Two:
		((AMWaitPage*)page(id))->stopTimer();
		QWizard::back();
		if(currentId() == Page_Check)
        {
            cleanupPage(Page_Check);
            initializePage(Page_Check);
        }
        break;
	case Page_Set_One:
        while(currentId() != Page_Check)
        {
            QWizard::back();
        }
        if(currentId() == Page_Check)
        {
            cleanupPage(Page_Check);
            initializePage(Page_Check);
        }
        break;
	case Page_Set_Two:
		while(currentId() != Page_Wait_One)
		{
			QWizard::back();
		}
		if(currentId() == Page_Wait_One)
		{
			initializePage(Page_Wait_One);
		}
		break;
    default:
        QWizard::back();
    }

}

void AMSamplePlateWizard::sliderChanged()
{
	emit movePlate(field("adjustmentSlider").toInt());
}

void AMSamplePlateWizard::addPoint(QPointF position)
{
	int pagePointAddedFrom = relativeId() - 1;
	QPointF* newPoint = pointList_->at(pagePointAddedFrom);
	QPointF newPosition = mapPointToVideo(position);
	*newPoint = newPosition;
	next();
}


void AMSampleWaitPage::initializePage()
{
    AMWaitPage::initializePage();
    startTimer(1000);
}


AMSampleSetPage::AMSampleSetPage(QWidget *parent)
    :AMViewPage(parent)
{
    adjustmentSlider_ = new QSlider();
    adjustmentSlider_->setOrientation(Qt::Horizontal);
    adjustmentSlider_->setMaximum(1000);
    adjustmentSlider_->setMinimum(-1000);
    adjustmentSlider_->setValue(0);
    registerField("adjustmentSlider", adjustmentSlider_);
}

void AMSampleSetPage::initializePage()
{
    AMViewPage::initializePage();
    layout()->addWidget(adjustmentSlider_);
    disconnect(adjustmentSlider_, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
    adjustmentSlider_->setValue(0);
    connect(adjustmentSlider_, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
}

void AMSampleSetPage::sliderChanged()
{
    setLabelText(message(Text));
    emit slider();
}


void AMSampleCheckPage::checkBoxChanged(bool state)
{
    setFinalPage(state);
}


double AMSamplePlateWizard::coordinateX(int id)
{
	return coordinateList()->at(id-1)->x();
}

double AMSamplePlateWizard::coordinateY(int id)
{
	return coordinateList()->at(id-1)->y();
}

double AMSamplePlateWizard::coordinateZ(int id)
{
	return coordinateList()->at(id-1)->z();
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
