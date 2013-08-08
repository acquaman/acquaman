#include "AMShapeDataListView.h"

#include "AMShapeDataSet.h"
#include "AMSampleEthanView.h"

AMShapeDataListView::AMShapeDataListView(QWidget* parent)
    : QListView(parent)
{
    sampleView_ = new AMSampleEthanView();
}

void AMShapeDataListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    shapeModel()->setCurrentIndex(current.row());
    sampleView_->show();
    sampleView_->setSample(shapeModel()->currentShape()->sample());
    emit currentIndexChanged();
}

AMShapeDataSet *AMShapeDataListView::shapeModel()
{
    return (AMShapeDataSet*)model();
}
