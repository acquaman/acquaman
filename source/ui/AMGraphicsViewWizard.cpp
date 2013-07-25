#include "AMGraphicsViewWizard.h"

#include <QPointF>
#include "AMGraphicsVideoSceneCopier.h"
#include "AMShapeDataSetGraphicsView.h"
#include <QLayout>

#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <QVector3D>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsVideoItem>
#include <QScrollBar>
#include <QAbstractButton>

AMGraphicsViewWizard::AMGraphicsViewWizard(QWidget* parent)
    :QWizard(parent)
{
    view_ = new AMShapeDataSetGraphicsView();
    scale_ = new QPointF(1,1);
    connect(QWizard::button(QWizard::FinishButton), SIGNAL(clicked()), this, SIGNAL(done()));
}

AMShapeDataSetGraphicsView *AMGraphicsViewWizard::view()
{
    return view_;
}

QPointF AMGraphicsViewWizard::scale()
{
    return *scale_;
}

void AMGraphicsViewWizard::setScale(QPointF scale)
{
    view_->scale(1/scale_->x(), 1/scale_->y());
    *scale_ = scale;
    view_->scale(scale_->x(),scale_->y());
}

void AMGraphicsViewWizard::setScale(double scaleFactor)
{
    QPointF scalePoint(scaleFactor,scaleFactor);
    setScale(scalePoint);
}


QString AMGraphicsViewWizard::message(int type)
{
    if(type)
    {
        return "Default message";
    }
    else
    {
        return "Default Error message";
    }
}

QPointF AMGraphicsViewWizard::mapPointToVideo(QPointF point)
{
    QList<QGraphicsItem*> list = view()->items();
    QGraphicsVideoItem* videoItem;
    foreach(QGraphicsItem* item, list)
    {
        if(item->type() == QGraphicsItem::UserType)
        {
            videoItem = (QGraphicsVideoItem*)item;
        }
    }
    QPointF topLeft = videoItem->sceneBoundingRect().topLeft();
    QPointF bottomRight = videoItem->sceneBoundingRect().bottomRight();
    QPointF sceneTopLeft = view()->mapSceneToVideo(topLeft);
    QPointF sceneBottomRight = view()->mapSceneToVideo(bottomRight);
    double left = sceneTopLeft.x();
    double right = sceneBottomRight.x();
    double top = sceneTopLeft.y();
    double bottom = sceneBottomRight.y();

    double positionX = (point.x() - left)/(right-left);
    double positionY = (point.y() - top)/(bottom - top);

    return QPointF(positionX, positionY);
}

void AMGraphicsViewWizard::setView(AMShapeDataSetGraphicsView *view)
{
    view_ = view;
    view_->setObjectName("AMGraphicsViewWizard view 1");

    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->cloneScene(view->scene());
    view_->setScene(copier->scene());
    view_->scene()->setObjectName("AMGraphicsViewWizard scene 1");

    /// This item prevents the screen from flashing or going blank
    /// when there is a QGraphicsItem or a descendent in less than
    /// all the views, but at least one.
    /// It seems that placing a QGraphicsTextItem with it's bounding rectangle
    /// touching the current views boundaries (adjusted for scrolling) will
    /// stop the behaviour.
    QGraphicsTextItem* textFixItem = new QGraphicsTextItem("Fix");
    textFixItem->setObjectName("Fix");
    textFixItem->setZValue(INT_MAX);
    textFixItem->setPos(-1*textFixItem->boundingRect().width(), -1*textFixItem->boundingRect().height());
    view_->scene()->addItem(textFixItem);


    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    fixText();


}

void AMGraphicsViewWizard::showHelp()
{
    QMessageBox::information(this, message(Help_Title), message(Help));
}

void AMGraphicsViewWizard::updateScene(AMShapeDataSetGraphicsView *view)
{
    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->updateScene(view_->scene(),view->scene());

    /// re add the "Fix" item (see setView)
    QGraphicsTextItem* textFixItem = new QGraphicsTextItem("Fix");
    textFixItem->setObjectName("Fix");
    textFixItem->setZValue(INT_MAX);
    textFixItem->setPos(-1*textFixItem->boundingRect().width(), -1*textFixItem->boundingRect().height());
    view_->scene()->addItem(textFixItem);

}

void AMGraphicsViewWizard::outputItems()
{
    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->cloneScene(view_->scene());
}

void AMGraphicsViewWizard::fixText()
{
    QList<QGraphicsItem*> list = this->view()->items();
    QGraphicsTextItem* fixTextItem;
    foreach(QGraphicsItem* item, list)
    {

        if(item->type() == QGraphicsTextItem::Type)
        {
            QGraphicsTextItem* textItem = (QGraphicsTextItem*)(item);
            if(textItem->objectName() == "Fix")
            {
                fixTextItem = textItem;
            }
        }
    }


    QRect visibleRect = view_->viewport()->rect();

    visibleRect.moveTopLeft((QPoint(view_->horizontalScrollBar()->value(),
    view_->verticalScrollBar()->value())));

    QPointF fixLocation = visibleRect.topLeft() - (fixTextItem->boundingRect().bottomRight());
    fixTextItem->setPos(fixLocation);

}

AMWizardPage::AMWizardPage(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle("AMWizardPage Title");
    QVBoxLayout* selectLayout = new QVBoxLayout();
    selectLayout->setContentsMargins(0,0,0,0);
    selectLayout->addWidget(topLabel_ = new QLabel("AMWizardPage Label"));
    selectLayout->addStretch();
    setLayout(selectLayout);
    topLabel_->setWordWrap(true);
}

AMGraphicsViewWizard *AMWizardPage::viewWizard() const
{
    return (AMGraphicsViewWizard*)wizard();
}

QString AMWizardPage::message(int type)
{
    int messageType;
    switch(type)
    {
    case Title:
        messageType = AMGraphicsViewWizard::Title;
        break;
    case Text:
        messageType = AMGraphicsViewWizard::Text;
        break;
    case Other:
        messageType = AMGraphicsViewWizard::Other;
        break;
    case Help:
        messageType = AMGraphicsViewWizard::Help;
        break;
    default:
        messageType = AMGraphicsViewWizard::Default;
        break;
    }
    return viewWizard()->message(messageType);
}

void AMWizardPage::setLabelText(QString text)
{
    topLabel_->setText(text);
}


AMWaitPage::AMWaitPage(QWidget *parent)
    :AMWizardPage(parent)
{
    waitTimer_ = new QTimer();
    connect(waitTimer_, SIGNAL(timeout()), this, SLOT(nextPage()));
}

bool AMWaitPage::isComplete() const
{
    return false;
}

void AMWaitPage::stopTimer()
{
    waitTimer_->stop();
}

void AMWaitPage::startTimer(int msec)
{
    waitTimer_->start(msec);
}

void AMWaitPage::nextPage()
{
    waitTimer_->stop();
    wizard()->next();
}


AMViewPage::AMViewPage(QWidget *parent)
    : AMWizardPage(parent)
{
    viewFrame_ = new QFrame();
    view_ = new AMShapeDataSetGraphicsView();

}

AMShapeDataSetGraphicsView *AMViewPage::view()
{
    return view_;
}

void AMViewPage::setView(AMShapeDataSetGraphicsView *view)
{
    view_ = view;
}


void AMViewPage::initializePage()
{
    if(true)
    {
        setView(((AMGraphicsViewWizard*)wizard())->view());
    }
    addView();
    viewWizard()->fixText();
    connect(view_->verticalScrollBar(), SIGNAL(valueChanged(int)), viewWizard(), SLOT(fixText()));
    connect(view_->horizontalScrollBar(), SIGNAL(valueChanged(int)), viewWizard(), SLOT(fixText()));
}

void AMViewPage::cleanupPage()
{
    layout()->removeWidget(viewFrame_);
}


void AMViewPage::addView()
{
    viewFrame_ = new QFrame();
    QHBoxLayout* viewLayout = new QHBoxLayout();
    viewLayout->addStretch();
    viewLayout->addWidget(view_);
    viewLayout->addStretch();
    viewFrame_->setLayout(viewLayout);
    layout()->addWidget(viewFrame_);
}



