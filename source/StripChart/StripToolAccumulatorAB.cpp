///*
//Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
//Copyright 2013-2014 David Chevrier and Darren Hunter.

//This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

//Acquaman is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Acquaman is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
//*/


//#include "StripToolAccumulatorAB.h"

///// Constructor. \c outputName is the name() for the output data source.
//StripToolAccumulatorAB::StripToolAccumulatorAB(const QString& outputName, QObject* parent)
//    : AMStandardAnalysisBlock(outputName, parent)
//{
//    analyzedName_ = "";
//    canAnalyze_ = false;

//    dataSource_ = 0;
//    oldData_ = 0;

//    axes_ << AMAxisInfo("invalid", 0, "No input data");
//    setState(AMDataSource::InvalidFlag);
//}

///// This constructor is used to reload analysis blocks directly out of the database
//StripToolAccumulatorAB::StripToolAccumulatorAB(AMDatabase* db, int id)
//    : AMStandardAnalysisBlock("tempName")
//{

//    dataSource_ = 0;
//    dataTime_ = 0;

//    axes_ << AMAxisInfo("invalid", 0, "No input data");
//    setState(AMDataSource::InvalidFlag);

//    loadFromDb(db, id);
//        // will restore sumAxis, sumRangeMin, and sumRangeMax. We'll remain invalid until we get connected.

//    AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.
//}


//// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
///* - there must be a single input source or a list of 2D data sources
//    - the rank() of that input source must be 2 (two-dimensional)
//    */
//bool StripToolAccumulatorAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

//    if(dataSources.isEmpty())
//        return true;	// always acceptable; the null input.

//    // otherwise we need two input sources.
//    if(dataSources.count() == 2 && dataSources.at(0)->rank() == 0 && dataSources.at(1)->rank() == 2)
//        return true;

//    // Or we need a list of 2D data sources.
//    else if (dataSources.count() > 1){
//        return false;
//    }

//    return false;
//}


///// Set the data source inputs.
//void StripToolAccumulatorAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

//    // disconnect connections from old source, if it exists.
//    if(dataSource_) {
//        disconnect(dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        disconnect(dataSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
//        disconnect(dataSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
//        dataSource_ = 0;
//    }

////    if(oldData_) {
////		disconnect(oldData_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
////		disconnect(oldData_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
////		disconnect(oldData_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
////		oldData_ = 0;
////	}

//    if(dataSources.isEmpty()) {
//        dataSource_ = 0;
//        oldData_ = 0;
//        sources_.clear();
//        canAnalyze_ = false;

//        setDescription("-- No input data --");
//    }

//    // we know that this will only be called with valid input source
//    else if (dataSources.count() == 2) {
//        dataSource_ = dataSources.at(0);
//        oldData_ = dataSources.at(1);
//        sources_ = dataSources;
//        canAnalyze_ = true;

//        // Have to call into AMDataStore directly to avoid setModified(true)
//        AMDataSource::setDescription(QString("%2 has accumulated %1")
//                                     .arg(dataSource_->name())
//                                     .arg(oldData_->name()) );

//        connect(dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        connect(dataSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
//        connect(dataSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

//    }

//    else {

//        sources_ = dataSources;
//        setInputSource();
//    }

////	invalidateCache();
//    reviewState();

//    emitSizeChanged(0);
//    emitValuesChanged();
//    emitAxisInfoChanged(0);
//    emitInfoChanged();
//}

//void StripToolAccumulatorAB::setAnalyzedName(const QString &name)
//{
//    if(analyzedName_ == name)
//        return;
//    analyzedName_ = name;
//    setModified(true);
//    canAnalyze_ = canAnalyze(name);
//    setInputSource();

////	invalidateCache();
//    reviewState();

//    emitSizeChanged(0);
//    emitValuesChanged();
//    emitAxisInfoChanged(0);
//    emitInfoChanged();
//}

//void StripToolAccumulatorAB::setInputSource()
//{
//    // disconnect connections from old source, if it exists.
//    if(dataSource_) {
//        disconnect(dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        disconnect(dataSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
//        disconnect(dataSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
//        dataSource_ = 0;
//    }

//    int index = indexOfInputSource(analyzedName_);

//    if (index >= 0){

//        dataSource_ = inputDataSourceAt(index);
//        canAnalyze_ = true;

//        setDescription(QString("%2 has accumulated %1")
//                       .arg(dataSource_->name())
//                       .arg(oldData_->name()));

//        connect(dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        connect(dataSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
//        connect(dataSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

//    } else {

//        dataSource_ = 0;
//        oldData_ = 0;
//        canAnalyze_ = false;
//        setDescription("Accumulate");
//    }
//}

//bool StripToolAccumulatorAB::canAnalyze(const QString &name) const
//{
//    // Always can analyze a single 0D data source.
//    if (sources_.count() == 1)
//        return true;

//    if (indexOfInputSource(name) == 0)
//        return true;

//    return false;
//}

//AMNumber StripToolAccumulatorAB::value(const AMnDIndex& indexes) const {
//    if(indexes.rank() != 1)
//        return AMNumber(AMNumber::DimensionError);

//    if(!isValid())
//        return AMNumber(AMNumber::InvalidError);

//#ifdef AM_ENABLE_BOUNDS_CHECKING
//    if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
//        return AMNumber(AMNumber::OutOfBoundsError);
//#endif

//    double newVal = 0.0;
////    if(sumAxis_ == 0)
////        for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
////            newVal += (double)inputSource_->value(AMnDIndex(i, indexes.i()));
////    else
////        for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
////            newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), i));

//    return newVal;

//}
//#include <QDebug>
//bool StripToolAccumulatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
//{
//    if(indexStart.rank() != 1 || indexEnd.rank() != 1)
//        return false;

//    if(!isValid())
//        return false;

//    if (!canAnalyze())
//        return false;

//#ifdef AM_ENABLE_BOUNDS_CHECKING
//    if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
//        return false;
//#endif

//    int totalSize = indexEnd.i() - indexStart.i() + 1;
//    int offset = indexStart.i();

//    QVector<double> data = QVector<double>(totalSize);
//    oldData_->values(indexStart, indexEnd, data.data());

//    data[indexEnd.i() + 1] = dataSource_->value(AMnDIndex());

//    return true;
//}

//AMNumber StripToolAccumulatorAB::axisValue(int axisNumber, int index) const {

//    if(!isValid())
//        return AMNumber(AMNumber::InvalidError);

//    if(axisNumber != 0)
//        return AMNumber(AMNumber::DimensionError);

//    int otherAxis = (sumAxis_ == 0) ? 1 : 0;

//    return inputSource_->axisValue(otherAxis, index);
//}

//// Connected to be called when the values of the input data source change
//void StripToolAccumulatorAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {

//    if(start.isValid() && end.isValid()) {
//        int otherAxis = (sumAxis_ == 0) ? 1 : 0;
//        int startIndex = start.at(otherAxis);
//        int endIndex = end.at(otherAxis);
//        for(int i=startIndex; i<=endIndex; i++)
//            cachedValues_[i] = AMNumber();	// invalidate the changed region
//        emitValuesChanged(startIndex, endIndex);
//    }
//    else {
//        invalidateCache();
//        emitValuesChanged();
//    }

//    /// \todo start an idle-time computation of the cached values?
//}

///// Connected to be called when the size of the input source changes
//void StripToolAccumulatorAB::onInputSourceSizeChanged() {

//    int otherAxis = (sumAxis_ == 0) ? 1 : 0;
//    if(axes_.at(0).size != inputSource_->size(otherAxis)) {
//        axes_[0].size = inputSource_->size(otherAxis);
//        cachedValues_.resize(axes_.at(0).size);	// resize() will fill in with default-constructed value for AMNumber(), which is AMNumber::Null. So this keeps cache invalid (if was previously invalid), and nulls the new values so they are calculated as required.
//        emitSizeChanged(0);
//    }
//}

///// Connected to be called when the state() flags of any input source change
//void StripToolAccumulatorAB::onInputSourceStateChanged() {

//    // just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
//    onInputSourceSizeChanged();
//    reviewState();
//}

////#include "analysis/AM2DSummingABEditor.h"
////QWidget* AM2DSummingAB::createEditorWidget() {
////    return new AM2DSummingABEditor(this);
////}
