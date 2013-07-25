#include "AMCameraConfigurationWizard.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QVariant>
#include <QTimer>
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include "AMShapeDataSetGraphicsView.h"
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QTextDocument>
#include <QScrollBar>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QVector3D>
#include "AMGraphicsVideoSceneCopier.h"
#include <QTimerEvent>
#include "AMGraphicsViewWizard.h"


AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : AMGraphicsViewWizard(parent)
{
    numberOfPoints_ = 6;
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
    setWindowTitle(message(Wizard_Title));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
//    connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));

    pointList_ = new QList<QPointF*>();
    coordinateList_ = new QList<QVector3D*>();

    QSize maxSize(0,0);
    QList<int> pageNumbers = pageIds();
    foreach(int pageNumber, pageNumbers)
    {
        page(pageNumber)->adjustSize();
        if(page(pageNumber)->size().width() > maxSize.width() || page(pageNumber)->height() > maxSize.height())
            maxSize = page(pageNumber)->size();
    }

    setMinimumSize(600,600);

    pointList_->clear();
    for(int i = 0; i < numberOfPoints_; i++)
    {
        pointList_->append(new QPointF(0,0));
    }
    /// set the coordinates
    coordinateList_->append(new  QVector3D(0,0,0));
    coordinateList_->append(new  QVector3D(0,0,12));
    coordinateList_->append(new  QVector3D(5,0,5));
    coordinateList_->append(new  QVector3D(0,5,0));
    coordinateList_->append(new  QVector3D(12,0,0));
    coordinateList_->append(new  QVector3D(12,5,0));


}

AMCameraConfigurationWizard::~AMCameraConfigurationWizard()
{

    pointList_->clear();
    delete pointList_;

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

void AMCameraConfigurationWizard::addPoint(QPointF position)
{
    qDebug()<<"AMCameraConfigurationWizard::addPoint - adding point from page"<<currentId();
    QPointF* newPoint;
    int index;
    switch(currentId())
    {
    case Page_Select_One:
        index = 0;
        break;
    case Page_Select_Two:
        index = 1;
        break;
    case Page_Select_Three:
        index = 2;
        break;
    case Page_Select_Four:
        index = 3;
        break;
    case Page_Select_Five:
        index = 4;
        break;
    case Page_Select_Six:
        index = 5;
        break;
    default:
        index = -1;
        break;
    }

    if(field("isConfigured").toBool())
    {
        for(int i = 0; i < pointList_->count(); i++)
        {
            qDebug()<<"Deleting point"<<i;
            newPoint = pointList_->at(i);
            *newPoint = QPointF(0,0);
        }
        return;
    }
    else if(index < 0)return;


    newPoint = pointList_->at(index);
    QPointF newPosition = mapPointToVideo(position);
    *newPoint = newPosition;

    foreach(QPointF* point, *pointList_)
    {
        qDebug()<<*point;
    }

    next();

}

QList<QPointF *> *AMCameraConfigurationWizard::pointList()
{
    return pointList_;
}

QList<QVector3D *> *AMCameraConfigurationWizard::coordinateList()
{
    return coordinateList_;
}

QString AMCameraConfigurationWizard::message(int messageType)
{
    if(messageType == Wizard_Title)
        return QString("Camera Wizard");
    else if(messageType == Help_Title)
        return QString("Camera Wizard Help");
    switch(currentId())
    {
    case Page_Intro:
        switch(messageType)
        {
        case Title:
            return QString("Introduction Page");
        case Text:
            return QString(tr("This is the introduction page of the camera configuration wizard."));
        case Help:
            return QString("Help message for the intro page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Check:
        switch(messageType)
        {
        case Title:
            return QString("Check page");
        case Text:
             return QString(tr("This is the page where you check to see if the camera is correctly lined up."));
        case Help:
             return QString("Help message for check page.");
        case Other:
            return QString("Is the camera correct?");
        case Default:
            return QString(tr("< &Intro"));
        default:
            break;
        }
        break;
    case Page_Select_One:
    case Page_Select_Two:
    case Page_Select_Three:
    case Page_Select_Four:
    case Page_Select_Five:
    case Page_Select_Six:
        switch(messageType)
        {
        case Title:
            return QString("Selection Page ");
        case Text:
            return QString(tr("Select the point corresponding to the coordinate: "));
        case Help:
            return QString("Help message for selection page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Wait_One:
    case Page_Wait_Two:
    case Page_Wait_Three:
    case Page_Wait_Four:
    case Page_Wait_Five:
    case Page_Wait_Six:
        switch(messageType)
        {
        case Title:
            return QString("Moving to position ");
        case Text:
             return QString(tr("Please wait until the next page appears."));
        case Help:
            return QString("Help message for wait page");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    case Page_Final:
        switch(messageType)
        {
        case Title:
            return QString("Final Page");
        case Text:
            return QString("This is the final page of the wizard.");
        case Help:
            return QString("Help message for final page.");
        case Other:
        case Default:
        default:
            break;
        }
        break;
    }
    switch(messageType)
    {
    case Title:
        return "Default Title";
    case Text:
         return QString("Default message.");
    case Help:
        return QString("Default help message.");
    case Other:
    case Default:
    default:
        return "";
    }







}




void AMCameraConfigurationWizard::back()
{
    int id = currentId();
    switch(id)
    {
        case Page_Wait_One:
            ((WaitPage*)page(id))->stopTimer();
            QWizard::back();
            cleanupPage(Page_Check);
            initializePage(Page_Check);
            break;
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
            if(currentId() == Page_Check)
            {
                cleanupPage(Page_Check);
                initializePage(Page_Check);
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




IntroPage::IntroPage(QWidget *parent)
    : AMWizardPage(parent)
{
}

void IntroPage::initializePage()
{
    setTitle(message(Title));
    setLabelText(message(Text));
    startTimer(0);

}

void IntroPage::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    viewWizard()->next();
}


CheckPage::CheckPage(QWidget *parent)
    : AMViewPage(parent)
{
    isConfigured_ = new QCheckBox();
    isConfigured_->setChecked(true);
    QVBoxLayout* layout = (QVBoxLayout*)(this->layout());
    layout->addWidget(isConfigured_);
    setLayout(layout);

    connect(isConfigured_, SIGNAL(clicked(bool)), this, SLOT(configuredSet(bool)));

    registerField("isConfigured", isConfigured_);
}

void CheckPage::initializePage()
{

    setTitle(message(Title));
    setLabelText(message(Text));
    isConfigured_->setText(message(Other));
    setButtonText(QWizard::BackButton,message(Default));
    AMViewPage::initializePage();

}

void CheckPage::configuredSet(bool set)
{
    setFinalPage(set);
    if(set)
    {
        viewWizard()->addPoint(QPointF(0,0));
    }
}




FinalPage::FinalPage(QWidget *parent)
    :AMWizardPage(parent)
{
}

void FinalPage::initializePage()
{
    setTitle(message(Title));
    setLabelText(message(Text));
}




SelectPage::SelectPage(QWidget *parent)
    : AMViewPage(parent)
{

}

void SelectPage::initializePage()
{
    setTitle(message(Title));
    setLabelText(message(Text));
    int currentId = viewWizard()->currentId();
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

    setTitle(message(Title) + QString::number(relativeId));

    QVector3D coordinate = *(((AMCameraConfigurationWizard*)viewWizard())->coordinateList()->at(relativeId-1));

    topLabel_->setText(message(Text) + QString("%1,%2,%3").arg(coordinate.x()).arg(coordinate.y()).arg(coordinate.z()));

    AMViewPage::initializePage();


    connect(view(), SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));
}

void SelectPage::addPoint(QPointF position)
{
    ((AMCameraConfigurationWizard*)viewWizard())->addPoint(position);
}




WaitPage::WaitPage(QWidget *parent)
    : AMWaitPage(parent)
{

}

void WaitPage::initializePage()
{

    setTitle(message(Title));
    setLabelText(message(Text));
    AMWaitPage::startTimer(1000);
    int wizardId = viewWizard()->currentId();
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

    QString title = message(Title) + QString("%1").arg(relativeId);
    setTitle(title);
}







