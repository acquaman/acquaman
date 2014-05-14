#include "STPlotView.h"

STPlotView::STPlotView(STModel *model, QObject *parent) :
    QObject(parent)
{
    model_ = 0;
}

STPlotView::~STPlotView()
{

}

STModel* STPlotView::model() const
{
    return model_;
}

void STPlotView::setModel(STModel *newModel)
{
    if (model_ != 0) {

    }

    model_ = newModel;

    emit modelChanged();
}
