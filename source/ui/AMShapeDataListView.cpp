#include "AMShapeDataListView.h"

#include "AMShapeDataSet.h"
#include "AMSampleView.h"

AMShapeDataListView::AMShapeDataListView(QWidget* parent)
    : QListView(parent)
{
}

void AMShapeDataListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    shapeModel()->setCurrentIndex(current.row());
    emit currentIndexChanged();
}

AMShapeDataSet *AMShapeDataListView::shapeModel()
{
    return (AMShapeDataSet*)model();
}
