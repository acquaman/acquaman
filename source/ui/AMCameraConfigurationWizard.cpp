#include "AMCameraConfigurationWizard.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QVariant>
#include <QTimer>
#include <QGraphicsView>

AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Check, new CheckPage);
    setPage(Page_Final, new FinalPage);
    setPage(Page_Select_One, new SelectPage);
    setPage(Page_Select_Two, new SelectPage);
    setPage(Page_Select_Three, new SelectPage);
    setPage(Page_Select_Four, new SelectPage);
    setPage(Page_Select_Five, new SelectPage);
    setPage(Page_Select_Six, new SelectPage);
    setPage(Page_Wait_One, new WaitPage);
    setPage(Page_Wait_Two, new WaitPage);
    setPage(Page_Wait_Three, new WaitPage);
    setPage(Page_Wait_Four, new WaitPage);
    setPage(Page_Wait_Five, new WaitPage);
    setPage(Page_Wait_Six, new WaitPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
//    setPixmap(QWizard::LogoPixmap, QPixMap());
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle("Camera Wizard");
//    connect(QWizard::button(QWizard::BackButton),SIGNAL(clicked()), this,SLOT(back()));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    backwards_ = false;
}

int AMCameraConfigurationWizard::nextId() const
{
    switch(currentId())
    {
        case Page_Intro:
            return Page_Check;
        case Page_Check:
            if(field("isConfigured").toBool())
            {
                return -1;
            }
            else
            {
                return Page_Wait_One;
            }
        case Page_Select_One:
            return Page_Wait_Two;
        case Page_Select_Two:
            return Page_Wait_Three;
        case Page_Select_Three:
            return Page_Wait_Four;
        case Page_Select_Four:
            return Page_Wait_Five;
        case Page_Select_Five:
            return Page_Wait_Six;
        case Page_Select_Six:
            return Page_Final;
        case Page_Wait_One:
            return Page_Select_One;
        case Page_Wait_Two:
            return Page_Select_Two;
        case Page_Wait_Three:
            return Page_Select_Three;
        case Page_Wait_Four:
            return Page_Select_Four;
        case Page_Wait_Five:
            return Page_Select_Five;
        case Page_Wait_Six:
            return Page_Select_Six;
        case Page_Final:
        default:
            return -1;
    }
}

#include <QDebug>
void AMCameraConfigurationWizard::back()
{
    qDebug()<<"AMCameraConfigurationWizard::back current Id() ="<<currentId();
    int id = currentId();
    switch(id)
    {
        case Page_Wait_One:
        case Page_Wait_Two:
        case Page_Wait_Three:
        case Page_Wait_Four:
        case Page_Wait_Five:
        case Page_Wait_Six:
            ((WaitPage*)page(id))->stopTimer();
            QWizard::back();
            break;
        case Page_Select_One:
            while(currentId() != Page_Check)
            {
                QWizard::back();
            }
            break;
        case Page_Select_Two:
            while(currentId() != Page_Wait_One) QWizard::back();
            if(currentId() == Page_Wait_One) initializePage(Page_Wait_One);
            break;
        case Page_Select_Three:
            while(currentId() != Page_Wait_Two) QWizard::back();
            if(currentId() == Page_Wait_Two) initializePage(Page_Wait_Two);
            break;
        case Page_Select_Four:
            while(currentId() != Page_Wait_Three) QWizard::back();
            if(currentId() == Page_Wait_Three) initializePage(Page_Wait_Three);
            break;
        case Page_Select_Five:
            while(currentId() != Page_Wait_Four) QWizard::back();
            if(currentId() == Page_Wait_Four) initializePage(Page_Wait_Four);
            break;
        case Page_Select_Six:
            while(currentId() != Page_Wait_Five) QWizard::back();
            if(currentId() == Page_Wait_Five) initializePage(Page_Wait_Five);
            break;
        default:
            QWizard::back();
    }
}

void AMCameraConfigurationWizard::showHelp()
{
    QString message;
    switch(currentId())
    {
        case Page_Intro:
            message = "Help message for the intro page";
            break;
        default:
            message = "Default help message.";
    }

    QMessageBox::information(this, "Camera Wizard Help", message);
}


IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Introduction Page");
    topLabel_ = new QLabel(tr("This is the introduction page of the camera configuration wizard."));
    topLabel_->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    setLayout(layout);
}



CheckPage::CheckPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Check page");
    topLabel_ = new QLabel(tr("This is the page where you check to see if the camera is correctly lined up."));
    topLabel_->setWordWrap(true);
    isConfigured_ = new QCheckBox("Is the camera correct?");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    layout->addWidget(isConfigured_);
    setLayout(layout);

    connect(isConfigured_, SIGNAL(clicked(bool)), this, SLOT(configuredSet(bool)));

            registerField("isConfigured", isConfigured_);
}

void CheckPage::configuredSet(bool set)
{
    setFinalPage(set);
}



FinalPage::FinalPage(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle("Final Page");
    topLabel_ = new QLabel(tr("This is the final page of the wizard."));
    topLabel_->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    setLayout(layout);
}


SelectPage::SelectPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Selection Page");
    topLabel_ = new QLabel(tr("This is the selection page number "));
    topLabel_->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    setLayout(layout);
}

void SelectPage::initializePage()
{
    int currentId = wizard()->currentId();
    int relativeId = 0;
    switch(currentId)
    {
        case AMCameraConfigurationWizard::Page_Select_One:
            relativeId = 1;
            break;
        case AMCameraConfigurationWizard::Page_Select_Two:
            relativeId = 2;
            break;
        case AMCameraConfigurationWizard::Page_Select_Three:
            relativeId = 3;
            break;
        case AMCameraConfigurationWizard::Page_Select_Four:
            relativeId = 4;
            break;
        case AMCameraConfigurationWizard::Page_Select_Five:
            relativeId = 5;
            break;
        case AMCameraConfigurationWizard::Page_Select_Six:
            relativeId = 6;
            break;
        default:
            relativeId = 0;
    }

    setTitle("Selection Page " + QString::number(relativeId));
}


WaitPage::WaitPage(QWidget *parent)
    : QWizardPage(parent)
{

    setTitle("Wait Page");
    topLabel_ = new QLabel(tr("Please wait until the next page appears."));
    topLabel_->setWordWrap(true);
    waitTimer_ = new QTimer();





    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    setLayout(layout);

    connect(waitTimer_, SIGNAL(timeout()), this, SLOT(nextPage()));
}

void WaitPage::initializePage()
{

    waitTimer_->start(1000);
    int wizardId = wizard()->currentId();
    int relativeId = 0;
    switch(wizardId)
    {
    case AMCameraConfigurationWizard::Page_Wait_One:
        relativeId = 1;
        break;
    case AMCameraConfigurationWizard::Page_Wait_Two:
        relativeId = 2;
        break;
    case AMCameraConfigurationWizard::Page_Wait_Three:
        relativeId = 3;
        break;
    case AMCameraConfigurationWizard::Page_Wait_Four:
        relativeId = 4;
        break;
    case AMCameraConfigurationWizard::Page_Wait_Five:
        relativeId = 5;
        break;
    case AMCameraConfigurationWizard::Page_Wait_Six:
        relativeId = 6;
        break;
    default:
        relativeId = 0;
        break;
    }

    QString title = QString("Moving to position %1").arg(relativeId);
    setTitle(title);
}

bool WaitPage::isComplete() const
{
    return false;
}

void WaitPage::stopTimer()
{
    waitTimer_->stop();
}

void WaitPage::nextPage()
{
    waitTimer_->stop();
    wizard()->next();
}

