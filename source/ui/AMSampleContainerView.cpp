#include "AMSampleContainerView.h"

#include "AMSampleContainer.h"
#include "AMSampleEthanView.h"
#include <QDebug>
AMSampleContainerView::AMSampleContainerView(QWidget *parent)
    :QListView(parent)
{
    init();
}

AMSampleContainerView::AMSampleContainerView(AMSampleContainer *sampleContainer, QWidget *parent)
    :QListView(parent)
{
    init();
    setSampleContainer(sampleContainer);
}

void AMSampleContainerView::setSampleContainer(AMSampleContainer *sampleContainer)
{
    sampleContainer_ = sampleContainer;
    setModel(sampleContainer_);
}

void AMSampleContainerView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    sampleContainer_->setIndex(current.row());
    int index = sampleContainer_->index();
    sampleView_->setSample(sampleContainer_->sample(sampleContainer_->index()));
    sampleView_->show();

    emit indexChanged(index);

    update(QModelIndex());
}

void AMSampleContainerView::init()
{
    setWindowTitle("Sample Container");
    sampleView_ = new AMSampleEthanView();
}

