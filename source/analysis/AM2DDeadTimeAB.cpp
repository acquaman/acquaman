/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AM2DDeadTimeAB.h"

#include <QDebug>

AM2DDeadTimeAB::AM2DDeadTimeAB(const QString &outputName, QObject *parent) :
		AMStandardAnalysisBlock(outputName, parent)
{
	spectra_ = 0; //NULL
	icr_ = 0; //NULL
	ocr_ = 0; //NULL

	cacheCompletelyInvalid_ = false;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

AM2DDeadTimeAB::AM2DDeadTimeAB(AMDatabase *db, int id) :
		AMStandardAnalysisBlock("tempname")
{
	spectra_ = 0; //NULL
	icr_ = 0; //NULL
	ocr_ = 0; //NULL

	cacheCompletelyInvalid_ = false;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);

	loadFromDb(db, id);
	AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.

}

bool AM2DDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const{
	if(dataSources.isEmpty())
		return true; //always acceptable, the null input

	//otherwise we need a spectra source (2D), an icr (1D), and an ocr (1D)
	if(dataSources.count() == 3 && dataSources.at(0)->rank() == 2
	   && dataSources.at(1)->rank() == 1 && dataSources.at(2)->rank() == 1)
		return true;

	return false;
}

void AM2DDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources){
	// disconnect connections from old sources, if they exist.
	if(spectra_) {
		disconnect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}
	if(icr_) {
		disconnect(icr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(icr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(icr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}
	if(ocr_) {
		disconnect(ocr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(ocr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(ocr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	if(dataSources.isEmpty()){
		spectra_ = 0; //NULL
		icr_ = 0; //NULL
		ocr_ = 0; //NULL
		sources_.clear();

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}
	else{
		spectra_ = dataSources.at(0);
		icr_ = dataSources.at(1);
		ocr_ = dataSources.at(2);
		sources_ = dataSources;

		axes_[0] = spectra_->axisInfoAt(0);
		axes_[1] = spectra_->axisInfoAt(1);

		setDescription(QString("Dead time correction of %1").arg(spectra_->name()));

		connect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(icr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(icr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(icr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(ocr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(ocr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(ocr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	invalidateCache();
	reviewState();

	emitSizeChanged(0);
	emitSizeChanged(1);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitAxisInfoChanged(1);
	emitInfoChanged();
}

AMNumber AM2DDeadTimeAB::value(const AMnDIndex& indexes, bool doBoundsChecking) const {
	if(indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(doBoundsChecking)
		if(((unsigned)indexes.i() >= (unsigned)axes_.at(0).size) || ((unsigned)indexes.j() >= (unsigned)axes_.at(1).size))
			return AMNumber(AMNumber::OutOfBoundsError);

	AMNumber rv = cachedValues_.at(indexes.i()).at(indexes.j());
	// if we haven't calculated this sum yet, the cached value will be invalid. Correct and store.
	if(!rv.isValid()) {
		double newVal = 0.0;	/// \todo preserve int/double nature of values
		if((int)spectra_->value(indexes, doBoundsChecking) != 0)
			newVal = (double(icr_->value(indexes.j())))/(double(ocr_->value(indexes.j())))*((int)spectra_->value(indexes));
		(cachedValues_[indexes.i()])[indexes.j()] = newVal;
		cacheCompletelyInvalid_ = false;
		return newVal;
	}
	// otherwise return the value we have.
	else
		return rv;
}

AMNumber AM2DDeadTimeAB::axisValue(int axisNumber, int index, bool doBoundsChecking) const {

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if((axisNumber != 0) || (axisNumber != 1) )
		return AMNumber(AMNumber::DimensionError);

	//int otherAxis = (sumAxis_ == 0) ? 1 : 0;

	return spectra_->axisValue(axisNumber, index, doBoundsChecking);
}

bool AM2DDeadTimeAB::loadFromDb(AMDatabase *db, int id){
	bool success = AMDbObject::loadFromDb(db, id);
	if(success)
		AMDataSource::name_ = AMDbObject::name(); /// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
	return success;
}

void AM2DDeadTimeAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end){
	if(start.isValid() && end.isValid()){
		if(start.i() == end.i()){
			int startIndex = start.j();
			int endIndex = end.j();
			for(int x = startIndex; x < endIndex; x++)
				(cachedValues_[start.i()])[x] = AMNumber(); //invalidate the changed region
		}
		else
			qDebug() << "Interesting, start and end have different i values " << start.i() << end.i();
	}
	else{
		invalidateCache();
		emitValuesChanged();
	}
}

void AM2DDeadTimeAB::onInputSourceSizeChanged(){
	if(axes_.at(0).size != spectra_->size(0)){
		axes_[0].size = spectra_->size(0);
		cachedValues_.resize(axes_.at(0).size);
		emitSizeChanged(0);
	}
	if(axes_.at(1).size != spectra_->size(1)){
		axes_[1].size = spectra_->size(1);
		for(int x = 0; x < axes_.at(0).size; x++)
			(cachedValues_[x]).resize(axes_.at(1).size);
		emitSizeChanged(1);
	}
}

void AM2DDeadTimeAB::onInputSourceStateChanged(){
	reviewState();

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
}

void AM2DDeadTimeAB::invalidateCache() {
	if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size || cachedValues_.at(0).size() != axes_.at(1).size) {
		for(int x = 0; x < axes_.at(0).size; x++)
			cachedValues_[x] = QVector<AMNumber>(axes_.at(1).size);	// everything in there is now AMNumber::Null.
		cacheCompletelyInvalid_ = true;
	}
}

void AM2DDeadTimeAB::reviewState() {

	if(spectra_ == 0 || !spectra_->isValid()
		|| icr_ == 0 || !icr_->isValid()
		|| ocr_ == 0 || !ocr_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	setState(0);
}
