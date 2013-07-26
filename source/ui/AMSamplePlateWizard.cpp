#include "AMSamplePlateWizard.h"

#include "AMBeamConfiguration.h"
#include <QAbstractButton>
#include <QSlider>
#include <QLayout>

AMSamplePlateWizard::AMSamplePlateWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    setPage(Page_Intro, new AMWizardPage);
    setPage(Page_Check, new AMCheckPage);
    setPage(Page_Wait, new AMSampleWaitPage);
    setPage(Page_Set, new AMSampleSetPage);
    setStartId(Page_Intro);
    setOption(HaveHelpButton, true);
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(message(Wizard_Title));

    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));

    connect(page(Page_Set), SIGNAL(slider()), this, SLOT(sliderChanged()));

    setMinimumSize(600,600);

    numberOfPoints_ = 1;



}

AMSamplePlateWizard::~AMSamplePlateWizard()
{
}

int AMSamplePlateWizard::nextId() const
{
    switch(currentId())
    {
    case Page_Intro:
        return Page_Check;
    case Page_Check:
        return Page_Wait;
    case Page_Wait:
        return Page_Set;
    case Page_Set:
    default:
        return -1;
    }
}

QString AMSamplePlateWizard::message(int type)
{
    if(type == Wizard_Title) return "Sample Plate Wizard";
    else if(type == Help_Title) return "Help";
    switch(currentId())
    {
    case Page_Intro:
        switch(type)
        {
        case Title:
            return "Sample Plate Wizard Intro Page";
        case Text:
            return "Sample Plate Wizard introduction page text.";
        case Help:
            return "Sample Plate Wizard introduction page help message";
        case Other:
        case Default:
        default:
            return "intro default";
        }
    case Page_Check:
        switch(type)
        {
        case Title:
            return "Sample Plate Check Page";
        case Text:
            return "Sample Plate Check Page text";
        case Help:
            return "Sample Plate Check Page help message";
        case Other:
            return "Does the plate line up with the line?";
        case Default:
        default:
            return "check default";
        }
    case Page_Wait:
        switch(type)
        {
        case Title:
            return "Sample Plate Wait Page";
        case Text:
            return "Sample Plate Wait page text";
        case Help:
            return "Sample Plate Wait page help message";
        case Other:
        case Default:
        default:
            return "wait default";
        }
    case Page_Set:
        switch(type)
        {
        case Title:
            return "Sample Plate Set Page";
        case Text:
            return QString("Sample Plate Set Page text %1").arg(field("adjustmentSlider").toInt());
        case Help:
            return "Sample Plate Set Page help message";
        case Other:
        case Default:
        default:
            return "set default";
        }
    }
    return "";
}

void AMSamplePlateWizard::back()
{
    switch(currentId())
    {
    case Page_Wait:
        ((AMWaitPage*)page(Page_Wait))->stopTimer();
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
    default:
        QWizard::back();
    }

}

void AMSamplePlateWizard::sliderChanged()
{
//    page(currentId())->setToolTip(QString("Slider value is %1").arg(field("adjustmentSlider").toInt()));
    emit movePlate(field("adjustmentSlider").toInt());
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
    adjustmentSlider_->setValue(0);
    layout()->addWidget(adjustmentSlider_);
    disconnect(adjustmentSlider_, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
    connect(adjustmentSlider_, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
}

void AMSampleSetPage::sliderChanged()
{
    setLabelText(message(Text));
    emit slider();
}



