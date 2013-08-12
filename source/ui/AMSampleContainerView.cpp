#include "AMSampleContainerView.h"

#include "AMSampleContainer.h"
#include "AMSampleEthanView.h"
#include <QDebug>
AMSampleContainerView::AMSampleContainerView(QWidget *parent)
    :QListView(parent)
{
    setWindowTitle("Sample Container");
}

AMSampleContainerView::AMSampleContainerView(AMSampleContainer *sampleContainer, QWidget *parent)
    :QListView(parent)
{
    init();
    setSampleContainer(sampleContainer);
}

void AMSampleContainerView::setSampleContainer(AMSampleContainer *sampleContainer)
{
    init();
    sampleContainer_ = sampleContainer;
    setModel(sampleContainer_);
}

void AMSampleContainerView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    sampleContainer_->setIndex(current.row());
    sampleView_->setSample(sampleContainer_->sample(sampleContainer_->index()));
    sampleView_->show();
    for(int i = 0; i < sampleContainer_->count(); i++)
    {
        qDebug()<<sampleContainer_->sampleName(i);
    }

    this->update(QModelIndex());
}

void AMSampleContainerView::init()
{
    setWindowTitle("Sample Container");
    sampleView_ = new AMSampleEthanView();
}

