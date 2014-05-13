#include "AMSampleContainerView.h"

/*
#include "dataman/AMSampleContainer.h"
#include "ui/dataman/AMSampleView.h"
#include "dataman/AMSample.h"
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
    int index = sampleContainer_->currentIndex();
    sampleView_->setSample(sampleContainer_->sample(sampleContainer_->currentIndex()));
    sampleView_->show();

    emit indexChanged(index);

    update(QModelIndex());
}

void AMSampleContainerView::setCurrentSelection(int rowIndex)
{
    QModelIndex modelIndex = sampleContainer_->index(rowIndex);
    setCurrentIndex(modelIndex);
}

void AMSampleContainerView::init()
{
    setWindowTitle("Sample Container");
    sampleView_ = new AMSampleView();
}

*/
