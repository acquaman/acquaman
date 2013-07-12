#include "AMCameraConfigurationWizard.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>

AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Check, new CheckPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
//    setPixmap(QWizard::LogoPixmap, QPixMap());
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle("Camera Wizard");
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

int IntroPage::nextId() const
{
    return AMCameraConfigurationWizard::Page_Check;
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
}

int CheckPage::nextId() const
{
    if(isConfigured_->isChecked())
    {
        return -1;
    }
    else
    {
        return AMCameraConfigurationWizard::Page_Check;
    }
}
