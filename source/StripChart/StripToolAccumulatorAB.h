/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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

/*
#ifndef STRIPTOOLACCUMULATORAB_H
#define STRIPTOOLACCUMULATORAB_H


#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts a 0D data source and a 2D data source as input and outputs an updated 2D data source by adding the value to the end of the 2D source.
class StripToolAccumulatorAB : public AMStandardAnalysisBlock {
    Q_OBJECT

    Q_PROPERTY(QString analyzedName READ analyzedName WRITE setAnalyzedName)

    Q_CLASSINFO("AMDbObject_Attributes", "description=0D Accumulator Block")

public:
    /// Constructor. \c outputName is the name() for the output data source.
    StripToolAccumulatorAB(const QString& outputName, QObject* parent = 0);
    /// This constructor is used to reload analysis blocks directly out of the database
    Q_INVOKABLE StripToolAccumulatorAB(AMDatabase* db, int id);

    QString infoDescription() const { return QString("%2 accumulating %1").arg(oldData_->name()).arg(dataSource_->name()); }
*/
    /// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
    /*! - there must be a single input source or a list of 2D data sources.
  - the rank() of that input source must be 2 (two-dimensional)
  */
/*
    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

protected:
    /// Set the data source inputs.
    virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

public:
    /// Set the analyzed data source name.
    void setAnalyzedName(const QString &name);
    /// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
    QString analyzedName() const { return analyzedName_; }
    /// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
    bool canAnalyze() const { return canAnalyze_; }
    /// Returns whether the data source can be evaluated by passing in a name.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
    bool canAnalyze(const QString &name) const;

    // Creating editors for editing parameters
    ////////////////////////////////////
    /// Create, connect, and return a widget suitable for displaying/editing the expressions.
    virtual QWidget* createEditorWidget();



    // Data value access
    ////////////////////////////

    /// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
    virtual AMNumber value(const AMnDIndex& indexes) const;
    */
    /// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
    /*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

    \code
    int outputSize = indexStart.totalPointsTo(indexEnd);
    \endcode
    */
/*
    virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
    /// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
    virtual AMNumber axisValue(int axisNumber, int index) const;


    // Analysis parameters
    ///////////////////////////




signals:

public slots:

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

    AMDataSource* dataSource_;	// our single input source, or 0 if we don't have one.
    AMDataSource* oldData_;
    QTime *dataTime_;

	// COMMENTED OUT
    AMDataSource* accumulatorSource_;

	int sumAxis_;
	int sumRangeMin_, sumRangeMax_;
	//

    /// The name of the data source that should be analyzed.
    QString analyzedName_;
    /// Flag holding whether or not the data source can be analyzed.
    bool canAnalyze_;


    /// Helper function to look at our overall situation and determine what the output state should be.
    void reviewState() {

	if(!canAnalyze_ || dataSource_ == 0 || dataTime_ == 0 || !dataSource_->isValid()) {
	    setState(AMDataSource::InvalidFlag);
	    return;

	} else {
	    setState(0);
	}
    }


};

#endif // STRIPTOOLACCUMULATORAB_H
*/
