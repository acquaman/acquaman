#include "AMGraphicsViewWizard.h"

#include <QPointF>
#include "AMGraphicsVideoSceneCopier.h"
#include "AMShapeDataSetGraphicsView.h"
#include <QLayout>

#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <QVector3D>

AMGraphicsViewWizard::AMGraphicsViewWizard(QWidget* parent)
    :QWizard(parent)
{
    view_ = new AMShapeDataSetGraphicsView();
    scale_ = new QPointF(1,1);
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

void AMGraphicsViewWizard::setView(AMShapeDataSetGraphicsView *view)
{
    view_ = view;
    view_->setObjectName("AMGraphicsViewWizard view 1");

    AMGraphicsVideoSceneCopier* copier = new AMGraphicsVideoSceneCopier();
    copier->cloneScene(view->scene());
    view_->setScene(copier->scene());
    view_->scene()->setObjectName("AMGraphicsViewWizard scene 1");

    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
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



