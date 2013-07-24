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
#include "AMShapeDataSetGraphicsView.h"



AMBeamConfigurationWizard::AMBeamConfigurationWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    setPage(Page_Intro, new AMBeamIntroPage);
    setPage(Page_Check_One, new AMBeamCheckPage);
    setPage(Page_Check_Two, new AMBeamCheckPage);
    setPage(Page_Check_Three, new AMBeamCheckPage);
    setPage(Page_Wait_One, new AMBeamWaitPage);
    setPage(Page_Wait_Two, new AMBeamWaitPage);
    setPage(Page_Wait_Three, new AMBeamWaitPage);
    setPage(Page_Set_One, new AMBeamSelectPage);
    setPage(Page_Set_Two, new AMBeamSelectPage);
    setPage(Page_Set_Three, new AMBeamSelectPage);
    setPage(Page_Final, new AMBeamFinalPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));
    disconnect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));
    connect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));

    setMinimumSize(600,600);

    setting_ = false;

    /// two points for each square, three squares.
    numberOfPoints_ = 6;

    pointList_ = new QList<QPointF*>();

    for(int i = 0; i < numberOfPoints_; i++)
    {
        pointList_->append(new QPointF(0,0));
    }

    topLeft_ = true;

}

AMBeamConfigurationWizard::~AMBeamConfigurationWizard()
{
}

int AMBeamConfigurationWizard::nextId() const
{
    switch(currentId())
    {
    case Page_Intro:
        return Page_Wait_One;
    case Page_Check_One:
        return Page_Wait_Two;
    case Page_Check_Two:
        return Page_Wait_Three;
    case Page_Check_Three:
        if(field(QString("Configured %1").arg(Page_Check_One)).toBool() && field(QString("Configured %1").arg(Page_Check_Two)).toBool() && field(QString("Configured %1").arg(Page_Check_Three)).toBool())
            return Page_Final;
        else
            return Wait_One_Again;
    case Page_Wait_One:
//        return Page_Intro;
        if(setting_) return Page_Set_One;
        else return Page_Check_One;
    case Page_Wait_Two:
        if(setting_) return Page_Set_Two;
        else return Page_Check_Two;
    case Page_Wait_Three:
        if(setting_) return Page_Set_Three;
        else return Page_Check_Three;
    case Page_Set_One:
        return Wait_Two_Again;
    case Page_Set_Two:
        return Wait_Three_Again;
    case Page_Set_Three:
        return Page_Final;
    default:
        return -1;
    }
}

void AMBeamConfigurationWizard::next()
{
    if(nextId() == Wait_One_Again)
    {
        setting_ = true;
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
    }
    else if (nextId() == Wait_Two_Again)
    {
        setting_ = false;
        QWizard::back();
        while(currentId() != Page_Wait_Two)
            QWizard::next();
        if(currentId() == Page_Wait_Two)
        {
            setting_ = true;
            initializePage(Page_Wait_Two);
        }
    }
    else if(nextId() == Wait_Three_Again)
    {
        setting_ = false;
         QWizard::back();
        while(currentId() != Page_Wait_Three)
            QWizard::next();
        if(currentId() == Page_Wait_Three)
        {
            setting_ = true;
            initializePage(Page_Wait_Three);
        }
    }
    else if((currentId() == Page_Check_One && !(field(QString("Configured %1").arg(Page_Check_One)).toBool()))
            || (currentId() == Page_Check_Two && !(field(QString("Configured %1").arg(Page_Check_Two)).toBool())))
    {
        while(currentId() != Page_Check_Three)
        {
            QWizard::next();
        }
        next();
    }
    else AMGraphicsViewWizard::next();
}

void AMBeamConfigurationWizard::showHelp()
{
    QString helpMessage = message(Help);

    QMessageBox::information(this,message(Help_Title), helpMessage);
}


QString AMBeamConfigurationWizard::message(int type)
{
    if(type == Wizard_Title) return "Beam Configuration Wizard";
    else if(type == Help_Title) return "Help";

    switch(currentId())
    {
    case Page_Intro:
        switch(type)
        {
        case Title:
            return "Introduction Page";
        case Text:
            return "Introduction page text.";
        case Help:
            return "Intro page help";
        case Other:
        case Default:
        default:
            break;
        }
        break;

    case Page_Check_One:
    case Page_Check_Two:
    case Page_Check_Three:
        switch(type)
        {
        case Title:
            return "Confirm Beam";
        case Text:
            return QString("Check to see if beam %1 is in the correct configuration.").arg(relativeId());
        case Help:
            return "Check Page help";
        case Other:
            return "Is the beam correct?";
        case Default:
        default:
            break;
        }
        break;
    case Page_Set_One:
    case Page_Set_Two:
     case Page_Set_Three:
        switch(type)
        {
        case Title:
            return "Set Beam";
        case Text:
            return QString("Draw a box over beam position %1").arg(relativeId());
        case Help:
            return "set page help";
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Wait_One:
    case Page_Wait_Two:
    case Page_Wait_Three:
        switch(type)
        {
        case Title:
            return "Please Wait";
        case Text:
            return QString("Moving to position %1").arg(relativeId());
        case Help:
            return "Wait page help";
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
            return "Final page Title";
        case Text:
            return (QString("Check Page %1 has value: %2 \n").arg(Page_Check_One).arg(field(QString("Configured %1").arg(Page_Check_One)).toBool())
                    + QString("Check Page %1 has value: %2 \n").arg(Page_Check_Two).arg(field(QString("Configured %1").arg(Page_Check_Two)).toBool())
                                 + QString("Check Page %1 has value: %2").arg(Page_Check_Three).arg(field(QString("Configured %1").arg(Page_Check_Three)).toBool()));
        case Help:
            return "Final page help";
        case Other:
        case Default:
        default:
            break;
        }
    default:
        return "";
    }

    return "";
}

QList<QPointF *> *AMBeamConfigurationWizard::pointList()
{
    return pointList_;
}

void AMBeamConfigurationWizard::addPoint(QPointF position)
{
    int index = relativeId() - 1;
    QPointF* newPoint;// = new QPointF();
    if(topLeft_)
    {
        newPoint = (*pointList_)[2*(index)];
        connect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
        topLeft_ = !topLeft_;
    }
    else
    {
        newPoint = (*pointList_)[2*(index) + 1];
    }
    *newPoint = mapPointToVideo(position);

    emit showShape(index);


}

void AMBeamConfigurationWizard::endPoint(QPointF position)
{
    disconnect(view(), SIGNAL(mouseMoved(QPointF)), this, SLOT(addPoint(QPointF)));
    topLeft_ = true;
}

QList<QVector3D *> *AMBeamConfigurationWizard::coordinateList()
{
    return coordinateList_;
}

int AMBeamConfigurationWizard::relativeId()
{
    switch(currentId())
    {
    case Page_Check_One:
        return 1;
    case Page_Check_Two:
        return 2;
    case Page_Check_Three:
        return 3;
    case Page_Wait_One:
        return 1;
    case Page_Wait_Two:
        return 2;
    case Page_Wait_Three:
        return 3;
    case Page_Set_One:
        return 1;
    case Page_Set_Two:
        return 2;
    case Page_Set_Three:
        return 3;
    default:
        return 0;
    }
}



void AMBeamConfigurationWizard::back()
{
    switch(currentId())
    {
    case Page_Check_One:
        while(currentId() != Page_Intro)
            QWizard::back();
        break;
    case Page_Check_Two:
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Check_Three:
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Wait_One:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        if(setting_)
        {
            setting_ = false;
            while(currentId() != Page_Wait_Three)
                next();
            if(currentId() == Page_Wait_Three)
                initializePage(Page_Wait_Three);

        }
        else
        {
            QWizard::back();
        }
        break;
    case Page_Wait_Two:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Wait_Three:
        ((AMWaitPage*)page(currentId()))->stopTimer();
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Set_One:
        setting_ = false;
        QWizard::back();
        while(currentId() != Page_Wait_Three)
            QWizard::next();
        if(currentId() == Page_Wait_Three)
            initializePage(Page_Wait_Three);
        break;
    case Page_Set_Two:
        setting_ = true;
        while(currentId() != Page_Wait_One)
            QWizard::back();
        if(currentId() == Page_Wait_One)
            initializePage(Page_Wait_One);
        break;
    case Page_Set_Three:
        setting_ = true;
        while(currentId() != Page_Wait_Two)
            QWizard::back();
        if(currentId() == Page_Wait_Two)
            initializePage(Page_Wait_Two);
        break;
    case Page_Final:
        while(currentId() != Page_Wait_Three)
            QWizard::back();
        if(currentId() == Page_Wait_Three)
            initializePage(Page_Wait_Three);
        break;
    default:
        AMGraphicsViewWizard::back();
        break;
    }

}

AMBeamIntroPage::AMBeamIntroPage(QWidget *parent)
    : AMWizardPage(parent)
{
}

void AMBeamIntroPage::initializePage()
{
    setTitle(message(Title));
    setLabelText(message(Text));

}



AMBeamCheckPage::AMBeamCheckPage(QWidget *parent)
    : AMViewPage(parent)
{
    setTitle("Check Page");
    setLabelText("Check Page label text.");
    beamConfigured_ = new QCheckBox();
    beamConfigured_->setChecked(true);
    layout()->addWidget(beamConfigured_);


     connect(beamConfigured_, SIGNAL(clicked(bool)), this, SLOT(configuredChanged(bool)));

}

void AMBeamCheckPage::initializePage()
{

    qDebug()<<"Check page, setting title";
    setTitle(message(Title));
    setLabelText(message(Text));
    beamConfigured_->setText(message(Other));

    QString fieldName = QString("Configured %1").arg(viewWizard()->currentId());
    if(field(fieldName).isNull())
        registerField(fieldName,beamConfigured_);
    AMViewPage::initializePage();
}



void AMBeamCheckPage::configuredChanged(bool configured)
{
    qDebug()<<"Configuration is"<<configured;
}


AMBeamWaitPage::AMBeamWaitPage(QWidget *parent)
    : AMWaitPage(parent)
{

}

void AMBeamWaitPage::initializePage()
{

    setTitle(message(Title));
    setLabelText(message(Text));

    AMWaitPage::startTimer(1000);
}




AMBeamSelectPage::AMBeamSelectPage(QWidget *parent)
    : AMViewPage(parent)
{

}

void AMBeamSelectPage::initializePage()
{
    qDebug()<<"Initializing beam set page"<<viewWizard()->currentId();
    setTitle(message(Title));
    setLabelText(message(Text));

    AMViewPage::initializePage();


    disconnect(view(), SIGNAL(mousePressed(QPointF)), viewWizard(), SLOT(addPoint(QPointF)));
    disconnect(view(), SIGNAL(mouseLeftReleased(QPointF)), viewWizard(), SLOT(endPoint(QPointF)));
    connect(view(), SIGNAL(mousePressed(QPointF)), viewWizard(), SLOT(addPoint(QPointF)));
    connect(view(), SIGNAL(mouseLeftReleased(QPointF)), viewWizard(), SLOT(endPoint(QPointF)));
}


AMBeamFinalPage::AMBeamFinalPage(QWidget *parent)
    :AMWizardPage(parent)
{
}

void AMBeamFinalPage::initializePage()
{
    setTitle(message(Title));
    setLabelText(message(Text));
}

