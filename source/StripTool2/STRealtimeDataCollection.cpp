#include "STRealtimeDataCollection.h"

STRealtimeDataCollection::STRealtimeDataCollection(const QString &name, QObject *parent) :
    STDataCollection(name, parent)
{
    displayModel_ = new MPlotRealtimeModel(this);
    connect( displayModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex, QModelIndex)) );
}



STRealtimeDataCollection::~STRealtimeDataCollection()
{

}



MPlotRealtimeModel* STRealtimeDataCollection::displayModel()
{
    return displayModel_;
}



void STRealtimeDataCollection::addData(QTime time, double value)
{
}
