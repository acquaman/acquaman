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


#ifndef AM1DINTEGRALAB_H
#define AM1DINTEGRALAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*! This analysis block accepts a single 1D input data source and calculates the integral.
	The output data is the same size as the input data source.  It uses the trapezoidal method
	of numerical integration.

	It can take a list of 1D data sources, but only analyzes the data source that matches the analyzed name.
	If only one 1D data source is provided then it will analyze it, even if the name does not match.
  */
class AM1DIntegralAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString analyzedName READ analyzedName WRITE setAnalyzedName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Integral Block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DIntegralAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - any number of 1D data sources or one 1D data source.
		- the rank() of that input source must be 1 (one-dimensional)
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Set the analyzed data source name.
	void setAnalyzedName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString analyzedName() const { return analyzedName_; }
	/// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
	bool canAnalyze() const { return canAnalyze_; }
	/// Returns whether the data source can be evaluated by passing in a name.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
	bool canAnalyze(const QString &name) const;

	// Data Retrieval

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;

	///////////////////////////////

	/// Reimplement the create widget method.
	QWidget *createEditorWidget();

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
	/// Helper method that sets the inputSource_ pointer to the correct one based on the current state of analyzedName_.
	void setInputSource();

	/// helper function to clear the cachedValues_
	void invalidateCache() {
		if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size) {
			cachedValues_ = QVector<AMNumber>(axes_.at(0).size);	// everything in there is now AMNumber::Null.
			cacheCompletelyInvalid_ = true;
		}
	}

	/// Cached previously-summed values.  Either they don't need to be re-calculated, or they're AMNumber::Null and do need to be recalculated.
	mutable QVector<AMNumber> cachedValues_;
	/// Optimization: invalidating the cache with invalid() requires clearing all values in it. If we've just done this, we can avoid re-doing it until there's actually something to clear.
	mutable bool cacheCompletelyInvalid_;

	/// Pointer to the data source that will be analyzed.
	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	/// The name of the data source that should be analyzed.
	QString analyzedName_;
	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
};

#endif // AM1DINTEGRALAB_H
