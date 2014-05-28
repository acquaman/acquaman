//#include "STVectorDataCollection.h"

//#include <QDebug>

//STVectorDataCollection::STVectorDataCollection(const QString &name, QObject *parent) :
//    STDataCollection(name, parent)
//{
//    dataSize_ = 0;
//    storeSize_ = 10;
//    displaySize_ = 10;

//    timeModel_ = new QVector<QTime&>(storeSize_);
//    valueModel_ = new QVector<double>(storeSize_);
//    displayModel_ = new MPlotVectorSeriesData();
//}



//STVectorDataCollection::~STVectorDataCollection()
//{

//}



//int STVectorDataCollection::dataSize() const
//{
//    return dataSize_;
//}



//int STVectorDataCollection::storeSize() const
//{
//    return storeSize_;
//}



//int STVectorDataCollection::displaySize() const
//{
//    return displaySize_;
//}



//QVector<QTime&> STVectorDataCollection::timeModel() const
//{
//    return timeModel_;
//}



//QVector<double>* STVectorDataCollection::valueModel() const
//{
//    return valueModel_;
//}



//MPlotVectorSeriesData* STVectorDataCollection::displayModel() const
//{
//    return displayModel_;
//}



//void STVectorDataCollection::setStoreSize(int newStoreSize)
//{
//    if (newStoreSize >= 0) {
//        storeSize_ = newStoreSize;
//        emit storeSizeChanged(storeSize_);
//    }
//}



//void STVectorDataCollection::setDisplaySize(int newDisplaySize)
//{
//    if (newDisplaySize >= 0) {
//        displaySize_ = newDisplaySize;
//        emit displaySizeChanged(displaySize_);
//    }
//}



//void STVectorDataCollection::addData(QTime &time, double value)
//{
//    if (dataSize_ == 0 && storeSize_ <= 0) {
//        qDebug() << "STVectorDataCollection :: the time/value vectors are empty and storeSize_ is not large enough to add new data values. Cannot add new value.";
//        return;
//    }

//    if (dataSize_ <= storeSize_) {
//        modelsAddLast(time, value);

//    } else if (dataSize_ > 0 && dataSize_ > storeSize_) {
//        modelsRemoveFirst();
//        modelsShuffleDown();
//        modelsReplaceLast(time, value);

//    }

//}



//void STVectorDataCollection::onDataAdded()
//{
//    updateDisplayModel();
//}



//void STVectorDataCollection::modelsRemoveFirst()
//{
//    timeModel_->remove(0);
//    valueModel_->remove(0);
//}



//void STVectorDataCollection::modelsShuffleDown()
//{
////    for (int i = 1; i < dataSize_; i++) {
////        timeModel_[i - 1] = timeModel_->at(i);
////        valueModel_[i - 1] = valueModel_->at(i);
////    }
//}



//void STVectorDataCollection::modelsAddLast(QTime &time, double value)
//{
////    timeModel_[dataSize_] = time;
////    valueModel_[dataSize_] = value;

////    dataSize_++;

////    emit dataAdded();
//}



//void STVectorDataCollection::modelsReplaceLast(QTime &time, double value)
//{
////    timeModel_[dataSize_ - 1] = time;
////    valueModel_[dataSize_ - 1] = value;

////    emit dataAdded();
//}



//void STVectorDataCollection::updateDisplayModel()
//{

//}



