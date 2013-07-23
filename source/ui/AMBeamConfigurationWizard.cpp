#include "AMBeamConfigurationWizard.h"
#include <QWizard>
#include <QList>
#include "AMGraphicsViewWizard.h"
#include <QLabel>
#include <QAbstractButton>
#include <QLayout>
#include <QDebug>
#include <QCheckBox>



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
    setWindowTitle(message(Title_Wizard));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));
    disconnect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));
    connect(button(QWizard::NextButton), SIGNAL(clicked()), this, SLOT(next()));

    setMinimumSize(600,600);

    setting_ = false;
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
        if(setting_) return Page_Set_One;
        else return Page_Check_One;
    case Page_Wait_Two:
        if(setting_) return Page_Set_Two;
        else return Page_Check_Two;
    case Page_Wait_Three:
        if(setting_) return Page_Set_Three;
        else return Page_Check_Three;
    case Page_Set_One:
        qDebug()<<"Current page is page_set_one";
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
        qDebug()<<"NextId is Wait_Two_Again";
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
    else AMGraphicsViewWizard::next();
}

void AMBeamConfigurationWizard::showHelp()
{
}


QString AMBeamConfigurationWizard::message(AMBeamConfigurationWizard::WizardMessage type)
{
    switch(type)
    {
    case Title_Wizard:
        return "Beam Configuration Wizard";
    default:
        return "Default Message";
    }
}

void AMBeamConfigurationWizard::addPoint(QPointF position)
{
    QPointF* newPoint = new QPointF();
    *newPoint = position;
    pointList_->append(newPoint);
}

QList<QVector3D *> *AMBeamConfigurationWizard::coordinateList()
{
    return coordinateList_;
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
    setTitle("Intro");
    setLabelText("Intro page label text.");

}

AMBeamCheckPage::AMBeamCheckPage(QWidget *parent)
    : AMViewPage(parent)
{
    setTitle("Check Page");
    setLabelText("Check Page label text.");
    beamConfigured_ = new QCheckBox("Is the beam in the correct spot?");
    beamConfigured_->setChecked(true);
    layout()->addWidget(beamConfigured_);

    connect(beamConfigured_, SIGNAL(clicked(bool)), this, SLOT(configuredChanged(bool)));

}

void AMBeamCheckPage::initializePage()
{
    int currentId = viewWizard()->currentId();
    int relativeId = 0;
    switch(currentId)
    {
    case AMBeamConfigurationWizard::Page_Check_One:
        relativeId = 1;
        break;
    case AMBeamConfigurationWizard::Page_Check_Two:
        relativeId = 2;
        break;
    case AMBeamConfigurationWizard::Page_Check_Three:
        relativeId = 3;
        break;
    default:
        relativeId = 0;
    }
    setTitle(title() + QString("%1").arg(relativeId));
    setLabelText(QString("This is check page %1").arg(relativeId));

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
    setTitle("Wait Page");
    setLabelText("Wait page label text");
}

void AMBeamWaitPage::initializePage()
{
    int relativeId;
    switch(viewWizard()->currentId())
    {
    case AMBeamConfigurationWizard::Page_Wait_One:
        relativeId = 1;
        break;
    case AMBeamConfigurationWizard::Page_Wait_Two:
        relativeId = 2;
        break;
    case AMBeamConfigurationWizard::Page_Wait_Three:
        relativeId = 3;
        break;
    default:
        relativeId = 0;
    }
    setLabelText(QString("Moving to position %1").arg(relativeId));

    AMWaitPage::startTimer(1000);
}


AMBeamSelectPage::AMBeamSelectPage(QWidget *parent)
    : AMViewPage(parent)
{
    setTitle("Select Page");
    setLabelText("Selection Page");
}

void AMBeamSelectPage::initializePage()
{
    qDebug()<<"Initializing beam set page"<<viewWizard()->currentId();
    int relativeId;
    switch(viewWizard()->currentId())
    {
    case AMBeamConfigurationWizard::Page_Set_One:
        relativeId = 1;
        break;
    case AMBeamConfigurationWizard::Page_Set_Two:
        relativeId = 2;
        break;
    case AMBeamConfigurationWizard::Page_Set_Three:
        relativeId = 3;
        break;
    default:
        relativeId = 0;
    }

    setLabelText(QString("This is selection page %1").arg(relativeId));

    AMViewPage::initializePage();
}


AMBeamFinalPage::AMBeamFinalPage(QWidget *parent)
    :AMWizardPage(parent)
{
    setTitle("Final Page");
    setLabelText("Final Page label Text.");
}

void AMBeamFinalPage::initializePage()
{
    setLabelText(QString("Check Page %1 has value: %2 \n").arg(AMBeamConfigurationWizard::Page_Check_One).arg(field(QString("Configured %1").arg(AMBeamConfigurationWizard::Page_Check_One)).toBool())
    + QString("Check Page %1 has value: %2 \n").arg(AMBeamConfigurationWizard::Page_Check_Two).arg(field(QString("Configured %1").arg(AMBeamConfigurationWizard::Page_Check_Two)).toBool())
    + QString("Check Page %1 has value: %2").arg(AMBeamConfigurationWizard::Page_Check_Three).arg(field(QString("Configured %1").arg(AMBeamConfigurationWizard::Page_Check_Three)).toBool()));
}
