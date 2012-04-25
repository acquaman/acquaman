/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AM2DDEADTIMEAB_H
#define AM2DDEADTIMEAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts a 2D data source for a spectrum to adjust to deadtime as well as two 1D data soruces for OCR and ICR as input, and outputs a 2D data source by adjusting the original spectrum for the deadtime.
class AM2DDeadTimeAB: public AMStandardAnalysisBlock {
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=2D Dead Time Correction Block")
public:
	/// Constructor. \c outputName is the name() for the output data source.
	AM2DDeadTimeAB(const QString& outputName = "InvalidInput", QObject* parent = 0);
	/// This constructor is used to reload analysis blocks directly out of the database
	Q_INVOKABLE AM2DDeadTimeAB(AMDatabase* db, int id);

	QString infoDescription() const { return QString("(DeadTime Correction)"); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a three input sources
  - the rank() of the first input source must be 2 (two-dimensional) for the spectrum
  - the rank() of the second input source must be 1 (two-dimensional) for the ICR
  - the rank() of the third input source must be 1 (two-dimensional) for the OCR
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs. Order must be spectrum followed by ICR followed by OCR
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes, bool doBoundsChecking = true) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const;

	/// Re-implemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name()
	bool loadFromDb(AMDatabase *db, int id);

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// Cached previously-corrected values.  Either they don't need to be re-calculated, or they're AMNumber::Null and do need to be recalculated.
	mutable QVector<QVector<AMNumber> > cachedValues_;
	/// Optimization: invalidating the cache with invalid() requires clearing all values in it. If we've just done this, we can avoid re-doing it until there's actually something to clear.
	mutable bool cacheCompletelyInvalid_;

	AMDataSource* spectra_;	// our spectrum input source, or 0 if we don't have one.
	AMDataSource* icr_;	// our ICR input source, or 0 if we don't have one.
	AMDataSource* ocr_;	// our OCR input source, or 0 if we don't have one.

	/// helper function to clear the cachedValues_
	void invalidateCache();

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
};

#endif // AM2DDEADTIMEAB_H
