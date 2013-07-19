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


AMCameraConfigurationWizard::AMCameraConfigurationWizard(QWidget* parent)
    : QWizard(parent)
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
    setWindowTitle("Camera Wizard");
//    connect(QWizard::button(QWizard::BackButton),SIGNAL(clicked()), this,SLOT(back()));
    disconnect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::BackButton), SIGNAL(clicked()), this, SLOT(back()));
    connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SLOT(allDone()));
    backwards_ = false;
    scale_ = QPointF(1,1);
    view_ = new AMShapeDataSetGraphicsView(0);

    pointList_ = new QList<QPointF*>();
    coordinateList_ = new QList<QVector3D*>();

//    videoItem_ = 0;
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
//        coordinateList_->append(new QVector3D(0,0,0));
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

    delete view_;
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

AMShapeDataSetGraphicsView *AMCameraConfigurationWizard::view()
{
    return view_;
}

QPointF AMCameraConfigurationWizard::scale()
{
    return scale_;
}

void AMCameraConfigurationWizard::setScale(QPointF scale)
{
    scale_ = scale;
    view_->scale(scale_.x(),scale_.y());
}

void AMCameraConfigurationWizard::setScale(double scaleFactor)
{
    setScale(QPointF(1/scale().x(),1/scale().y()));

    QPointF scalePoint(scaleFactor,scaleFactor);
    setScale(scalePoint);
}

void AMCameraConfigurationWizard::addPoint(QPointF position)
{
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
    if(index < 0 )
        return;


    QList<QGraphicsItem*> list = view_->items();
    QGraphicsVideoItem* videoItem;
    foreach(QGraphicsItem* item, list)
    {
        if(item->type() > 12)
        {
            videoItem = (QGraphicsVideoItem*)item;
        }

    }

    newPoint = pointList_->at(index);
    QPointF topLeft = videoItem->sceneBoundingRect().topLeft();
    QPointF bottomRight = videoItem->sceneBoundingRect().bottomRight();
    double left = view_->mapSceneToVideo(topLeft).x();
    double right = view_->mapSceneToVideo(bottomRight).x();
    double top = view_->mapSceneToVideo(topLeft).y();
    double bottom = view_->mapSceneToVideo(bottomRight).y();
    double positionX = position.x();//right;
    positionX = (positionX - left);
    positionX = positionX/(right-left);
    double positionY= position.y();//view_->mapSceneToVideo(bottomRight).y();
    positionY = positionY - top;
    positionY = positionY/(bottom-top);
    QPointF newPosition(positionX,positionY);
    *newPoint = newPosition;

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




void AMCameraConfigurationWizard::back()
{
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

void AMCameraConfigurationWizard::setView(AMShapeDataSetGraphicsView* view)
{
    view_ = view;

    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->cloneScene(view->scene());
    view_->setScene(copier->scene());

    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

void AMCameraConfigurationWizard::allDone()
{

    emit done();
    emit customButtonClicked(0);
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

    checkView_ = new AMShapeDataSetGraphicsView(0);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    layout->addWidget(isConfigured_);
    setLayout(layout);





    connect(isConfigured_, SIGNAL(clicked(bool)), this, SLOT(configuredSet(bool)));

    registerField("isConfigured", isConfigured_);
}

void CheckPage::setCheckView(AMShapeDataSetGraphicsView *view)
{
    checkView_ = view;
}

void CheckPage::initializePage()
{
    setCheckView(((AMCameraConfigurationWizard*)wizard())->view());

    QFrame* viewFrame = new QFrame();
    QHBoxLayout* viewLayout = new QHBoxLayout();
    viewLayout->addStretch();
    viewLayout->addWidget(checkView_);
    viewLayout->addStretch();
    viewFrame->setLayout(viewLayout);


    layout()->addWidget(viewFrame);

    double scaleFactor = 2;
//    ((AMCameraConfigurationWizard*)wizard())->setScale(scaleFactor);


}

void CheckPage::cleanupPage()
{
    QLayout* oldLayout = (this->layout());
    delete oldLayout;
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    layout->addWidget(isConfigured_);
    layout->addStretch();
    setLayout(layout);
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

void SelectPage::setSelectionView(AMShapeDataSetGraphicsView *view)
{
    view_ = view;
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

    topLabel_->setText(topLabel_->text()+ QString::number(relativeId));

    setSelectionView(((AMCameraConfigurationWizard*)wizard())->view());

    QFrame* viewFrame = new QFrame();
    QHBoxLayout* viewLayout = new QHBoxLayout();
    viewLayout->addStretch();
    viewLayout->addWidget(view_);
    viewLayout->addStretch();
    viewFrame->setLayout(viewLayout);

    layout()->addWidget(viewFrame);
    double scaleFactor = 1;
    ((AMCameraConfigurationWizard*)wizard())->setScale(scaleFactor);

    connect(view_, SIGNAL(mousePressed(QPointF)), this, SLOT(addPoint(QPointF)));



}



void SelectPage::cleanupPage()
{
    QLayout* oldLayout = (this->layout());
    delete oldLayout;
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(topLabel_);
    setLayout(layout);
    topLabel_->setText("This is the selection page number ");
}

void SelectPage::setView(AMShapeDataSetGraphicsView *view)
{
    view_ = view;
}

void SelectPage::addPoint(QPointF position)
{
    ((AMCameraConfigurationWizard*)wizard())->addPoint(position);
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



